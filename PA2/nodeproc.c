/*
 * nodeproc.c – Distributed Bellman-Ford node process
 *
 * Algorithm:
 *   Each node maintains (myRoot, myCost, parent, expTime).
 *   Messages are 128 bytes: buf[0] = link index, buf[1..127] = payload.
 *   Payload (network byte order, uint32_t each): root, cost, id, exp_ms.
 *
 * Timeouts (via select):
 *   HELLO_TIMEOUT  – send keep-alive if silent for this interval.
 *   ROOT_TIMEOUT   – if expTime reaches 0, revert to self as root.
 *   lifetime       – graceful shutdown.
 *
 * Usage:
 *   nodeproc <netproc_addr> <node_id> <lifetime> <cost1> [cost2 ...]
 */

#include <arpa/inet.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#include "bf.h"  /* PORT, HELLO_TIMEOUT, ROOT_TIMEOUT */

/* Milliseconds since CLOCK_REALTIME epoch */
static long long now_ms(void) {
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    return (long long)ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
}

/* Reliable send with MSG_NOSIGNAL */
static int sendall(int fd, const void *buf, size_t len) {
    const char *p = (const char *)buf;
    while (len) {
        ssize_t n = send(fd, p, len, MSG_NOSIGNAL);
        if (n <= 0) return -1;
        p += n; len -= (size_t)n;
    }
    return 0;
}

/*
 * Wire payload (bytes 1-16 of the 127-byte user area).
 * All fields in network byte order.
 */
struct __attribute__((packed)) bf_payload {
    uint32_t root;
    uint32_t cost;
    uint32_t id;
    uint32_t exp_ms; /* remaining root-info lifetime in milliseconds */
};

static int send_update(int fd, uint8_t link,
                       uint32_t root, uint32_t cost,
                       uint32_t id,   uint32_t exp_ms) {
    uint8_t buf[128];
    memset(buf, 0, sizeof buf);
    buf[0] = link;
    struct bf_payload *p = (struct bf_payload *)(buf + 1);
    p->root   = htonl(root);
    p->cost   = htonl(cost);
    p->id     = htonl(id);
    p->exp_ms = htonl(exp_ms);
    return sendall(fd, buf, sizeof buf);
}

/* Print state: parent == -1 means self is root → print "NULL" */
static void print_state(long long rel_ms,
                        uint32_t root, int32_t parent, uint32_t cost) {
    if (parent < 0)
        printf("time=%.01f\tRoot=%u\tparent=%s\tdistance=%u\n",
               (double)rel_ms / 1e3, root, "NULL", cost);
    else
        printf("time=%.01f\tRoot=%u\tparent=%d\tdistance=%u\n",
               (double)rel_ms / 1e3, root, parent, cost);
    fflush(stdout);
}

