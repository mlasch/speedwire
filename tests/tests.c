#include <stdio.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <stdlib.h>
#include <speedwire.h>

#define CAPTURED_PACKET_LEN 642
#define PCAP_ETH_IP_OFFSET 42

extern const unsigned char pktX[600];
extern const unsigned char pkt1[CAPTURED_PACKET_LEN];

static void test_handle_packet(void **state)
{
	struct sockaddr_in addr;
	int addrlen = sizeof(addr);

	speedwire_data_t * speedwire_data;
	speedwire_data = malloc(sizeof(speedwire_data_t));

	handle_packet(pktX, 600, &addr, addrlen, speedwire_data);
	handle_packet(pkt1+PCAP_ETH_IP_OFFSET, CAPTURED_PACKET_LEN-PCAP_ETH_IP_OFFSET, &addr, addrlen, speedwire_data);
}

int main(int argc, char *argv[])
{
	const struct CMUnitTest tests[] =
	{
		cmocka_unit_test(test_handle_packet),
	};

	int count_fail_tests = cmocka_run_group_tests(tests, NULL, NULL);

	return count_fail_tests;
}