/**
 * \author Jeffee Hsiung
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include "dplist.h"

//#define DEBUG

/*
 * definition of error codes
 */
#define DPLIST_NO_ERROR 0
#define DPLIST_MEMORY_ERROR 1   //error due to mem alloc failure
#define DPLIST_INVALID_ERROR 2  //error due to a list operation applied on a NULL list

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
    dplist_node_t *prev, *next; //pointer to node
    element_t element;
};

struct dplist {
    dplist_node_t* head;// head points to node 
    // more fields will be added later
};

dplist_t* dpl_create() {
	dplist_t* list; //list points to dplist
	list = malloc(sizeof(struct dplist)); // dplist block on heap
    	DPLIST_ERR_HANDLER(list == NULL, DPLIST_MEMORY_ERROR); 
    	list->head = NULL; //initialize the data to null
    	return list; 
}

void dpl_free(dplist_t** list) { // pointer of pointers to  dplist
	if (list != NULL){ 
		if((*list)->head != NULL){// dplist.head != null
			(*list)->head = NULL; // set dplist.head to null
		};
		free(*list); //free the dplist block on heap
		list = NULL; 
	}
}

/* Important note: to implement any list manipulation operator (insert, append, delete, sort, ...), always be aware of the following cases:
 * 1. empty list ==> avoid errors
 * 2. do operation at the start of the list ==> typically requires some special pointer manipulation
 * 3. do operation at the end of the list ==> typically requires some special pointer manipulation
 * 4. do operation in the middle of the list ==> default case with default pointer manipulation
 * ALWAYS check that you implementation works correctly in all these cases (check this on paper with list representation drawings!)
 **/

dplist_t* dpl_insert_at_index(dplist_t* list, element_t element, int index) {
    dplist_node_t *ref_at_index, *list_node;
    if (list == NULL) return NULL;

    list_node = malloc(sizeof(dplist_node_t));
    DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
    list_node->element = element; // store the new element onto heap pointed by list_node
    // pointer drawing breakpoint
    if (list->head == NULL) { // covers case 1
        list_node->prev = NULL;
        list_node->next = NULL;
        list->head = list_node;
        // pointer drawing breakpoint
    } else if (index <= 0) { // covers case 2, insert into beginning
        list_node->prev = NULL;
        list_node->next = list->head;
        list->head->prev = list_node;
        list->head = list_node;
        // pointer drawing breakpoint
    } else {
        ref_at_index = dpl_get_reference_at_index(list, index);
        assert(ref_at_index != NULL); // ref_at_index shall not be null
        // pointer drawing breakpoint
        if (index < dpl_size(list)) { // covers case 4, within the list
            list_node->prev = ref_at_index->prev; //insert new node at index
            list_node->next = ref_at_index;
            ref_at_index->prev->next = list_node;
            ref_at_index->prev = list_node;
            // pointer drawing breakpoint
        } else { // covers case 3
            assert(ref_at_index->next == NULL);
            list_node->next = NULL;
            list_node->prev = ref_at_index;
            ref_at_index->next = list_node;
            // pointer drawing breakpoint
        }
    }
    return list;
}

dplist_t* dpl_remove_at_index(dplist_t* list, int index) {
    	dplist_node_t* list_node;
	DPLIST_ERR_HANDLER(list == NULL,DPLIST_INVALID_ERROR);
	DPLIST_ERR_HANDLER(index >= dpl_size(list), DPLIST_INVALID_ERROR);
	if(list->head != NULL){
		list_node = dpt_get_reference_at_index(list,index);  //target node
		assert(list_node != NULL);
		list_node->prev->next = list_node->next;
		list_node->next->prev = list_node->prev;
		if(index <= 0){
			assert(list_node->prev == NULL);
			list_node->next->prev = NULL;
			list->head = list_node->next;
		}
		list_node->element = NULL;//set data to NULL
		free(list_node);
	} 
	list_node = NULL;
	return  list;

}

int dpl_size(dplist_t*list) {
	dplist_node_t* list_node; //ptr to node
	if(list->head == NULL) return 0;
	list_node = list->head; 
	int counter = 1; 
	while(list_node->next != NULL) //node.next is not null
	{
	list_node = list_node->next;
	counter++;
	}
	return counter;
}

dplist_node_t* dpl_get_reference_at_index(dplist_t* list, int index) {
    	int count;
    	dplist_node_t* dummy;
    	DPLIST_ERR_HANDLER(list == NULL, DPLIST_INVALID_ERROR);
    	if (list->head == NULL) return NULL;
	if(index <= 0){dummy = list->head;}
    	for (dummy = list->head, count = 0; dummy->next != NULL; dummy = dummy->next, count++) {
        	if (count >= index) return dummy;
    	}
    	return dummy; //dummy = head pointing to node at the index
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {


}

int dpl_get_index_of_element(dplist_t *list, element_t element) {

    //TODO: add your code here

}
