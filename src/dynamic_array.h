#ifndef DYNAMIC_ARRAY_H
#define DYNAMIC_ARRAY_H
#include "core.h"

#define MIN_DARRAY_CAPACITY 4
#define DARRDEF static inline

typedef struct
{
    u32 size;
    u32 capacity;
}dArrayHeader_t;

DARRDEF dArrayHeader_t* _darray_header(void* array)
{
    dArrayHeader_t* headers = (dArrayHeader_t*)array;
    return &headers[-1];
}

DARRDEF void _darray_free(void* array)
{
    if (!array) return;

    dArrayHeader_t* realptr = _darray_header(array);

    qu_free(realptr);
}

DARRDEF void* _darray_new(size_t capacity, size_t element_size)
{
    dArrayHeader_t* header = (dArrayHeader_t*)qu_malloc(sizeof(dArrayHeader_t) + (capacity * element_size));
    header->size = 0;
    header->capacity = capacity;

    return &header[1];
}

DARRDEF void* _darray_grow_to_cap(void* array, size_t newcap, size_t element_size)
{
    if (!array)
        return _darray_new(newcap, element_size);

	dArrayHeader_t* header = _darray_header(array);

	if (header->capacity >= newcap)
		return array;

	header->capacity = newcap;
	header = qu_realloc(header, sizeof(dArrayHeader_t) + (header->capacity * element_size));

	return &header[1];
}

DARRDEF void* _darray_maybe_grow(void* array, size_t element_size)
{
    if (!array)
        return _darray_new(MIN_DARRAY_CAPACITY, element_size);

    dArrayHeader_t* header = _darray_header(array);

    if (header->size < header->capacity)
        return array;

    array = _darray_grow_to_cap(array, header->capacity * 2, element_size);

	return array;
}

#define darray_push(ARR, VAL) ARR = _darray_maybe_grow(ARR, sizeof(*ARR)); (ARR)[_darray_header(ARR)->size++] = (VAL)
#define darray_size(ARR) ((ARR) ? _darray_header(ARR)->size : 0)
#define darray_free(ARR) _darray_free(ARR); ARR = NULL
#define darray_set_capacity(ARR, CAP) ARR = _darray_grow_to_cap(ARR, CAP, sizeof(*ARR))
#define darray_back(ARR) &((ARR)[_darray_header(ARR)->size-1])
#define darray_back_cpy(ARR) (ARR)[_darray_header(ARR)->size-1]
#define darray_delete(ARR, INDEX) ARR[INDEX] = darray_back_cpy(ARR); _darray_header(ARR)->size--
#define darray_clear(ARR) (ARR) ? _darray_header(ARR)->size = 0 : 0

#ifdef DARRAY_TESTS
#include <stdio.h>

DARRDEF void darray_test_growing(void)
{
    printf("DArray test growing:\n");
    int* array = NULL;

    loop_j0(12)
    {
        darray_push(array, j + 1);
        loop_i0(darray_size(array))
        {
            printf("%d ", array[i]);
        }
        printf("\nsize %d / cap %d\n", _darray_header(array)->size, _darray_header(array)->capacity);
    }

    darray_free(array);
}

DARRDEF void darray_test_prealloc(void)
{
    printf("DArray test prealloc:");
    int* array = NULL;

    darray_set_capacity(array, 8);

    loop_j0(9)
    {
        darray_push(array, j + 1);
        loop_i0(darray_size(array))
        {
            printf("%d ", array[i]);
        }
        printf("\nsize %d / cap %d\n", _darray_header(array)->size, _darray_header(array)->capacity);
    }

    darray_free(array);
}

DARRDEF void darray_test_delete(void)
{
    printf("DArray test delete:");
    int* array = NULL;

    darray_set_capacity(array, 16);

    loop_j0(16)
    {
        darray_push(array, j + 1);
    }

	loop_i0(darray_size(array))
	{
		printf("%d ", array[i]);
	}
    printf("\n");

    darray_delete(array, 0);
    loop_i0(darray_size(array))
    {
        printf("%d ", array[i]);
    }
    printf("\n");

    darray_delete(array, 2);
    loop_i0(darray_size(array))
    {
        printf("%d ", array[i]);
    }
    printf("\n");

    darray_delete(array, 3);
    loop_i0(darray_size(array))
    {
        printf("%d ", array[i]);
    }
    printf("\n");

    darray_free(array);
}

DARRDEF void darray_test_drive(void)
{
    darray_test_growing();
	darray_test_prealloc();
    darray_test_delete();
}

#endif
#endif
