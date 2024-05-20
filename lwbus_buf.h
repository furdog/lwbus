/**
 * @file lwbus_buf.h 
 * @brief memory-efficient double-buffering solution.
 */

#ifndef LWBUS_BUF
#define LWBUS_BUF

#include "lwbus_deque.h"

struct lwbus_buf {
	struct lwbus_deque deque;
	
	/* size_t reserved_mem_len; */
	size_t swap_counter;
};

static void lwbus_buf_init(struct lwbus_buf *self, uint8_t *buf, size_t len)
{
	assert(self && buf && len);

	lwbus_deque_init(&self->deque, buf, len);
	
	/* self->reserved_mem_len = 0; */
	self->swap_counter     = 0;
}

/* static void lwbus_buf_reserve_memory(struct lwbus_buf *self, size_t len)
{
	assert(self->reserved_mem_len < self->deque.rear);
	
	self->reserved_mem_len = len;
}*/

static void lwbus_buf_swap(struct lwbus_buf *self)
{
	assert(self);

	self->swap_counter++;

	if (self->swap_counter % 2)
		lwbus_deque_invalidate_front(&self->deque);
	else
		lwbus_deque_invalidate_rear(&self->deque);
}

static size_t lwbus_buf_get_swap_counter(struct lwbus_buf *self)
{
	return self->swap_counter;
}

/** Writes to current "write" buffer. */
static void *lwbus_buf_write(struct lwbus_buf *self, void *data, size_t len)
{
	assert(self && data && len);
	
	if (self->swap_counter % 2)
		return lwbus_deque_push_front(&self->deque, data, len);

	return lwbus_deque_push_rear(&self->deque, data, len);
}

/** Returns pointer to current "read" buffer. 
 *  Be careful. Data pushed to rear buffer is in a reversed order. */
static void *lwbus_buf_read(struct lwbus_buf *self)
{
	assert(self);
	
	/* Warning! (direct access to class members) */
	return self->swap_counter % 2 ? &self->deque.buf[self->deque.rear] :
					 self->deque.buf;
				       
}

#endif /* LWBUS_BUF */
