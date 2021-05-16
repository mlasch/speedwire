#ifndef SPEEDWIRE_SPEEDWIRE_H
#define SPEEDWIRE_SPEEDWIRE_H

#include <stdio.h>
#include <netinet/in.h>

struct speedwire_header
{
	char vendor[4]; //0
	uint16_t length; // 4
	uint16_t tag0; // 6
	uint32_t group; // 8
	uint16_t datalength; // 12
	uint16_t tag; //14
	uint16_t proto_id; //16
	// start of OBIS
	uint16_t susy_id; //18
	uint32_t serial_no;  //20
	uint32_t meas_time; // 24
} __attribute__((packed));

struct obis_header {
	uint8_t channel;
	uint8_t index;
	uint8_t type;
	uint8_t tarif;
} __attribute__((packed));

typedef struct {
	struct orbis_data_t* next;
} orbis_data_t;

typedef struct {
	struct speedwire_header * header;
	orbis_data_t * orbis_data_list;
} speedwire_data_t;

void handle_packet(char* msgbuf, int nbytes, struct sockaddr_in* addr, int addrlen, speedwire_data_t* speedwire_data);

#endif //SPEEDWIRE_SPEEDWIRE_H
