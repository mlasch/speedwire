#include <arpa/inet.h>
#include <inttypes.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include <speedwire.h>

#pragma clang diagnostic push
#pragma ide diagnostic ignored "EndlessLoop"

#define SPEEDWIRE_PORT 9522
#define SPEEDWIRE_MULTICAST "239.12.255.254"
#define MSGBUFSIZE 2048

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
     * Kernel should join the multicast group.
     */
    struct ip_mreq mreq;
    mreq.imr_multiaddr.s_addr = inet_addr(SPEEDWIRE_MULTICAST);
    mreq.imr_interface.s_addr = htonl(INADDR_ANY);
    if (setsockopt(fd, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0) {
        perror("setsockopt");
        return 1;
    }
    for (;;) {
        char msgbuf[MSGBUFSIZE];
        int addrlen = sizeof(addr);
        int nbytes = recvfrom(fd, msgbuf, MSGBUFSIZE, 0, (struct sockaddr *)&addr, &addrlen);
        if (nbytes < 0) {
            perror("recvfrom");
            return 1;
        }
        printf("Received %d bytes\n", nbytes);
        speedwire_data_t speedwire_data;
        speedwire_data.orbis_data_list = NULL;
        handle_packet(msgbuf, nbytes, &addr, addrlen, &speedwire_data);
        orbis_data_t* obis_ptr = speedwire_data.orbis_data_list;
        while (obis_ptr != NULL) {

            printf("%s: %ld\n", obis_ptr->property_name, obis_ptr->counter);

            obis_ptr=obis_ptr->next;
        }
    }
    return 0;
}

#pragma clang diagnostic pop