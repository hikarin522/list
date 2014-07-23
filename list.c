// list.c
// header : list.h
// hikari

#include "list.h"

// LIST

// struct
struct LIST_CONTAINER
{
	list_container next;
	list_container previous;
	void *data[];	//struct hack
};

struct LIST
{
	unsigned int length;
	size_t size;
	list_container dummy;
	
	bool (*less)(void *, void *);
	bool (*greater)(void *, void *);
	bool (*equal)(void *, void *);

	void *(*allocator_new)(size_t);
	void (*allocator_free)(void *);
};

// new & delete

#ifndef NO_MALLOC
void *list_default_allocator_new(size_t size)
{
	return calloc(1, size);
}

void list_default_allocator_free(void *p)
{
	free(p);
	return;
}

list list_new(size_t data_size)
{
	return list_new_set_allocator(data_size, list_default_allocator_new, list_default_allocator_free);
}
#endif

list list_new_set_allocator(size_t data_size, void *(*alloc_new)(size_t), void (*alloc_free)(void *))
{
	list data;
	if (data_size <= 0)
		return NULL;
	
	data = (list)alloc_new(sizeof(struct LIST));
	if (data == NULL)
		return NULL;
	
	data->dummy = (list_container)alloc_new(sizeof(struct LIST_CONTAINER));
	if (data->dummy == NULL)
	{
		alloc_free(data);
		return NULL;
	}
	
	data->dummy->next = data->dummy;
	data->dummy->previous = data->dummy;
	data->length = 0;
	data->size = data_size;
	data->allocator_new = alloc_new;
	data->allocator_free = alloc_free;
	data->less = NULL;
	data->greater = NULL;
	data->equal = NULL;

	return data;
}

bool list_delete(list data)
{
	list_container tmp;
	if (data == NULL)
		return false;
	
	for (tmp = data->dummy->next; tmp != data->dummy;)
	{
		list_container next = tmp->next;
		data->allocator_free(tmp);
		tmp = next;
	}
	
	data->allocator_free(data->dummy);
	data->allocator_free(data);
	return true;
}

void list_set_less(list data, bool (*less)(void *, void *))
{
	data->less = less;
	return;
}

void list_set_greater(list data, bool (*greater)(void *, void *))
{
	data->greater = greater;
	return;
}

void list_set_equal(list data, bool (*equal)(void *, void *))
{
	data->equal = equal;
	return;
}

void *list_push_back(list data)
{
	list_container tmp;
	if (data == NULL)
		return NULL;
	
	tmp = (list_container)data->allocator_new(sizeof(struct LIST_CONTAINER) + data->size);
	if (tmp == NULL)
		return NULL;
	
	tmp->previous = data->dummy->previous;
	tmp->next = data->dummy;
	data->dummy->previous->next = tmp;
	data->dummy->previous = tmp;
	data->length++;
	
	return tmp->data;
}

void *list_push_front(list data)
{
	list_container tmp;
	if (data == NULL)
		return NULL;
	
	tmp = (list_container)data->allocator_new(sizeof(struct LIST_CONTAINER) + data->size);
	if (tmp == NULL)
		return NULL;
	
	tmp->previous = data->dummy;
	tmp->next = data->dummy->next;
	data->dummy->next->previous = tmp;
	data->dummy->next = tmp;
	data->length++;
	
	return tmp->data;
}

void *list_push(list_iterator *pit)
{
	list_container tmp;
	if (pit == NULL || pit->iterator == NULL || pit->mylist == NULL)
		return NULL;
	
	tmp = (list_container)pit->mylist->allocator_new(sizeof(struct LIST_CONTAINER) + (pit->mylist)->size);
	if (tmp == NULL)
		return NULL;
	
	tmp->next = pit->iterator;
	tmp->previous = pit->iterator->previous;
	pit->iterator->previous = tmp;
	pit->iterator = tmp;
	pit->mylist->length++;
	
	return tmp->data;
}

void *list_rpush(list_reverse_iterator *pit)
{
	list_container tmp;
	if (pit == NULL || pit->iterator == NULL || pit->mylist == NULL)
		return NULL;
	
	tmp = (list_container)pit->mylist->allocator_new(sizeof(struct LIST_CONTAINER) + (pit->mylist)->size);
	if (tmp == NULL)
		return NULL;
	
	tmp->next = pit->iterator->next;
	tmp->previous = pit->iterator;
	pit->iterator->next->previous = tmp;
	pit->iterator->next = tmp;
	pit->iterator = tmp;
	pit->mylist->length++;
	
	return tmp->data;
}

bool list_pop_back(list data)
{
	list_container tmp;
	if (data == NULL || data->dummy->next == data->dummy)
		return false;
	
	tmp = data->dummy->previous->previous;
	data->allocator_free(data->dummy->previous);
	data->dummy->previous = tmp;
	tmp->next = data->dummy;
	data->length--;
	
	return true;
}

bool list_pop_front(list data)
{
	list_container tmp;
	if (data == NULL || data->dummy->next == data->dummy)
		return false;
	
	tmp = data->dummy->next->next;
	data->allocator_free(data->dummy->next);
	data->dummy->next = tmp;
	tmp->previous = data->dummy;
	data->length--;
	
	return true;
}

