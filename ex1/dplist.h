/**
 * \author Jeffee Hsiung
 */

#ifndef _DPLIST_H_
#define _DPLIST_H_

typedef char element_t;

/**
 * dplist_t is a struct containing at least a head pointer to the start of the list;
 */
typedef struct dplist dplist_t;

typedef struct dplist_node dplist_node_t;

/* General remark on error handling
 * All functions below will:
 * - use assert() to check if memory allocation was successfully.
 */

/** Create and allocate memory for a new list
 * \return a pointer to a newly-allocated and initialized list.
 */
dplist_t *dpl_create();

/** Deletes all elements in the list
 * - Every list node of the list needs to be deleted. (free memory)
 * - The list itself also needs to be deleted. (free all memory)
 * - '*list' must be set to NULL.
 * \param list a double pointer to the list
 */
void dpl_free(dplist_t **list);

/** Returns the number of elements in the list.
 * - If 'list' is is NULL, -1 is returned.
 * \param list a pointer to the list
 * \return the size of the list
 */
int dpl_size(dplist_t *list);

/** Inserts a new list node containing an 'element' in the list at position 'index'
 * - the first list node has index 0.
 * - If 'index' is 0 or negative, the list node is inserted at the start of 'list'.
 * - If 'index' is bigger than the number of elements in the list, the list node is inserted at the end of the list.
 * - If 'list' is is NULL, NULL is returned.
 * \param list a pointer to the list
 * \param element the data that needs to be inserted
 * \param index the position at which the element should be inserted in the list
 * \return a pointer to the list or NULL
 */
dplist_t *dpl_insert_at_index(dplist_t *list, element_t element, int index);

/** Removes the list node at index 'index' from the list.
 * - The list node itself should always be freed.
 * - If 'index' is 0 or negative, the first list node is removed.
 * - If 'index' is bigger than the number of elements in the list, the last list node is removed.
 * - If the list is empty, return the unmodified list.
 * - If 'list' is is NULL, NULL is returned.
 * \param list a pointer to the list
 * \param index the position at which the node should be removed from the list
 * \return a pointer to the list or NULL
 */
dplist_t *dpl_remove_at_index(dplist_t *list, int index);

/** Returns a reference to the list node with index 'index' in the list.
 * - If 'index' is 0 or negative, a reference to the first list node is returned.
 * - If 'index' is bigger than the number of list nodes in the list, a reference to the last list node is returned.
 * - If the list is empty, NULL is returned.
 * - If 'list' is is NULL, NULL is returned.
 * \param list a pointer to the list
 * \param index the position of the node for which the reference is returned
 * \return a pointer to the list node at the given index or NULL
 */
dplist_node_t *dpl_get_reference_at_index(dplist_t *list, int index);

/** Returns the list element contained in the list node with index 'index' in the list.
 * - return is not returning a copy of the element with index 'index', i.e. 'element_copy()' is not used.
 * - If 'index' is 0 or negative, the element of the first list node is returned.
 * - If 'index' is bigger than the number of elements in the list, the element of the last list node is returned.
 * - If the list is empty, 0 is returned.
 * - If 'list' is NULL, 0 is returned.
 * \param list a pointer to the list
 * \param index the position of the node for which the element is returned
 * \return the element at the given index
 */
element_t dpl_get_element_at_index(dplist_t *list, int index);

/** Returns an index to the first list node in the list containing 'element'.
 * - the first list node has index 0.
 * - If 'element' is not found in the list, -1 is returned.
 * - If 'list' is NULL, -1 is returned.
 * \param list a pointer to the list
 * \param element the element to look for
 * \return the index of the element that matches 'element'
 */
int dpl_get_index_of_element(dplist_t *list, element_t element);

#endif  //DPLIST_H_
