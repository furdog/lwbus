/**
 * @file lwbus_deque.h 
 * @brief deque implementation (only core methods used by lwbus)
 */

#ifndef LWBUS_DEQUE
#define LWBUS_DEQUE

#include <assert.h>
#include <stdint.h>
#include <string.h>

struct lwbus_deque {
	uint8_t *buf;
	uint8_t  len;
    
	size_t front;
	size_t rear;
};

static void lwbus_deque_init(struct lwbus_deque *self,
			     uint8_t *buf, size_t len)
{
	assert(self && buf && len);

	self->buf   = buf;
	self->len   = len;
	
	self->front = 0;
	self->rear  = len;
}

static void *lwbus_deque_push_front(struct lwbus_deque *self,
				    void *data, size_t len)
{
	void *result = NULL;
	
	assert(self && data && len);

	if ((self->front + len) > self->rear)
		return result;
	
	result = &self->buf[self->front];
	self->front += len;
	memcpy(result, data, len);

	return result;
}


static void* lwbus_deque_push_rear(struct lwbus_deque *self,
				   void *data, size_t len)
{
	void *result = NULL;
	
	assert(self && data && len);

	if ((self->front + len) > self->rear) /** Still correct here. */
		return NULL;

	self->rear -= len;
	result = &self->buf[self->rear];
	memcpy(result, data, len);

	return result;
}

static size_t lwbus_deque_pop_front(struct lwbus_deque *self, size_t until)
{
	assert(self);
	assert(until <= self->front);
	self->front = 0 + until;
	
	return self->front;
}

static size_t lwbus_deque_pop_rear(struct lwbus_deque *self, size_t until)
{
	assert(self);
	assert(until <= (self->len - self->rear));
	self->rear = self->len - until;

	return self->len - self->rear;
}

static void lwbus_deque_invalidate_front(struct lwbus_deque *self)
{
	assert(self);
	lwbus_deque_pop_front(self, 0);
}

static void lwbus_deque_invalidate_rear(struct lwbus_deque *self)
{
	assert(self);
	lwbus_deque_pop_rear(self, 0);
}

#endif /* LWBUS_DEQUE */