int main(int argc, char *argv[]) {
    if (argc < 5) {
        fprintf(stderr,
            "Usage: %s <netproc_addr> <node_id> <lifetime> <cost1> [cost2...]\n",
            argv[0]);
        return 1;
    }

    const char *addr_str = argv[1];
    uint32_t    my_id    = (uint32_t)atoi(argv[2]);
    int         lifetime = atoi(argv[3]);
    int         nlinks   = argc - 4;
    uint32_t   *costs    = malloc((size_t)nlinks * sizeof *costs);
    if (!costs) { perror("malloc"); return 1; }
    for (int i = 0; i < nlinks; i++)
        costs[i] = (uint32_t)atoi(argv[4 + i]);

    /* ---- connect to netproc ---- */
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s < 0) { perror("socket"); free(costs); return 1; }

    int one = 1;
    setsockopt(s, IPPROTO_TCP, TCP_NODELAY, &one, sizeof one);

    struct sockaddr_in srv = {0};
    srv.sin_family = AF_INET;
    srv.sin_port   = htons(PORT);
    if (inet_pton(AF_INET, addr_str, &srv.sin_addr) != 1) {
        fprintf(stderr, "Invalid address: %s\n", addr_str);
        close(s); free(costs); return 1;
    }
    if (connect(s, (struct sockaddr *)&srv, sizeof srv) < 0) {
        perror("connect"); close(s); free(costs); return 1;
    }

    /* send my ID (uint32_t, network byte order) */
    uint32_t id_net = htonl(my_id);
    if (sendall(s, &id_net, sizeof id_net) < 0) {
        perror("send id"); close(s); free(costs); return 1;
    }

    /* ---- initial state ---- */
    long long start_ms = now_ms();

    uint32_t my_root = my_id;
    uint32_t my_cost = 0;
    int32_t  parent  = -1;          /* -1 = no parent (self is root) */

    /* Absolute deadline for current root-info expiry.
     * For self-root we keep rolling it forward, so it effectively never fires. */
    long long exp_deadline = start_ms + (long long)ROOT_TIMEOUT * 1000;
    long long last_send    = start_ms;
    long long life_end     = start_ms + (long long)lifetime * 1000;

    /* Print initial state and send first hello */
    print_state(now_ms() - start_ms, my_root, parent, my_cost);
    send_update(s, 0xFF, my_root, my_cost, my_id,
                (uint32_t)(ROOT_TIMEOUT * 1000));
    last_send = now_ms();
    printf("time=%.01f\tMessage sent to all neighbors\n",
           (double)(last_send - start_ms) / 1e3);
    fflush(stdout);

    /* ================================================================ loop */
    for (;;) {
        long long now = now_ms();

        if (now >= life_end) {
            printf("time=%.01f\tLifetime expired. Shutting down.\n",
                   (double)(now - start_ms) / 1e3);
            fflush(stdout);
            break;
        }

        /* Next event: earliest of hello-due, root-expiry, lifetime-end */
        long long hello_due = last_send + (long long)HELLO_TIMEOUT * 1000;
        long long root_due  = (my_root == my_id) ? life_end : exp_deadline;
        long long next      = hello_due;
        if (root_due < next) next = root_due;
        if (life_end < next) next = life_end;

        long long wait = next - now;
        if (wait < 0) wait = 0;

        struct timeval tv = { .tv_sec  = wait / 1000,
                              .tv_usec = (wait % 1000) * 1000 };
        fd_set rfds;
        FD_ZERO(&rfds);
        FD_SET(s, &rfds);

        int ret = select(s + 1, &rfds, NULL, NULL, &tv);
        if (ret < 0) { perror("select"); break; }

        now = now_ms();

        /* ---- incoming message ---- */
        if (ret > 0 && FD_ISSET(s, &rfds)) {
            uint8_t buf[128];
            ssize_t n = recv(s, buf, 128, MSG_WAITALL);
            if (n <= 0) { if (n < 0) perror("recv"); break; }

            uint8_t             link  = buf[0];
            struct bf_payload  *pay   = (struct bf_payload *)(buf + 1);
            uint32_t r_root   = ntohl(pay->root);
            uint32_t r_cost   = ntohl(pay->cost);
            uint32_t r_exp_ms = ntohl(pay->exp_ms);

            /* Clamp to maximum valid expiry */
            if (r_exp_ms > (uint32_t)(ROOT_TIMEOUT * 1000))
                r_exp_ms = (uint32_t)(ROOT_TIMEOUT * 1000);

            if ((int)link >= nlinks) goto next_check; /* unknown link – ignore */

            /* Ignore echoes of our own root advertisement */
            if (r_root == my_id && my_root == my_id) goto next_check;

            uint32_t via_cost = r_cost + costs[(int)link];
            int changed = 0;

            if (r_root < my_root) {
                /* Better (smaller) root found */
                my_root      = r_root;
                my_cost      = via_cost;
                parent       = (int32_t)link;
                exp_deadline = now + (long long)r_exp_ms;
                changed      = 1;
            } else if (r_root == my_root) {
                long long new_deadline = now + (long long)r_exp_ms;
                if ((int32_t)link == parent) {
                    /* Update from our parent: refresh expiry (only extend) */
                    if (new_deadline > exp_deadline)
                        exp_deadline = new_deadline;
                    if (via_cost != my_cost) {
                        my_cost = via_cost;
                        changed = 1;
                    }
                } else if (via_cost < my_cost) {
                    /* Shorter path to same root via different neighbor */
                    my_cost      = via_cost;
                    parent       = (int32_t)link;
                    exp_deadline = new_deadline;
                    changed      = 1;
                }
            }

            if (changed) {
                now = now_ms();
                print_state(now - start_ms, my_root, parent, my_cost);

                uint32_t adv_exp = (my_root == my_id)
                    ? (uint32_t)(ROOT_TIMEOUT * 1000)
                    : (uint32_t)(exp_deadline - now_ms());
                send_update(s, 0xFF, my_root, my_cost, my_id, adv_exp);
                last_send = now_ms();
                printf("time=%.01f\tMessage sent to all neighbors\n",
                       (double)(last_send - start_ms) / 1e3);
                fflush(stdout);
            }
        }

    next_check:
        now = now_ms();

        /* ---- root expiry ---- */
        if (my_root != my_id && now >= exp_deadline) {
            my_root      = my_id;
            my_cost      = 0;
            parent       = -1;
            exp_deadline = now + (long long)ROOT_TIMEOUT * 1000;
            print_state(now - start_ms, my_root, parent, my_cost);
            send_update(s, 0xFF, my_root, my_cost, my_id,
                        (uint32_t)(ROOT_TIMEOUT * 1000));
            last_send = now_ms();
            printf("time=%.01f\tMessage sent to all neighbors\n",
                   (double)(last_send - start_ms) / 1e3);
            fflush(stdout);
            continue;
        }

        /* ---- hello timer ---- */
        if (now >= last_send + (long long)HELLO_TIMEOUT * 1000) {
            uint32_t adv_exp = (my_root == my_id)
                ? (uint32_t)(ROOT_TIMEOUT * 1000)
                : (uint32_t)(exp_deadline - now > 0 ? exp_deadline - now : 0);
            send_update(s, 0xFF, my_root, my_cost, my_id, adv_exp);
            last_send = now_ms();
            printf("time=%.01f\tMessage sent to all neighbors\n",
                   (double)(last_send - start_ms) / 1e3);
            fflush(stdout);
        }
    }

    close(s);
    free(costs);
    return 0;
}
