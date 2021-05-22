#ifndef SPEEDWIRE_SPEEDWIRE_H
#define SPEEDWIRE_SPEEDWIRE_H

#include <stdio.h>
#include <netinet/in.h>

struct speedwire_header {
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

typedef struct obis_header_t {
	uint8_t channel;
	uint8_t index;
	uint8_t type;
	uint8_t tarif;
} __attribute__((packed)) obis_header_t;

typedef struct orbis_data_t {
	struct orbis_data_t *next;
        char* property_name;
        union {
            uint32_t actual;
            uint64_t counter;
        };
} orbis_data_t;

typedef struct {

    orbis_data_t *orbis_data_list;
} speedwire_data_t;

char *lookup_channel_name(uint8_t type);

void handle_packet(const unsigned char *msgbuf, int nbytes, struct sockaddr_in *addr, int addrlen,
				   speedwire_data_t *speedwire_data);

#endif //SPEEDWIRE_SPEEDWIRE_H
