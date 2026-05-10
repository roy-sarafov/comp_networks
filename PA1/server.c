#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <pthread.h>
#include <time.h>
#include <sys/queue.h>

// Datagram format received from client
struct __attribute__((__packed__)) job_msg {
    uint32_t client_id;
    uint16_t job_index;
    uint32_t job_length;
};

// Queue node using sys/queue.h macros
struct job_node {
    uint32_t client_ip;
    uint16_t client_port;
    uint32_t client_id;
    uint16_t job_index;
    uint32_t job_length_ns;
    int64_t arrival_time_ns;
    STAILQ_ENTRY(job_node) entries;
};

STAILQ_HEAD(job_queue_head, job_node);

// Global shared state
struct {
    struct job_queue_head queue;
    pthread_mutex_t lock;
    pthread_cond_t cond;
    
    int current_q_size;
    int max_q_size;
    int target_jobs;
    int jobs_received;
    int64_t total_demand_ns;
    
    struct timespec start_time;
    int acceptor_done;
    
    int server_socket;
} server_state;

/*
 * get_elapsed_ns - Returns nanoseconds elapsed since server start.
 * Input:  none (reads server_state.start_time global)
 * Output: signed 64-bit elapsed time in nanoseconds
 */
int64_t get_elapsed_ns(void) {
    struct timespec now;
    clock_gettime(CLOCK_MONOTONIC, &now);
    int64_t elapsed = (now.tv_sec - server_state.start_time.tv_sec) * 1000000000LL;
    elapsed += (now.tv_nsec - server_state.start_time.tv_nsec);
    return elapsed;
}

/*
 * acceptor_thread_func - Acceptor thread: receives UDP datagrams and enqueues jobs.
 * Input:  arg - unused
 * Output: returns NULL on exit
 * Action: Loops until server_state.target_jobs datagrams are received. For each
 *         datagram, records arrival time, converts fields from network byte order,
 *         and inserts a new job_node at the tail of the queue (drop-tail if full).
 *         Dropped jobs still count toward jobs_received. Signals the worker thread
 *         via condition variable when a job is enqueued.
 */
void *acceptor_thread_func(void *arg) {
    (void)arg; // Unused
    struct job_msg buffer;
    struct sockaddr_in cliaddr;
    socklen_t len = sizeof(cliaddr);

    while (server_state.jobs_received < server_state.target_jobs) {
        ssize_t n = recvfrom(server_state.server_socket, &buffer, sizeof(buffer), 
                             0, (struct sockaddr *)&cliaddr, &len);
        if (n < 0) {
            perror("recvfrom failed");
            close(server_state.server_socket);
            exit(EXIT_FAILURE);
        }

        int64_t arrival = get_elapsed_ns();
        
        // Convert to host byte order
        uint32_t c_ip = ntohl(cliaddr.sin_addr.s_addr);
        uint16_t c_port = ntohs(cliaddr.sin_port);
        uint32_t c_id = ntohl(buffer.client_id);
        uint16_t j_idx = ntohs(buffer.job_index);
        uint32_t j_len = ntohl(buffer.job_length);

        pthread_mutex_lock(&server_state.lock);
        
        server_state.jobs_received++; // Even dropped jobs count towards total

        if (server_state.current_q_size < server_state.max_q_size) {
            // Queue has room, accept job
            struct job_node *new_job = malloc(sizeof(struct job_node));
            if (!new_job) {
                perror("malloc failed");
                pthread_mutex_unlock(&server_state.lock);
                close(server_state.server_socket);
                exit(EXIT_FAILURE);
            }
            new_job->client_ip = c_ip;
            new_job->client_port = c_port;
            new_job->client_id = c_id;
            new_job->job_index = j_idx;
            new_job->job_length_ns = j_len;
            new_job->arrival_time_ns = arrival;

            STAILQ_INSERT_TAIL(&server_state.queue, new_job, entries);
            server_state.current_q_size++;
            server_state.total_demand_ns += j_len;
            
            pthread_cond_signal(&server_state.cond);
        }
        // Else: drop tail policy (job discarded)

        pthread_mutex_unlock(&server_state.lock);
    }
    return NULL;
}

/*
 * worker_thread_func - Worker thread: dequeues and executes jobs.
 * Input:  arg - unused
 * Output: returns NULL on exit
 * Action: Waits on condition variable while queue is empty. Dequeues the head job,
 *         snapshots queue size and total demand (including the executing job), then
 *         releases the lock and sleeps for job_length_ns to simulate execution.
 *         Prints one TSV log line per job with timing and queue statistics.
 *         Exits when the queue is empty and the acceptor is done.
 */
