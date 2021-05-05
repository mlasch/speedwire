#include <stdio.h>
#include <setjmp.h>
#include <stddef.h>
#include <cmocka.h>
#include <speedwire.h>

extern const unsigned char pkt1[642];

static void test_handle_packet(void **state)
{
	printf("%c\n", pkt1[43]);
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