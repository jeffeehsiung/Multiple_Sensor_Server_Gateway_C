/*
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
#define DEBUG_PRINTF(...)                                                                                                               \
        do {                                                                                                                                \
            fprintf(stderr,"\nIn %s - function %s at line %d: ", __FILE__, __func__, __LINE__);     \
            fprintf(stderr,__VA_ARGS__);                                                                                            \
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
    element_t element; //element is a element_t = char*
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

void dpl_free(dplist_t** dbptr) { // pointer of pointers to  dplist)
        if (dbptr != NULL){ 
                if(*dbptr != NULL){ // *&dplist* != null
                        if((*dbptr)->head != NULL){// head = &node != null
				for(int i = 0; i < dpl_size(*dbptr); i++){
					(*dbptr) = dpl_remove_at_index(*dbptr,i);
				} //set every node to null
                                (*dbptr)->head = NULL; //  set node = null
                        }
                }
		*dbptr = NULL; // set dplist* = null
                free(*dbptr); //free the dplist* on heap
                dbptr = NULL; 
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

    list_node = malloc(sizeof(dplist_node_t)); //list_node needs to be freed
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
        free(list_node);
    }
    return list;
}

dplist_t* dpl_remove_at_index(dplist_t* list, int index) {
        dplist_node_t* list_node;
        if(list == NULL) return NULL; // If 'list' is is NULL, NULL is returned.
        if(list->head != NULL){
                list_node = dpl_get_reference_at_index(list,index);  // target node
                assert(list_node != NULL);
                if(index <= 0){
                        assert(list_node->prev == NULL);
                        if(list_node->next != NULL){
                                list_node->next->prev = NULL;
                                list->head = list_node->next;
				}
                }
                else if (index >= dpl_size(list)){
                        assert(list_node->next == NULL);
                        if(list_node->prev != NULL){list_node->prev->next = NULL;}
                }
                else{
                        list_node->prev->next = list_node->next;
                        list_node->next->prev = list_node->prev;
                }
		list_node->element = NULL;
                list_node = NULL;
                //free(list_node), list_node is not on heap;
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

dplist_node_t* dpl_get_reference_at_index(dplist_t* list, int index){
        int count;
        dplist_node_t* dummy;
        if (list == NULL || list->head == NULL) return NULL;
	dummy = list->head; //dummy = head = node
        for (count = 0; dummy->next != NULL; count++) {
		if (count >= index || dummy->next == NULL) {
			return dummy;
		}; 
		dummy = dummy->next; // dummy = next node
        }
        return dummy; //dummy = head pointing to node at the index
}

element_t dpl_get_element_at_index(dplist_t *list, int index) {
        dplist_node_t* ref_at_index = dpl_get_reference_at_index(list,index);
        if(ref_at_index != NULL){
                return ref_at_index->element;
        }
        return NULL;

}

int dpl_get_index_of_element(dplist_t *list, element_t element) {
        if(list != NULL && list->head != NULL){
                int count = 0;
                dplist_node_t* list_node;
                for(list_node = list->head, count = 0; list_node->next != NULL; list_node = list_node->next, count++){
                        if(list_node->element == element) return count; 
                }
        }
        return -1;
}