bool list_pop(list_iterator *pit)
{
	list_container tmp;
	
	if (pit == NULL || pit->iterator == NULL || pit->mylist == NULL)
		return false;
	
	pit->iterator->next->previous = pit->iterator->previous;
	pit->iterator->previous->next = pit->iterator->next;
	tmp = pit->iterator->next;
	pit->mylist->allocator_free(pit->iterator);
	pit->iterator = tmp;
	pit->mylist->length--;
	
	return true;
}

bool list_rpop(list_reverse_iterator *pit)
{
	list_container tmp;
	
	if (pit == NULL || pit->iterator == NULL || pit->mylist == NULL)
		return false;
	
	pit->iterator->next->previous = pit->iterator->previous;
	pit->iterator->previous->next = pit->iterator->next;
	tmp = pit->iterator->previous;
	pit->mylist->allocator_free(pit->iterator);
	pit->iterator = tmp;
	pit->mylist->length--;
	
	return true;
}

// length & data type size
unsigned int list_length(list data)
{
	return (data == NULL) ? 0 : data->length;
}

size_t list_size(list data)
{
	return (data == NULL) ? 0 : data->size;
}

// iterator
list_iterator list_begin(list data)
{
	list_iterator it;
	it.mylist = data;
	if (data == NULL)
		it.iterator = NULL;
	else
		it.iterator = data->dummy->next;

	return it;
}

list_iterator list_end(list data)
{
	list_iterator it;
	it.mylist = data;
	if (data == NULL)
		it.iterator = NULL;
	else
		it.iterator = data->dummy;
	
	return it;
}

list_reverse_iterator list_rbegin(list data)
{
	list_reverse_iterator it;
	it.mylist = data;
	if (data == NULL)
		it.iterator = NULL;
	else
		it.iterator = data->dummy->previous;
	
	return it;
}

list_reverse_iterator list_rend(list data)
{
	list_reverse_iterator it;
	it.mylist = data;
	if (data == NULL)
		it.iterator = NULL;
	else
		it.iterator = data->dummy;
	
	return it;
}

list_iterator list_next(list_iterator it)
{
	if (it.iterator != NULL && it.mylist != NULL && it.iterator != it.mylist->dummy)
	{
		it.iterator = it.iterator->next;
	}

	return it;
}

list_iterator list_previous(list_iterator it)
{
	if (it.iterator != NULL && it.mylist != NULL && it.iterator != it.mylist->dummy->next)
	{
		it.iterator = it.iterator->previous;
	}
	
	return it;
}

list_reverse_iterator list_rnext(list_reverse_iterator it)
{
	if (it.iterator != NULL && it.mylist != NULL && it.iterator != it.mylist->dummy)
	{
		it.iterator = it.iterator->previous;
	}

	return it;
}

list_reverse_iterator list_rprevious(list_reverse_iterator it)
{
	if (it.iterator != NULL && it.mylist != NULL && it.iterator != it.mylist->dummy->previous)
	{
		it.iterator = it.iterator->next;
	}
	
	return it;
}

bool list_increment(list_iterator *pit)
{
	if (pit == NULL || pit->iterator == NULL || pit->mylist == NULL || pit->iterator == pit->mylist->dummy)
		return false;

	pit->iterator = pit->iterator->next;

	return true;
}

bool list_decrement(list_iterator *pit)
{
	if (pit == NULL || pit->iterator == NULL || pit->mylist == NULL || pit->iterator == pit->mylist->dummy->next)
		return false;
		
	pit->iterator = pit->iterator->previous;

	return true;
}

bool list_rincrement(list_reverse_iterator *pit)
{
	if (pit == NULL || pit->iterator == NULL || pit->mylist == NULL || pit->iterator == pit->mylist->dummy)
		return false;

	pit->iterator = pit->iterator->previous;

	return true;
}

bool list_rdecrement(list_reverse_iterator *pit)
{
	if (pit == NULL || pit->iterator == NULL || pit->mylist == NULL || pit->iterator == pit->mylist->dummy->previous)
		return false;
		
	pit->iterator = pit->iterator->next;

	return true;
}

bool list_match(list_iterator ita, list_iterator itb)
{
	return (ita.iterator == itb.iterator);
}

bool list_rmatch(list_reverse_iterator ita, list_reverse_iterator itb)
{
	return (ita.iterator == itb.iterator);
}

// data
void *list_front(list data)
{
	if (data == NULL || data->dummy->next == data->dummy)
		return NULL;
	
	return data->dummy->next->data;
}

void *list_back(list data)
{
	if (data == NULL || data->dummy->previous == data->dummy)
		return NULL;
	
	return data->dummy->previous->data;
}

void *list_at(list_iterator it)
{
	if (it.iterator == NULL || it.mylist == NULL || it.iterator == it.mylist->dummy)
		return NULL;
	
	return it.iterator->data;
}

