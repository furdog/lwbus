/**
 * @file lwbus_deque_test.c 
 */

#include <stdio.h>
#include "lwbus_deque.h"

int main()
{
	uint8_t mem[64];
	struct lwbus_deque test;

	uint8_t test_msg[] = "Test message";

	void *retval;

	/* Test normal. */
	lwbus_deque_init(&test, mem, 64);

	retval = lwbus_deque_push_front(&test, test_msg, sizeof(test_msg));
	assert(retval);
	assert(!strcmp(retval, "Test message"));

	retval = lwbus_deque_push_rear(&test, test_msg, sizeof(test_msg));
	assert(retval);
	assert(!strcmp(retval, "Test message"));
	
	/* Test front vs rear. */
	lwbus_deque_init(&test, mem, 1);
	
	retval = lwbus_deque_push_front(&test, test_msg, 1);
	assert(retval);
	assert(*(char *)retval == 'T');

	retval = lwbus_deque_push_rear(&test, test_msg, 1);
	assert(!retval);

	/* Test rear vs front. */	
	lwbus_deque_init(&test, mem, 1);
	
	retval = lwbus_deque_push_rear(&test, test_msg, 1);
	assert(retval);
	assert(*(char *)retval == 'T');
	
	retval = lwbus_deque_push_front(&test, test_msg, 1);
	assert(!retval);
	
	/* Test front overflow. */	
	lwbus_deque_init(&test, mem, 1);
	
	retval = lwbus_deque_push_front(&test, test_msg, 1);
	assert(retval);
	assert(*(char *)retval == 'T');
	
	retval = lwbus_deque_push_front(&test, test_msg, 1);
	assert(!retval);

	/* Test rear overflow. */	
	lwbus_deque_init(&test, mem, 1);
	
	retval = lwbus_deque_push_rear(&test, test_msg, 1);
	assert(retval);
	assert(*(char *)retval == 'T');

	retval = lwbus_deque_push_rear(&test, test_msg, 1);
	assert(!retval);

	/* Test poppers. */
	lwbus_deque_init(&test, mem, 1);
	retval = lwbus_deque_push_front(&test, test_msg, 1);
	/* assert(lwbus_deque_pop_front(&test, 2) == 1); not valid yet */
	assert(lwbus_deque_pop_front(&test, 1) == 1);
	assert(lwbus_deque_pop_front(&test, 0) == 0);
	
	lwbus_deque_init(&test, mem, 1);
	retval = lwbus_deque_push_rear(&test, test_msg, 1);
	/* assert(lwbus_deque_pop_rear(&test, 2) == 1); not valid yet */
	assert(lwbus_deque_pop_rear(&test, 1) == 1);
	assert(lwbus_deque_pop_rear(&test, 0) == 0);

	/* Test invalidators. */
	lwbus_deque_init(&test, mem, 1);
	retval = lwbus_deque_push_front(&test, test_msg, 1);
	lwbus_deque_invalidate_front(&test);
	retval = lwbus_deque_push_front(&test, test_msg, 1);
	assert(retval);

	lwbus_deque_init(&test, mem, 1);
	retval = lwbus_deque_push_rear(&test, test_msg, 1);
	lwbus_deque_invalidate_rear(&test);
	retval = lwbus_deque_push_rear(&test, test_msg, 1);
	assert(retval);

	printf("All tests passed successfully!\n");

	return 0;
}
