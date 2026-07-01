
#ifndef CUTE_CONFIG_H
#define CUTE_CONFIG_H

#define CUTE_CONF_CALLSTACK_SIZE 1000

#define CUTE_CONF_SLOT_COUNT 256


//#define CUTE_CONF_DEBUG

#ifdef CUTE_CONF_DEBUG

#define CUTE_CONF_LOG_FILTER(DOMAIN) true

#define CUTE_LOG(DOMAIN, ...) \
    do { \
		if (CUTE_CONF_LOG_FILTER(DOMAIN)) {\
			printf("[LOG] (%s) ", DOMAIN); \
			printf(__VA_ARGS__); \
		} \
    } while (0)

#else

#define CUTE_LOG(DOMAIN, ...) do {} while (0)

#endif // CUTE_CONF_DEBUG

#endif // CUTE_CONFIG_H