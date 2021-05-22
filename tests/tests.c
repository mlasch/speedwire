
#include <setjmp.h>
#include <speedwire.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <cmocka.h>

#define CAPTURED_PACKET_LEN 642
#define PCAP_ETH_IP_OFFSET 42

extern const unsigned char pkt0[600];
extern const unsigned char pkt1[CAPTURED_PACKET_LEN];

static void test_lookup_channel_name(void **state) {
    assert_string_equal(lookup_channel_name(2), "sum_effective_power_minus");
    assert_string_equal(lookup_channel_name(73), "l3_unknown");
}

static void test_header_pkt0(void **state) {
    struct speedwire_header *header;
    header = (struct speedwire_header *)pkt0;

    assert_string_equal(header->vendor, "SMA");
    assert_int_equal(ntohs(header->proto_id), 0x6069);
    assert_int_equal(ntohl(header->meas_time), 1985149010);
}

static void test_handle_pkt0(void **state) {
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);

    speedwire_data_t speedwire_data;
    speedwire_data.orbis_data_list = NULL;
    handle_packet(pkt0, 600, &addr, addrlen, &speedwire_data);


    orbis_data_t* obis_ptr = speedwire_data.orbis_data_list;
    while (obis_ptr != NULL) {

        printf("%s: %ld\n", obis_ptr->property_name, obis_ptr->counter);

        obis_ptr=obis_ptr->next;
    }
}

static void test_handle_pkt1(void **state) {
    struct sockaddr_in addr;
    int addrlen = sizeof(addr);

    speedwire_data_t speedwire_data;
    speedwire_data.orbis_data_list = NULL;
    handle_packet(pkt1 + PCAP_ETH_IP_OFFSET, CAPTURED_PACKET_LEN - PCAP_ETH_IP_OFFSET, &addr, addrlen, &speedwire_data);
    orbis_data_t* obis_ptr = speedwire_data.orbis_data_list;
    while (obis_ptr != NULL) {

        printf("%s: %ld\n", obis_ptr->property_name, obis_ptr->counter);


        obis_ptr=obis_ptr->next;
    }
}

int main(int argc, char *argv[]) {
    const struct CMUnitTest tests[] = {
        cmocka_unit_test(test_lookup_channel_name),
        cmocka_unit_test(test_header_pkt0),
        cmocka_unit_test(test_handle_pkt0),
        cmocka_unit_test(test_handle_pkt1),
    };

    int count_fail_tests = cmocka_run_group_tests(tests, NULL, NULL);

    return count_fail_tests;
}