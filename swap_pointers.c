#include <stdio.h>
#include <stdlib.h>

/* function declaration */
void swap_pointers ( void ** first_pointer, void ** second_pointer );

/* main */
int main(){
	int a = 1, b = 2;
	void* p = &a;
	void* q = &b; //p points to a, q points to b
	printf ("before swap_pointers(), address of: p  = %p, q = %p \n", p,q); // prints p = &a and q = &b
	swap_pointers ( ( void ** ) &p, ( void ** ) &q ); // two pointer of pointers 
	printf ("after swap_pointers(), address of: p = %p, q = %p \n", p,q);//prints p = &b and q = &a
	return 0;
}

void swap_pointers ( void ** first_ptr, void ** second_ptr ) {
	//fst ptr is now pointing to p. snd ptr to q
	void* temp = *first_ptr; // temp pointing to the content of p, meaning &a. meaning temp is now pointing to a
	*first_ptr = *second_ptr; // *first ptr, so *&p, is now equal to *second ptr, so*&q. namely, *&p is now &b. p is now pointing to b 
    	*second_ptr = temp; // *second ptr, so *&q, is now equal to temp which is &a
}
