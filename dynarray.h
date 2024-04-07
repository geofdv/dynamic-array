#ifndef DYNARRAY_SENTRY_H
#define DYNARRAY_SENTRY_H

#include <stddef.h>

#define min_cap 10

typedef void (*copy_t)(void *dst, void *src);
typedef void (*destroy_t)(void *);

typedef struct dynarray_s dynarray_t;

struct dynarray_s {
	char *data;
	size_t elemsz;
	size_t nelems;
	size_t cap;

	copy_t copy;
	destroy_t destroy;
};

/* Lifetime managment. */
dynarray_t *
da_create(size_t elemsz, copy_t copy, destroy_t destroy);

void
da_destroy(dynarray_t *da);

/* Modifiers. */
int
da_append(dynarray_t *da, void *elem);

/* State. */
#define da_at_as(da, i, type) \
	*((type *)&(da->data[(i) * (da->elemsz)]))

int
da_is_full(dynarray_t *da);

int
da_is_empty(dynarray_t *da);

#endif /* DYNARRAY_SENTRY_H */