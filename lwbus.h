#include "lwbus_buf.h"

#define LWBUS_PAYLOAD_UNDEFINED 1

/** Prime lwbus service. Most basic, most simple, not much overhead. */
struct lwbus_srv_prime {
	enum    lwbus_srv_type;
	uint8_t payload[LWBUS_PAYLOAD_UNDEFINED];
} __attribute__((packed));

struct lwbus {
	/* List of lwbus services. */
	void *srv_list;
	
	/* Memory used by services. */
	struct lwbus_buf buf;
};
