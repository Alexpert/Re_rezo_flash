#ifndef ARRAY_LIST_H
#define ARRAY_LIST_H

typedef void element_t;

typedef struct {
	element_t **data;
	unsigned int max_size;
	unsigned int current_index;
} array_list_t;

/**
	* Initialize a new array_list
	* @return Newly created array_list
	*/
array_list_t *
init_array_list(void);

/**
	* Free the list
	* @param array_list Array_list to destroy
	* @return void
	*/
void
deinit_array_list(array_list_t *array_list);

/**
	* Prints the list
	* @param array_list Array_list to print
	* @return void
	*/
void
print_array_list(array_list_t *array_list);

/**
	* Increases the capacity of this ArrayList instance, if necessary, to ensure
	* that it can hold at least the number of elements specified by
	* the minimum capacity argument.
	* @param size Minimum size to be hold by the list
	* @return void
	*/
void
ensure_capacity(array_list_t *array_list, unsigned int size);

/**
	* Appends element to the end of the list
	* @param array_list Array_list to add the element to
	* @param element Element added to the array_list
	* @return void
	*/
void
add_element(array_list_t *array_list, element_t *element);

/**
	* Inserts element to a specified index of the list
	* @param array_list Array_list to add the element to
	* @param index Index where the element must be inserted in the list
	* @param element Element added to the array_list
	* @return void
	*/
void
insert_at(array_list_t *array_list, unsigned int index, element_t *element);

/**
	* Get an element from a specied index
	* @param array_list Array_list to get the element from
	* @param index Index of the requested element
	* @return Element stored at index,
	*					NULL if not found
	*/
element_t *
get(array_list_t *array_list, unsigned int index);

/**
	* Removes the element at the specified position in this list. Shifts any
	* subsequent elements to the left (subtracts one from their indices).
	* @param array_list Array_list to remove the element from
	* @param index Index of the element to be removed
	* @return the element that was removed from the list,
	*					NULL if not found
	*/
element_t *
remove_at(array_list_t *array_list, unsigned int index);

/**
	* Removes the element in this list. Shifts any
	* subsequent elements to the left (subtracts one from their indices).
	* @param array_list Array_list to remove the element from
	* @param element Element to be removed
	* @return the element that was removed from the list,
	*					NULL if not found
	*/
element_t *
remove_element(array_list_t *array_list, element_t *element);

#endif
