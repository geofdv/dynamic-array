#include <assert.h>
#include <stdlib.h>
#include <string.h> /* memcpy */

#include "dynarray.h"

#define address_of(da, i) \
	&((da)->data[(i) * (da)->elemsz])

#define address_of_last(da) \
	address_of(da, (da)->nelems)

dynarray_t *
da_create(size_t elemsz, copy_t copy, clean_t clean)
{
	dynarray_t *da;

	assert((copy && clean) || (!copy && !clean));

	da = malloc(sizeof(*da));
	if (!da)
		return NULL;

	da->data = NULL;
	da->elemsz = elemsz;
	da->nelems = 0;
	da->cap = 0;

	da->copy = copy;
	da->clean = clean;

	return da;
}

void
da_destroy(dynarray_t *da)
{
	if (!da)
		return ;

	if (da->clean) {
		int i;
		for (i = 0; i < da->nelems; i++) {
			da->clean(address_of(da, i));
		}
	}

	free(da->data);
	free(da);
}

static int
resize(dynarray_t *da)
{
	int i;

	assert(da);

	int new_cap = da->nelems == 0 ? 1 : 2 * da->cap;
	char *new_data = malloc(da->elemsz * new_cap);
	if (!new_data)
		return -1;

	/* need to copy old data to new place */
	for (i = 0; i < da->nelems; i++) {
		if (da->copy) {
			da->copy(&new_data[i * da->elemsz], &da->data[i * da->elemsz]);
		} else {
			memcpy(&new_data[i * da->elemsz], &da->data[i * da->elemsz], da->elemsz);
		}
	}

	/* need to clean old data */
	if (da->clean) {
		int i;
		for (i = 0; i < da->nelems; i++) {
			da->clean(address_of(da, i));
		}
	}

	free(da->data);

	da->data = new_data;
	da->cap = new_cap;

	return 0;
}

int
da_put_at(dynarray_t *da, int i, void *src)
{
	if (!da)
		return -1;

	/* clean object if needed */
	if (da->clean) {
		da->clean(address_of(da, i));
	}

	/* make copy at ith position */
	if (da->copy) {
		da->copy(address_of(da, i), src);	
	} else {
		memcpy(address_of(da, i), src, da->elemsz);
	}

	return 0;
}

int
da_get_at(dynarray_t *da, int i, void *dst)
{
	if (!da)
		return -1;

	/* clean dst object if needed */
	if (da->clean) {
		da->clean(dst);
	}

	/* make copy at ith position */
	if (da->copy) {
		da->copy(dst, address_of(da, i));	
	} else {
		memcpy(dst, address_of(da, i), da->elemsz);
	}

	return 0;
}

int
da_append(dynarray_t *da, void *elem)
{
	if (!da)
		return -1;

	if (da_is_full(da)) {
		int retval = resize(da);
		if (retval == -1) {
			return -1;
		}
	}

	if (da->copy) {
		da->copy(address_of_last(da), elem);
	} else {
		memcpy(address_of_last(da), elem, da->elemsz);
	}

	da->nelems++;

	return 0;
}

int
da_is_full(dynarray_t *da)
{
	if (!da)
		return 0;
	return da->nelems == da->cap;
}

int
da_is_empty(dynarray_t *da)
{
	if (!da)
		return 1;
	return da->nelems == 0;
}

