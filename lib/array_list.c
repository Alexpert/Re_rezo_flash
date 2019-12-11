#include "array_list.h"

#include <stdlib.h>
#include <stdio.h>

array_list_t *init_array_list(void)
{
	array_list_t *array_list = malloc(sizeof(array_list_t));
	array_list->data = malloc(sizeof(element_t *));
	array_list->current_index = 0;
	array_list->max_size = 1;

	return array_list;
}

void deinit_array_list(array_list_t * array_list)
{
	free(array_list->data);
	free(array_list);

	return;
}

void print_array_list(array_list_t * array_list)
{
	if (array_list->current_index == 0) {
		printf("[]\n");

		return;
	}

	printf("[");
	for (int i = 0; i < array_list->current_index - 1; i++)
		printf("%p, ", array_list->data[i]);

	printf("%p]\n", array_list->data[array_list->current_index - 1]);

	return;
}

void ensure_capacity(array_list_t * array_list, unsigned int size)
{
	unsigned int new_max = array_list->max_size;

	while (new_max <= size) {
		new_max *= 2;
	}

	if (new_max != array_list->max_size) {
		array_list->data =
		    realloc(array_list->data, new_max * sizeof(element_t *));
		array_list->max_size = new_max;
	}

	return;
}

void add_element(array_list_t * array_list, element_t * element)
{
	ensure_capacity(array_list, array_list->current_index);

	array_list->data[array_list->current_index++] = element;

	return;
}

void
insert_at(array_list_t * array_list, unsigned int index, element_t * element)
{
	int i;
	if (index > array_list->current_index) {
		ensure_capacity(array_list, index);
		array_list->current_index = index;
		array_list->data[array_list->current_index++] = element;
	} else {
		array_list->current_index++;
		ensure_capacity(array_list, array_list->current_index);
		i = array_list->current_index;
		while (i > index) {
			array_list->data[i] = array_list->data[i - 1];
			i--;
		}
		array_list->data[index] = element;
	}

	return;
}

int range_check(array_list_t * array_list, unsigned int index)
{
	return index < array_list->current_index;
}

element_t *get(array_list_t * array_list, unsigned int index)
{
	return range_check(array_list, index) ? array_list->data[index] : NULL;
}

element_t *remove_at(array_list_t * array_list, unsigned int index)
{
	unsigned int i = index;
	element_t *element = NULL;

	if (range_check(array_list, index)) {
		element = array_list->data[i];
		while (i < array_list->current_index - 1) {
			array_list->data[i] = array_list->data[i + 1];
			i++;
		}
		array_list->current_index = i;
	}

	return element;
}

element_t *remove_element(array_list_t * array_list, element_t * element)
{
	unsigned int i = 0;

	while (array_list->data[i] != element) {
		i++;
	}

	return remove_at(array_list, i);
}
