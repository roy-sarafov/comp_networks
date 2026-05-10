#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <math.h>
#include <time.h>

// Struct for the UDP datagram. Packed to avoid memory padding issues over the network.
struct __attribute__((__packed__)) job_msg {
    uint32_t client_id;
    uint16_t job_index;
    uint32_t job_length;
};

/*
 * randexp - Samples an exponential random variable.
 * Input:  lambda - rate parameter (must be > 0)
 * Output: a non-negative double drawn from Exp(lambda)
 */
double randexp(double lambda) {
    double u = rand() / ((double) RAND_MAX + 1.0);
    return -log(1.0 - u) / lambda;
}

/*
 * main - Entry point. Parses arguments and generates/sends num_jobs UDP job datagrams.
 * Input:  argc/argv: ip port num_jobs seed lambda mu
 * Output: EXIT_SUCCESS on completion, EXIT_FAILURE on error
 * Action: Seeds the RNG, creates a UDP socket, then for each job: samples inter-arrival
 *         time x ~ Exp(lambda) and job length y ~ Exp(mu), sleeps floor(x*1e6) ns,
 *         sends a packed datagram (client_id, job_index, job_length) to the server,
 *         and logs one TSV line to stdout.
 */
int main(int argc, char *argv[]) {
    // The instructions specified: "client ip port num_jobs seed lambda mu"
    // which makes 1 command + 6 args = 7 arguments
    if (argc != 7) {
        fprintf(stderr, "Usage: %s ip port num_jobs seed lambda mu\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    // Parse command line arguments
    const char *ip_str = argv[1];
    int port = (int)strtol(argv[2], NULL, 10);
    int num_jobs = (int)strtol(argv[3], NULL, 10);
    int seed = (int)strtol(argv[4], NULL, 10);
    double lambda = strtod(argv[5], NULL);
    double mu = strtod(argv[6], NULL);

    srand(seed);
    uint32_t client_id = (uint32_t)getpid();

    // Setup socket
    int sockfd;
    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    if (inet_pton(AF_INET, ip_str, &servaddr.sin_addr) <= 0) {
        perror("Invalid IP address");
        close(sockfd);
        exit(EXIT_FAILURE);
    }

    uint32_t ip_hex = ntohl(servaddr.sin_addr.s_addr);

    for (int i = 0; i < num_jobs; i++) {
        // Sample random variables
        double x = randexp(lambda);
        double y = randexp(mu);

        uint32_t floor_x = (uint32_t)floor(x * 1000000.0);
        uint32_t floor_y = (uint32_t)floor(y * 1000000.0);

        // Sleep for floor_x nanoseconds
        struct timespec req;
        req.tv_sec = floor_x / 1000000000L;
        req.tv_nsec = floor_x % 1000000000L;
        nanosleep(&req, NULL);

        // Prepare datagram in network byte order
        struct job_msg msg;
        msg.client_id = htonl(client_id);
        msg.job_index = htons((uint16_t)i);
        msg.job_length = htonl(floor_y);

        // Send UDP datagram
        if (sendto(sockfd, &msg, sizeof(msg), 0, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
            perror("Sendto failed");
            close(sockfd);
            exit(EXIT_FAILURE);
        }

        // Write log to stdout
        printf("%08x:%04x\t%d:%d\t%d\t%d\n", ip_hex, (unsigned int)port, client_id, i, floor_x, floor_y);
    }

    close(sockfd);
    return EXIT_SUCCESS;
}
