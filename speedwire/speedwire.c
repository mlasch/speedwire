#include "speedwire.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

static void print_header(struct speedwire_header *header) {
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

static void print_obis_header(obis_header_t *header) {
    printf("channel: %x\n", header->channel);
    printf("index: %d\n", header->index);
    printf("type: %x\n", header->type);
    printf("tarif: %x\n", header->tarif);
}

void handle_packet(const unsigned char *msgbuf, int nbytes, struct sockaddr_in *addr, int addrlen,
                   speedwire_data_t *speedwire_data) {
    /* fixed length header */
    struct speedwire_header *header;
    //obis_data_t *orbis_data = NULL;
    obis_data_t *obis_data_new;

    header = (struct speedwire_header *)msgbuf;
//    print_header(header);

    size_t offset = sizeof(struct speedwire_header); // 24

    clock_gettime(CLOCK_REALTIME, &speedwire_data->timestamp);
    while (offset < ntohs(header->datalength) + 16) {
        obis_header_t *obis;
        const char *name;

        obis = (obis_header_t *)&msgbuf[offset];
//        print_obis_header(obis);
        offset += sizeof(obis_header_t);

        switch (obis->type) {
        case 0:
            /* version */
//            printf("version: %x\n", ntohl(*(uint32_t *)&msgbuf[offset]));
            offset += 4;
            break;
        case 4:
            /* actual */
            name = lookup_channel_name(obis->index);
//            printf("-> %s: value %d\n", name, ntohl(*(uint32_t *)&msgbuf[offset]));

            obis_data_new = malloc(sizeof(obis_data_t));
            // TODO: error handling
            if (obis_data_new == NULL) exit(-1);
            obis_data_new->actual = ntohl(*(uint32_t *)&msgbuf[offset]);
            asprintf(&obis_data_new->property_name, "%s_act", name);
            obis_data_new->next = speedwire_data->obis_data_list;
            speedwire_data->obis_data_list = obis_data_new;
            obis_data_new = NULL;
            offset += 4;
            break;
        case 8:
            /* counter */
            name = lookup_channel_name(obis->index);
//            printf("%s: counter %ld\n", name, be64toh(*(uint64_t *)&msgbuf[offset]));

            obis_data_new = malloc(sizeof(obis_data_t));
            // TODO: error handling
            if (obis_data_new == NULL) exit(-1);
            obis_data_new->counter = be64toh(*(uint64_t *)&msgbuf[offset]);
            asprintf(&obis_data_new->property_name, "%s_cnt", name);
            obis_data_new->next = speedwire_data->obis_data_list;
            speedwire_data->obis_data_list = obis_data_new;
            obis_data_new = NULL;
            offset += 8;
            break;
        default:
            /* unknown */
            exit(-1);
        }
    }

    /* End marker 4 bytes */
    uint16_t end_length = ntohs(*(uint16_t *)&msgbuf[offset]);
    offset += 2;
    uint16_t end_marker = ntohs(*(uint16_t *)&msgbuf[offset]);
    offset += 2;
    assert(offset == nbytes);
    printf("End of packet %hu %hu offset: %ld\n", end_length, end_marker, offset);
}

void speedwire_free_obis_data_list(obis_data_t* obis_data_list) {
    //    obis_data_t* obis_list_ptr = data->obis_data_list;
    obis_data_t* delete_ptr = NULL;
    while (obis_data_list) {
        delete_ptr = obis_data_list;
        obis_data_list = obis_data_list->next;
        free(delete_ptr->property_name);
        free(delete_ptr);
    }
}

void speedwire_free_data(speedwire_data_t* data) {
    speedwire_free_obis_data_list(data->obis_data_list);
    free(data);
}

void speedwire_free_batch(speedwire_batch_t* batch) {
    speedwire_batch_t* delete_ptr = NULL;

    while (batch) {
//        printf("Delete Batch\n");
        delete_ptr = batch;
        speedwire_free_data(batch->speedwire_data);
        batch = batch->next;

        free(delete_ptr);
    }
}
