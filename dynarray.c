#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h> /* memcpy */

#include "dynarray.h"

dynarray_t *
da_create(size_t elemsz, copy_t copy, destroy_t destroy)
{
	dynarray_t *da;

	da = malloc(sizeof(*da));
	if (!da)
		return NULL;

	da->data = NULL;
	da->elemsz = elemsz;
	da->nelems = 0;
	da->cap = 0;

	da->copy = copy;
	da->destroy = destroy;

	return da;
}

void
da_destroy(dynarray_t *da)
{
	if (!da)
		return ;

	if (da->destroy) {
		int i;
		for (i = 0; i < da->nelems; i++) {
			void *elem = &da->data[i * da->elemsz];
			da->destroy(elem);
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

	/* need to destroy old data */
	if (da->destroy) {
		int i;
		for (i = 0; i < da->nelems; i++) {
			void *elem = &da->data[i * da->elemsz];
			da->destroy(elem);
		}
	}

	free(da->data);

	da->data = new_data;
	da->cap = new_cap;

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
		da->copy(&da->data[da->nelems * da->elemsz], elem);
	} else {
		memcpy(&da->data[da->nelems * da->elemsz], elem, da->elemsz);
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

