/**
 * @file lwbus_buf_test.c 
 */

#include <stdio.h>
#include "lwbus_buf.h"

char test_msg[]  = "Test message";
char test_msg2[] = "Overriden message";
	
#define TEST_MEMORY sizeof(test_msg2) + sizeof(test_msg) * 2

int main()
{
	static uint8_t mem[TEST_MEMORY] = {0}; /* Filled by zeros. */
	struct lwbus_buf test;

	void *writ_mem;
	void *read_mem;

	/* --- Test normal. --- */
	lwbus_buf_init(&test, mem, TEST_MEMORY);

	/* Write some stuff into "read" memory. */
	writ_mem = lwbus_buf_write(&test, test_msg, sizeof(test_msg));
	assert(writ_mem);
	
	read_mem = lwbus_buf_read(&test);
	assert(read_mem);
	
	/* Not yet in "read" buffer, so should not be comparable. */
	assert(strcmp(writ_mem, read_mem));

	/* Do swap, so memory gets into "read" buffer. */
	assert(lwbus_buf_get_swap_counter(&test) == 0);
	lwbus_buf_swap(&test);
	assert(lwbus_buf_get_swap_counter(&test) == 1);

	read_mem = lwbus_buf_read(&test);
	assert(read_mem);

	/* Now it is. */
	assert(!strcmp(writ_mem, read_mem));

	/* Write something different into memory. */
	writ_mem = lwbus_buf_write(&test, test_msg2, sizeof(test_msg2));
	assert(writ_mem);
	
	/* "read" memory must remain intact. */
	read_mem = lwbus_buf_read(&test);
	assert(read_mem);
	assert(!strcmp(test_msg, read_mem));
	
	lwbus_buf_swap(&test);

	/* Now it changes. */
	read_mem = lwbus_buf_read(&test);
	assert(read_mem);
	assert(!strcmp(test_msg2, read_mem));
	
	/* Test overflow. */
	while(lwbus_buf_write(&test, test_msg, sizeof(test_msg)));

	/* "read" memory must remain intact even after overflow. */
	read_mem = lwbus_buf_read(&test);
	assert(read_mem);
	assert(!strcmp(test_msg2, read_mem));
	
	lwbus_buf_swap(&test);

	/* Ensure that that multiple values was actually written. */
	read_mem = lwbus_buf_read(&test);
	assert(read_mem);
	assert(!strcmp(test_msg, read_mem));
	assert(!strcmp(test_msg, (char *)read_mem + sizeof(test_msg)));

	assert(lwbus_buf_get_swap_counter(&test) == 3);
	
	printf("All tests passed successfully!\n");

	return 0;
}
