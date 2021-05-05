#include "speedwire.h"

static void print_block(struct speedwire_header* header) {
	printf("size: %ld\n", sizeof(header));
	printf("vendor: %s\n", header->vendor);
	printf("datalength: %x\n", ntohl(header->datalength));
	printf("group: %d\n", ntohl(header->group));
	printf("length: %x\n", ntohs(header->length));
	printf("tag: %d\n", ntohs(header->tag));
	printf("proto_id: %x\n", ntohs(header->proto_id));
}

void handle_packet(char* msgbuf, int nbytes, struct sockaddr_in* addr, int addrlen) {
	struct speedwire_header* header;
	header = (struct speedwire_header*) msgbuf;
	print_block(header);
}