void *worker_thread_func(void *arg) {
    (void)arg; // Unused

    while (1) {
        pthread_mutex_lock(&server_state.lock);
        
        while (server_state.current_q_size == 0 && !server_state.acceptor_done) {
            pthread_cond_wait(&server_state.cond, &server_state.lock);
        }

        if (server_state.current_q_size == 0 && server_state.acceptor_done) {
            pthread_mutex_unlock(&server_state.lock);
            break; // No more jobs incoming and queue is empty
        }

        // Dequeue first job
        struct job_node *job = STAILQ_FIRST(&server_state.queue);
        STAILQ_REMOVE_HEAD(&server_state.queue, entries);
        
        int current_q_size_snap = server_state.current_q_size;
        int64_t current_demand_snap = server_state.total_demand_ns;
        
        server_state.current_q_size--;
        server_state.total_demand_ns -= job->job_length_ns;
        
        pthread_mutex_unlock(&server_state.lock);

        // Execute job (simulate by sleeping)
        struct timespec req;
        req.tv_sec = job->job_length_ns / 1000000000L;
        req.tv_nsec = job->job_length_ns % 1000000000L;
        nanosleep(&req, NULL);

        int64_t departure = get_elapsed_ns();

        // Print exact formatting required
        printf("%08x:%04x\t%d:%d\t%ld\t%ld\t%d\t%ld\n",
               job->client_ip, job->client_port, job->client_id, job->job_index, 
               (long)job->arrival_time_ns, (long)departure, current_q_size_snap, (long)current_demand_snap);

        free(job);
    }
    return NULL;
}

/*
 * main - Entry point. Parses arguments, sets up UDP socket, starts threads.
 * Input:  argc/argv: port num_jobs q_size
 * Output: EXIT_SUCCESS on clean shutdown, EXIT_FAILURE on error
 * Action: Binds a UDP socket to the given port, initializes shared state,
 *         spawns acceptor and worker threads, waits for acceptor to finish,
 *         signals worker, joins both threads, then cleans up.
 */
int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s port num_jobs q_size\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int port = (int)strtol(argv[1], NULL, 10);
    server_state.target_jobs = (int)strtol(argv[2], NULL, 10);
    server_state.max_q_size = (int)strtol(argv[3], NULL, 10);
    server_state.jobs_received = 0;
    server_state.current_q_size = 0;
    server_state.total_demand_ns = 0;
    server_state.acceptor_done = 0;

    STAILQ_INIT(&server_state.queue);

    // Setup Socket
    if ((server_state.server_socket = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = INADDR_ANY;
    servaddr.sin_port = htons(port);

    if (bind(server_state.server_socket, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
        perror("Bind failed");
        close(server_state.server_socket);
        exit(EXIT_FAILURE);
    }

    if (pthread_mutex_init(&server_state.lock, NULL) != 0) {
        perror("pthread_mutex_init failed");
        close(server_state.server_socket);
        exit(EXIT_FAILURE);
    }
    if (pthread_cond_init(&server_state.cond, NULL) != 0) {
        perror("pthread_cond_init failed");
        close(server_state.server_socket);
        pthread_mutex_destroy(&server_state.lock);
        exit(EXIT_FAILURE);
    }

    // Start clock
    clock_gettime(CLOCK_MONOTONIC, &server_state.start_time);

    pthread_t acceptor, worker;
    if (pthread_create(&acceptor, NULL, acceptor_thread_func, NULL) != 0) {
        perror("pthread_create acceptor failed");
        close(server_state.server_socket);
        pthread_mutex_destroy(&server_state.lock);
        pthread_cond_destroy(&server_state.cond);
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&worker, NULL, worker_thread_func, NULL) != 0) {
        perror("pthread_create worker failed");
        close(server_state.server_socket);
        pthread_mutex_destroy(&server_state.lock);
        pthread_cond_destroy(&server_state.cond);
        exit(EXIT_FAILURE);
    }

    // Main thread waits for acceptor to finish receiving `num_jobs`
    pthread_join(acceptor, NULL);

    // Signal worker that acceptor is done
    pthread_mutex_lock(&server_state.lock);
    server_state.acceptor_done = 1;
    pthread_cond_broadcast(&server_state.cond);
    pthread_mutex_unlock(&server_state.lock);

    // Wait for worker to clear queue
    pthread_join(worker, NULL);

    close(server_state.server_socket);
    pthread_mutex_destroy(&server_state.lock);
    pthread_cond_destroy(&server_state.cond);

    return EXIT_SUCCESS;
}
