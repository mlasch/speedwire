#ifndef SPEEDWIRE_SPEEDWIRE_H
#define SPEEDWIRE_SPEEDWIRE_H

#include <stdio.h>
#include <netinet/in.h>

struct speedwire_header
{
	char vendor[4];
	uint32_t datalength;
	uint32_t group;
	uint16_t length;
	uint16_t tag;
	uint16_t proto_id;

} __attribute__((packed));

void handle_packet(char* msgbuf, int nbytes, struct sockaddr_in* addr, int addrlen);

#endif //SPEEDWIRE_SPEEDWIRE_H
