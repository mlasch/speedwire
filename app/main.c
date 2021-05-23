#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include <pthread.h>
#include <curl/curl.h>

#include <speedwire.h>
#include <inserter.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#define SPEEDWIRE_PORT 9522
#define SPEEDWIRE_MULTICAST "239.12.255.254"
#define MSGBUFSIZE 2048

static void init_inserter_args(inserter_args_t* args) {
    args->batch_read_ptr = NULL;
    pthread_mutex_init(&args->mtx, NULL);
    pthread_cond_init(&args->cv, NULL);

    args->curl_handle = curl_easy_init();
}

static void destroy_inserter_args(inserter_args_t* args) {
    pthread_mutex_destroy(&args->mtx);
    pthread_cond_destroy(&args->cv);
}

int main(int argc, char *argv[]) {
    int fd = socket(AF_INET, SOCK_DGRAM, 0);
    if (fd < 0) {
        perror("socket");
        return 1;
    }
    /*
     * Allow multiple sockets to use the same port number.
     */
    if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &(int){1}, sizeof(int)) < 0) {
        perror("Reusing ADDR failed");
        return 1;
    }
    /*
     * Set port and bind.
     */
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = htonl(INADDR_ANY);
    addr.sin_port = htons(SPEEDWIRE_PORT);

    if (bind(fd, (struct sockaddr *)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }

    /*
     * Get interface index
     */
    const char* if_name = "eth0";
    uint32_t if_index = if_nametoindex(if_name);
    if (if_index == 0) {
        perror("Cannot resolve interface name");
        printf("No interface found for %s. Using INADDR_ANY\n", if_name);

    }

    /*
     * Kernel should join the multicast group.
     */
    struct ip_mreqn mreqn;
    mreqn.imr_multiaddr.s_addr = inet_addr(SPEEDWIRE_MULTICAST);
    mreqn.imr_address.s_addr = INADDR_ANY;
    mreqn.imr_ifindex = if_index; // NOLINT(cppcoreguidelines-narrowing-conversions)
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreqn, sizeof(mreqn)) < 0) {
        perror("setsockopt");
        return 1;
    }

    /*
     * Start inserter thread
     */
    inserter_args_t inserter_args;
    init_inserter_args(&inserter_args);
    pthread_t inserter_thread;
    int err = pthread_create(&inserter_thread, NULL, &influxdb_inserter, &inserter_args);
    if (err != 0) {
        printf("\ncan't create thread :[%s]", strerror(err));
        exit(1);
    }

    /*
     * Start packet collector
     */
    uint32_t packet_cnt = 0;
    speedwire_batch_t* batch_current = NULL;
    speedwire_batch_t* batch_head = NULL;
    for (;;) {
        const unsigned char msgbuf[MSGBUFSIZE];
        socklen_t addrlen = sizeof(addr);
        int nbytes = recvfrom(fd, msgbuf, MSGBUFSIZE, 0, (struct sockaddr *)&addr, &addrlen);
        if (nbytes < 0) {
            perror("recvfrom");
            return 1;
        }
        printf("Received %d bytes\n", nbytes);
        speedwire_data_t* speedwire_data_collect = malloc(sizeof(speedwire_data_t));
        speedwire_data_collect->obis_data_list = NULL;
        handle_packet(msgbuf, nbytes, &addr, addrlen, speedwire_data_collect);

        // add packet to batch
        batch_current = malloc(sizeof(speedwire_batch_t));
        batch_current->speedwire_data = speedwire_data_collect;
        batch_current->next = batch_head;
        batch_head = batch_current;
        batch_current = NULL;
        packet_cnt++;

        if (packet_cnt >= 10) {
            printf("Run inserter on collected batch\n");
            pthread_mutex_lock(&inserter_args.mtx);
            inserter_args.batch_read_ptr = batch_head;
            pthread_cond_signal(&inserter_args.cv);
            pthread_mutex_unlock(&inserter_args.mtx);

            batch_head = NULL;
            packet_cnt = 0;
        }
    }
    return 0;
}

#pragma clang diagnostic pop
