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
	if(dbptr != NULL){
		if(*dbptr != NULL){ // *dptr = *&list = &dplist(on heap) != null
			if((*dbptr)->head != NULL){// head = &node != null
                                for(int i = 0; i < dpl_size(*dbptr); i++){
                                        (*dbptr) = dpl_remove_at_index(*dbptr,i,free_element);
                                } //set every node to null
				(*dbptr)->head = NULL; //  break arrow between head to node
                        }
		}
		free(*dbptr);
		printf("list = dplist* is free as a bird now. \n");
		*dbptr = NULL;
		dbptr = NULL; 
        }
}

dplist_t* dpl_insert_at_index(dplist_t* list, void* element, int index, bool insert_copy) {
	dplist_node_t* ref_at_index;
	dplist_node_t* list_node;
    	if(list == NULL) return NULL;
    	list_node = malloc(sizeof(dplist_node_t)); //list_node needs to be freed
    	DPLIST_ERR_HANDLER(list_node == NULL, DPLIST_MEMORY_ERROR);
    	if(insert_copy){
		list_node->element = list->element_copy(element);
	}else{
		list_node->element = element; 
	} // on heap: list_node, my_element_t* copy = list_node->element
    	// pointer drawing breakpoint
    	if (list->head == NULL) { // covers case 1, empty, free bird
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
	// free(list_node->element);
	// free(list_node);
	// list_node = NULL;
    return list;
}

dplist_t* dpl_remove_at_index(dplist_t* list, int index, bool free_element) {
	dplist_node_t* list_node;
        if(list == NULL) return NULL; // If 'list' is is NULL, NULL is returned.
        if(list->head != NULL){
                list_node = dpl_get_reference_at_index(list,index);  // target node
                if(list_node != NULL){
                	if(index <= 0){
                        	assert(list_node->prev == NULL);
                        	if(list_node->next != NULL){
                                	list_node->next->prev = NULL;
                                	list->head = list_node->next;
                        	}
                	}
                	else if (index >= dpl_size(list)){
                        	assert(list_node->next == NULL);
                        	if(list_node->prev != NULL){
					list_node->prev->next = NULL;
				}
                	}
                	else{
                        	list_node->prev->next = list_node->next;
                        	list_node->next->prev = list_node->prev;
                	}
                	if(free_element){ 
				if(list_node->element == NULL){ free(list_node->element); }else{
				list->element_free(&(list_node->element));} //list_node->element = void*, &(void*) = void**
			}
		}
                //free(list_node);
		list_node = NULL;
        } 
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
        if(list != NULL && list->head != NULL){
                int count = 0;
                dplist_node_t* list_node;
		list_node = list->head;
                for(count = 0; list_node->next != NULL; count++){
                        if(list->element_compare((list_node->element), element) == 0){
				return count; }else{
				list_node = list_node->next;
			} 
                }
        }
        return -1;
}

dplist_node_t* dpl_get_reference_at_index(dplist_t* list, int index) {
        int count;
        dplist_node_t* dummy;
        if(list == NULL || list->head == NULL) return NULL;
        dummy = list->head; //dummy = head = node
        for (count = 0; dummy->next != NULL; count++) {
                if (count >= index || dummy->next == NULL) {
                        return dummy;
                }; 
                dummy = dummy->next; // dummy = next node
        }
        return dummy; //dummy = head pointing to node at the index
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