bool list_insert(list_iterator *pos, list_iterator *insit)
{
	list_container tmp;

	if (pos == NULL || insit == NULL || pos->iterator == NULL || pos->mylist == NULL || insit->iterator == NULL || insit->mylist == NULL || insit->iterator == insit->mylist->dummy)
		return false;
	
	tmp = insit->iterator->next;

	insit->iterator->previous->next = insit->iterator->next;
	insit->iterator->next->previous = insit->iterator->previous;

	insit->iterator->next = pos->iterator;
	insit->iterator->previous = pos->iterator->previous;
	
	pos->iterator->previous->next = insit->iterator;
	pos->iterator->previous = insit->iterator;
	pos->iterator = insit->iterator;

	insit->iterator = tmp;
	
	return true;
}

bool list_swap(list_iterator *ita, list_iterator *itb)
{
	list_iterator it;
	if (ita == NULL || itb == NULL || ita->iterator == NULL || ita->mylist == NULL || ita->iterator == ita->mylist->dummy || itb->iterator == NULL || itb->mylist == NULL || itb->iterator == itb->mylist->dummy)
		return false;
	
	it.iterator = ita->iterator->next;
	ita->iterator->next->previous = itb->iterator;
	itb->iterator->next->previous = ita->iterator;
	ita->iterator->next = itb->iterator->next;
	itb->iterator->next = it.iterator;

	it.iterator = ita->iterator->previous;
	ita->iterator->previous->next = itb->iterator;
	itb->iterator->previous->next = ita->iterator;
	ita->iterator->previous = itb->iterator->previous;
	itb->iterator->previous = it.iterator;

	it = *ita;
	*ita = *itb;
	*itb = it;

	return true;
}

// sort
bool list_reverse_sort(list data)
{
	list_container it;
	if (data == NULL || data->length <= 0)
		return false;

	for (it = data->dummy->next; it != data->dummy;)
	{
		list_container tmp = it->next;
		it->next = it->previous;
		it->previous = tmp;
		it = tmp;
	}

	it = data->dummy->next;
	data->dummy->next = data->dummy->previous;
	data->dummy->previous = it;

	return true;
}

list_iterator list_quick_sort_pivot(list_iterator itbegin, list_iterator itend, bool (*predicate)(void *, void *))
{
	list_iterator it2 = list_next(itbegin);
	list_iterator it3 = itend;
	
	if (predicate(list_at(itbegin), list_at(it2)))
	{
		if (predicate(list_at(itbegin), list_at(it3)))
			return predicate(list_at(it2), list_at(it3)) ? it2 : it3;
		else
			return it3;
	}
	else
	{
		if (predicate(list_at(it2), list_at(it3)))
			return predicate(list_at(itbegin), list_at(it3)) ? itbegin : it3;
		else
			return it2;
	}
}

#define QS_SIZE 32
bool list_quick_sort(list data, sort_predicate pred)
{
	bool (*predicate)(void *, void *) = NULL;
	list_iterator stack_begin[QS_SIZE];
	list_iterator stack_end[QS_SIZE];
	int stack_it;
	
	if (data == NULL || data->length <= 0)
		return false;

	if (pred == LESS)
		predicate = data->less;
	else if (pred == GREATER)
		predicate = data->greater;
	
	if (predicate == NULL)
		return false;
	
	stack_begin[0] = list_begin(data);
	stack_end[0] = list_previous(list_end(data));
	stack_it = 0;
	
	while (stack_it >= 0)
	{
		int la = 0, lb = 0;
		list_iterator it;
		list_iterator pivot;
		list_iterator next = list_next(stack_begin[stack_it]);

		if (list_match(stack_begin[stack_it], stack_end[stack_it]))
		{
			stack_it--;
			continue;
		}
		
		if (list_match(next, stack_end[stack_it]))
		{
			if (predicate(list_at(next), list_at(stack_begin[stack_it])))
				list_swap(&stack_end[stack_it], &stack_begin[stack_it]);
			stack_it--;
			continue;
		}
		
		pivot = list_quick_sort_pivot(stack_begin[stack_it], stack_end[stack_it], predicate);
		for (it = stack_begin[stack_it]; !list_match(it, stack_end[stack_it]); list_increment(&it))
		{
			lb = 1;
			if (predicate(list_at(pivot), list_at(it)))
			{
				list_iterator rit;
				bool s = true;
				lb = 0;
				for (rit = stack_end[stack_it]; !list_match(rit, it); list_decrement(&rit))
				{
					if (!predicate(list_at(pivot), list_at(rit)))
					{
						if (list_match(stack_begin[stack_it], it))
							stack_begin[stack_it] = rit;
						if (list_match(stack_end[stack_it], rit))
							stack_end[stack_it] = it;
						list_swap(&it, &rit);
						s = false;
						break;
					}
					lb++;
				}
				if (s == true)
					break;
			}
			la++;
		}
	
		if (lb < la)
		{
			stack_end[stack_it + 1] = stack_end[stack_it];
			stack_end[stack_it] = list_previous(it);
			stack_begin[stack_it + 1] = it;
		}
		else
		{
			stack_begin[stack_it + 1] = stack_begin[stack_it];
			stack_begin[stack_it] = it;
			stack_end[stack_it + 1] = list_previous(it);
		}

		stack_it++;
	}
	
	return true;
}

