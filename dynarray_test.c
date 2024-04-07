#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "dynarray.h"

void
test1()
{
	dynarray_t *da;
	int i;

	da = da_create(sizeof(int), NULL /* no copy */, NULL /* no clean */);

	for (i = 0; i < 5; i++) {
		int elem = i;
		da_append(da, &elem);
	}

	for (i = 0; i < 5; i++) {
		int elem = da_at_as(da, i, int);
		assert(elem == i);	
	}

	da_destroy(da);
}

typedef struct buf_s buf_t;

struct buf_s {
	int *data;
	int len;
};

void
buf_init_with_value(buf_t *b, int val, int len)
{
	int i;

	b->data = malloc(sizeof(int) * len);
	if (!b->data) abort();

	for (i = 0; i < len; i++) {
		b->data[i] = val;
	}
	b->len = len;
}

void
buf_init_from_buf(void *dst, void *src)
{
	buf_t *buf_dst = dst;
	buf_t *buf_src = src;

	buf_dst->len = buf_src->len;
	buf_dst->data = malloc(buf_src->len * sizeof(int));
	if (!buf_dst->data) abort();

	memcpy(buf_dst->data, buf_src->data, buf_src->len * sizeof(int));
}

void
buf_clean(void *b)
{
	buf_t *buf = b;
	if (!buf) return ;
	free(buf->data);
}

static void
test2()
{
	dynarray_t *da;

	buf_t b1, b2;
	buf_t tmp;

	buf_init_with_value(&b1, 1 /* value */, 1 /* len */);
	buf_init_with_value(&b2, 2 /* value */, 2 /* len */);

	da = da_create(sizeof(buf_t), buf_init_from_buf, buf_clean);

	da_append(da, &b1);
	da_append(da, &b2);

	tmp = da_at_as(da, 0, buf_t);
	assert(tmp.data[0] == 1);	

	tmp = da_at_as(da, 1, buf_t);
	assert(tmp.data[0] == 2);	
	assert(tmp.data[1] == 2);	

	buf_clean(&b2);
	buf_clean(&b1);

	da_destroy(da);
}

/*
 * Make a copy of the first element from dynamic array.
 */
static void
test3()
{
	dynarray_t *da;

	buf_t b1, b1_copy;
	buf_t *b_ptr;

	buf_init_with_value(&b1, 1, 1);

	da = da_create(sizeof(buf_t), buf_init_from_buf, buf_clean);

	da_append(da, &b1);

	b_ptr = &da_at_as(da, 0, buf_t);
	buf_init_from_buf(&b1_copy, b_ptr);
	assert(b1_copy.data[0] == 1);	

	buf_clean(&b1_copy);
	buf_clean(&b1);

	da_destroy(da);
}

int
main()
{
	test1();
	test2();
	test3();

	return 0;
}
