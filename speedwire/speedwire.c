#include "speedwire.h"
#include <stdlib.h>
#include <assert.h>

static uint64_t be64_to_host(unsigned char* data) {
#if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
	return
			((uint64_t)data[7]<<0)  | ((uint64_t)data[6]<<8 ) |
			((uint64_t)data[5]<<16) | ((uint64_t)data[4]<<24) |
			((uint64_t)data[3]<<32) | ((uint64_t)data[2]<<40) |
			((uint64_t)data[1]<<48) | ((uint64_t)data[0]<<56);
#else
	return (uint64_t) *data;
#endif
}

static void print_header(struct speedwire_header* header) {
	printf("vendor: %s\n", header->vendor);
	printf("length: %hu\n", ntohs(header->length));
	printf("tag0: %x\n", ntohs(header->tag0));
	printf("group: %d\n", ntohl(header->group));
	printf("data length: %hu\n", ntohs(header->datalength));
	printf("tag: %d\n", ntohs(header->tag));
	printf("proto_id: %x\n", ntohs(header->proto_id));
	printf("susy_id: %x\n", ntohs(header->susy_id));
	printf("serial_no: %x\n", ntohl(header->serial_no));
	printf("meas_time: %ums\n", ntohl(header->meas_time));
}

static void print_obis_header(struct obis_header* header) {
	printf("channel: %x\n", header->channel);
	printf("index: %d\n", header->index);
	printf("type: %x\n", header->type);
	printf("tarif: %x\n", header->tarif);
}

void handle_packet(unsigned char* msgbuf, int nbytes, struct sockaddr_in* addr, int addrlen, speedwire_data_t* speedwire_data) {
	/* fixed length header */
	struct speedwire_header* header;
	struct obis_header* obis;

	header = (struct speedwire_header*) msgbuf;
	print_header(header);

	size_t offset = sizeof(struct speedwire_header); // 24
	printf("struct size: %ld\n", offset);

	while (offset < ntohs(header->datalength)+16) {
		uint32_t value;
		uint64_t counter;
		printf("== Offset %ld length: %d\n", offset, ntohs(header->datalength));
		obis = (struct obis_header*) &msgbuf[offset];
		print_obis_header(obis);
		offset += sizeof(struct obis_header);

		switch(obis->type) {
			case 0:
				/* version */
				printf("version: \n");
				offset += 4;
				break;
			case 4:
				/* actual */
				value = ntohl(*(uint32_t*)&msgbuf[offset]);
				printf("value %d\n", value);
				offset += 4;
				break;
			case 8:
				/* counter */

				printf("counter %ld\n", be64_to_host(&msgbuf[offset]));
				offset += 8;
				break;
			default:
				/* unknown */
				exit(-1);
		}
	}

	/* End marker 4 bytes */
	uint16_t end_length = ntohs(*(uint16_t*)&msgbuf[offset]);
	offset += 2;
	uint16_t end_marker = ntohs(*(uint16_t*)&msgbuf[offset]);
	offset += 2;
	assert(offset == nbytes);
	printf("End of packet %hu %hu offset: %ld\n", end_length, end_marker, offset);
}