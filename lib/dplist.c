/**
 * \author Jeffee Hsiung
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

/*
 * definition of error codes
 * */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1 // error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2 //error due to a list operation applied on a NULL list

#ifdef DEBUG
#define DEBUG_PRINTF(...) 									                                        \
        do {											                                            \
            fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);	    \
            fprintf(stderr,__VA_ARGS__);								                            \
            fflush(stderr);                                                                         \
                } while(0)
#else
#define DEBUG_PRINTF(...) (void)0
#endif


#define DPLIST_ERR_HANDLER(condition, err_code)                         \
    do {                                                                \
            if ((condition)) DEBUG_PRINTF(#condition " failed\n");      \
            assert(!(condition));                                       \
        } while(0)


/*
 * The real definition of struct list / struct node
 */

struct dplist_node {
	dplist_node_t *prev, *next;
	void* element;
};

struct dplist {
	dplist_node_t* head;
	void* (*element_copy)(void* src_element); //declaring a function pointer, functions are declared and defined in test
	void (*element_free)(void** elemenent);
	int (*element_compare)(void* x, void* y);
};


dplist_t* dpl_create(// callback functions
	void *(*element_copy)(void* src_element),
	void (*element_free)(void** element),
	int (*element_compare)(void* x, void* y)
) {
	dplist_t* list;
	list = malloc(sizeof(struct dplist)); //on heap, shall be freed later by ptrofptr
	DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR);
	list->head = NULL;
	list->element_copy = element_copy;
	list->element_free = element_free;
	list->element_compare = element_compare;
	return list;
}

void dpl_free(dplist_t** dbptr, bool free_element) {
	// check if dbptr and *dbptr->head is NULL. if not null, free all nodes
	dplist_node_t* node = (*dbptr)->head;
	dplist_node_t* next_node;
	if (dbptr == NULL) return;
	if (*dbptr == NULL) return;
	while (node != NULL) {
		next_node = node->next;
		if (free_element) {
			(*dbptr)->element_free(&(node->element));
		}
		free(node);
		node = next_node;
	}
	free(*dbptr);
	*dbptr = NULL;
}

dplist_t* dpl_insert_at_index(dplist_t* list, void* element, int index, bool insert_copy) {
	dplist_node_t* ref_at_index;
	dplist_node_t* list_node;
    if(list == NULL) return NULL;
    list_node = malloc(sizeof(dplist_node_t)); //list_node needs to be freed
	DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
	if(insert_copy){
		list_node->element = list->element_copy(element);
		free(element);
	}else{
		list_node->element = element;
	} // on heap: list_node, my_element_t* copy = list_node->element
	if (list->head == NULL) { // covers case 1, empty, free bird
		list_node->prev = NULL;
		list_node->next = NULL;
		list->head = list_node;
	} else if (index <= 0) { // covers case 2, insert into beginning
		list_node->prev = NULL;
		list_node->next = list->head;
		list->head->prev = list_node;
		list->head = list_node;
	} else {
		ref_at_index = dpl_get_reference_at_index(list, index-1);
		assert(ref_at_index != NULL);
		if (index < dpl_size(list)) { // covers case 4, within the list
			list_node->next = ref_at_index->next;
			ref_at_index->next->prev = list_node;
			list_node->prev = ref_at_index;
			ref_at_index->next = list_node;
		} else { // covers case 3
			list_node->prev = ref_at_index;
			ref_at_index->next = list_node;
			list_node->next = NULL;
		}
	}
    return list;
}

dplist_t* dpl_remove_at_index(dplist_t* list, int index, bool free_element) {
	// If 'list' is is NULL, NULL is returned.
	if(list == NULL){
		return NULL; 
	}
	// check if index is < 0 or > size of list. if index is < 0, set index to 0. if index is > size of list, set index to size of list
	if(index < 0){
		index = 0;
	}else if(index > dpl_size(list)){
		index = dpl_size(list);
	}
	dplist_node_t* node = dpl_get_reference_at_index(list, index);
	if(node == NULL){
		return NULL;
	}
	// remove node from list by changing the pointers of the previous and next nodes
	if(node->prev == NULL && node->next == NULL){ //only one node
		list->head = NULL;
	}else if(node->prev == NULL){ //first node
		list->head = node->next;
		node->next->prev = NULL;
	}else if(node->next == NULL){ //last node
		node->prev->next = NULL;
	}else{ //middle node
		node->prev->next = node->next;
		node->next->prev = node->prev;
	}
	if(free_element){
		list->element_free(&node->element);
	}
	// free node
	printf("addr of freed heap list_node: %p \n", node);
	free(node);
	node = NULL;

    return list;
}

int dpl_size(dplist_t* list) {
	dplist_node_t* list_node; // ptr to node
	if(list == NULL) return -1; // If 'list' is is NULL, -1 is returned.
	if(list->head == NULL) return 0;
	list_node = list->head;
	int counter = 1;
	while(list_node->next != NULL) // node.next is not null
	{
			list_node = list_node->next;
			counter++;
	}
	return counter;
}

void* dpl_get_element_at_index(dplist_t* list, int index) {
	dplist_node_t* ref_at_index = dpl_get_reference_at_index(list,index);
	if(ref_at_index != NULL){
			return ref_at_index->element;
	}
	return NULL;
}

int dpl_get_index_of_element(dplist_t* list, void* element) {
	// If 'list' or list->head is NULL, -1 is returned.
	if(list == NULL || list->head == NULL) return -1;
	dplist_node_t* list_node = list->head;
	for(int count = 0; list_node->next != NULL; count++){
		if(list->element_compare(list_node->element, element) == 0){
			return count;
		}
		list_node = list_node->next;
	}
	return -1;
}

dplist_node_t* dpl_get_reference_at_index(dplist_t* list, int index) {
    int count;
    dplist_node_t* dummy;
    if(list == NULL || list->head == NULL) return NULL;
	// if index is < 0, set index to 0. if index is > size of list, set index to last node
	if(index < 0){
		index = 0;
	}else if(index > dpl_size(list)){
		index = dpl_size(list);
	}
	dummy = list->head;
	// iterate through the list until the counter is equal to the index
	for(count = 0; count < index; count++){
		dummy = dummy->next;
	}
	return dummy;
}

void* dpl_get_element_at_reference(dplist_t* list, dplist_node_t* reference) {
	dplist_node_t* dummy;
	if(list == NULL || list->head == NULL || reference == NULL) return NULL;
	for(int i = 0; i < dpl_size(list); i++){
		dummy = list->head;
		if (dummy == reference){return dummy->element;}
		dummy = dummy->next;
	}
	return NULL;
}

