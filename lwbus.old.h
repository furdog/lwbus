#include <stdint.h>
#include <assert.h>
#include <stdlib.h>
#include <string.h>

#define LWBUS_PAYLOAD_UNDEFINED 1
#define LWBUS_MAX_SERVICES 32
#define LWBUS_MAX_MEMORY 64

#define LWBUS_DEBUG_CALLS

struct lwbus_iterator {
	void *data;
};

/** Each lwbus_srv_write creates a packet. */
struct lwbus_packet {
	struct lwbus_packet *next; /** Next packet. */
	uint8_t  len;
	uint8_t  payload[LWBUS_PAYLOAD_UNDEFINED];
} __attribute__((packed));

struct lwbus_service {
	const char *name; /** Name of the service. */
	struct lwbus_packet *packets[2]; /** Pointer to packets. */
	size_t nonce;
};

struct lwbus {
	struct lwbus_service services[LWBUS_MAX_SERVICES];
	uint8_t              services_count;
	
	uint8_t dbuf[2][LWBUS_MAX_MEMORY]; /** Lwbus uses double buffering. */
	uint8_t wbuf_idx; /** Which buffer is currently writeable. */
	
	uint8_t allocated; /** Total memory occupied. */
	size_t nonce;
};

void lwbus_init(struct lwbus *self)
{	
	self->services_count = 0;

	self->wbuf_idx = 0;

	self->allocated = 0;
}

/** Returns service descriptor. */
int lwbus_srv_add(struct lwbus *self, const char *name)
{
	struct lwbus_service *srv;
	
	assert(self->services_count < LWBUS_MAX_SERVICES);
	
	srv             = &self->services[self->services_count];
	srv->name       = name;
	srv->packets[0] = NULL;
	srv->packets[1] = NULL;

	return self->services_count++;
}

/** Returns number of bytes written. */
int lwbus_srv_write(struct lwbus *self, int desc, void *data, size_t len)
{
	struct lwbus_service *srv;
	struct lwbus_packet  *pkt;
	uint8_t wbuf   = self->wbuf_idx;
	uint8_t pkt_sz = sizeof(struct lwbus_packet) - 1 + len;
	
	assert(desc < self->services_count);
	assert(self->allocated + pkt_sz <= LWBUS_MAX_MEMORY);
	
	srv = &self->services[desc];
	pkt = (struct lwbus_packet *)&self->dbuf[wbuf][self->allocated];
	
	if (srv->nonce != self->nonce) {
		pkt->next = NULL;
		srv->nonce = self->nonce;
	} else {
		pkt->next = srv->packets[wbuf];
		srv->packets[wbuf];
	}
	
	srv->packets[wbuf] = pkt;
	
	pkt->len = pkt_sz;
	memcpy(pkt->payload, data, len);
	self->allocated += pkt_sz;
	
	return pkt_sz;
}

struct lwbus_packet *lwbus_srv_read(struct lwbus *self, int desc,
				    struct lwbus_iterator *it)
{
	return NULL;
}

int lwbus_srv_find(struct lwbus *self, const char *name)
{
	return 0;
}

void lwbus_update(struct lwbus *self)
{
	return;
}

/** DEBUG */
#include <stdio.h>

#if defined(LWBUS_DEBUG_CALLS)
#define lwbus_init(a)\
{printf("%i: lwbus_init 0x%p\n", __LINE__, a);fflush(0);\
	lwbus_init(a);}

int lwbus_srv_add_dbg(struct lwbus *self, const char *name, int line)
{
	printf("%i: lwbus_srv_add 0x%p \"%s\"", line, self, name);
	fflush(0);
	printf(" = %i\n", lwbus_srv_add(self, name));
	fflush(0);
}
#define lwbus_srv_add(a, b) lwbus_srv_add_dbg(a, b, __LINE__)

int lwbus_srv_write_dbg(struct lwbus *self, int desc, void *data, size_t len,
			int line)
{
	printf("%i: lwbus_srv_write 0x%p %i 0x%p %i", line, self, desc, 
	       data, (int)len);
	fflush(0);
	printf(" = %i\n", lwbus_srv_write(self, desc, data, len));
	fflush(0);
}
#define lwbus_srv_write(a, b, c, d) lwbus_srv_write_dbg(a, b, c, d, __LINE__)

#endif

/** MAIN */
#include <stdio.h>
struct lwbus lwb;

/** Most basic service that starts, pushes "hello world!", then dies out. */
void simple_lwbus_service()
{
	/** Create new service and return its descriptor (0 means failure) */
	int srvds = lwbus_srv_add(&lwb, "simple/");

	/** Write some message into service. */
	lwbus_srv_write(&lwb, srvds, "hello world!", 13);

	/** Remove service from services list (queued to removal) */
	/* lwbus_srv_del(&lwb, srvds); //NOT IMPLEMENTED */
}

void simple_lwbus_service_listener()
{
	/** Service descriptor returned by "srv_find" is read only. */
	int srvds = lwbus_srv_find(&lwb, "simple/");
	
	/** Print one service message. */
	printf("%s", lwbus_srv_read(&lwb, srvds, NULL)->payload);
}

int main()
{	
	lwbus_init(&lwb);
	
	/** Run simple service. */
	simple_lwbus_service();
	
	/** Push all events into lbus. */
	lwbus_update(&lwb);
	
	/** Run simple service listener. */
	//simple_lwbus_service_listener();
}
