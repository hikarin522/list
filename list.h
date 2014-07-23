// list.h
// source : list.c
// hikari

#pragma once
#ifndef LIST_H
#define LIST_H

// include
#include <stdbool.h>
#include <stdlib.h>
//#define NO_MALLOC

// LIST

// typedef & struct & enum
typedef struct LIST *list;
typedef struct LIST_CONTAINER *list_container;

typedef struct LIST_ITERATOR
{
	list mylist;
	list_container iterator;
} list_iterator;
typedef list_iterator list_reverse_iterator;

typedef enum SORT_PREDICATE
{
	LESS,
	GREATER
} sort_predicate;

// new & delete
#ifndef NO_MALLOC
void *list_default_allocator_new(size_t);
void list_default_allocator_free(void *);
list list_new(size_t data_size);
#endif

list list_new_set_allocator(size_t size, void *(*alloc_new)(size_t), void (*alloc_free)(void *));
bool list_delete(list);

void list_set_less(list, bool (*less)(void *, void *));
void list_set_greater(list, bool (*greater)(void *, void *));
void list_set_equal(list, bool (*equal)(void *, void *));

void *list_push_front(list);
void *list_push_back(list);
void *list_push(list_iterator *);
void *list_rpush(list_reverse_iterator *);
bool list_pop_front(list);
bool list_pop_back(list);
bool list_pop(list_iterator *);
bool list_rpop(list_reverse_iterator *);

// length & data type size
unsigned int list_length(list);
size_t list_size(list);

// iterator
list_iterator list_begin(list);
list_iterator list_end(list);
list_reverse_iterator list_rbegin(list);
list_reverse_iterator list_rend(list);
list_iterator list_next(list_iterator);
list_iterator list_previous(list_iterator);
list_reverse_iterator list_rnext(list_reverse_iterator);
list_reverse_iterator list_rprevious(list_reverse_iterator);
bool list_increment(list_iterator *);
bool list_decrement(list_iterator *);
bool list_rincrement(list_reverse_iterator *);
bool list_rdecrement(list_reverse_iterator *);
bool list_match(list_iterator, list_iterator);
bool list_rmatch(list_reverse_iterator, list_reverse_iterator);

// data
void *list_front(list);
void *list_back(list);
void *list_at(list_iterator);

bool insert(list_iterator *pos, list_iterator *insit);
bool swap(list_iterator *, list_iterator *);

// sort
bool list_rverse_sort(list);
bool list_quick_sort(list, sort_predicate);

#endif

