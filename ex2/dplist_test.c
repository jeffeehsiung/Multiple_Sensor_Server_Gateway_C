/*
 * \author Jeffee Hsiung
 */

#include "dplist.h"
#include <check.h>
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

void setup(void) {
    // Implement pre-test setup
}

void teardown(void) {
    // Implement post-test teardown
}
START_TEST(test_ListFree)
    {
        // Test free NULL
        dplist_t* list = NULL;
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected free NULL result to be NULL");

        // Test free empty list
        list = dpl_create();
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected free emptyList result to be NULL");
        // Test free with one element
        list = dpl_create();
	char a = 'A';
	char b = 'B';
        element_t element = &a;
        list = dpl_insert_at_index(list,element, 0);
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: expected free one element result to be NULL");
        // Test free with multiple element
        element_t elements[]  = {&a,&b};
	printf("address of char a: %p and b: %p \n",&a, &b);
	printf("size of elements: %ld: and size of element: %ld: \n", sizeof(elements), sizeof(element));
        int length = sizeof(elements)/sizeof(element);
        list = dpl_create();
        for(int i = 0; i < length; i++){
                list = dpl_insert_at_index(list,elements[i], i);
        }
        dpl_free(&list);
        ck_assert_msg(list == NULL, "Failure: exptected free with multiple element to be NULL");
    }
END_TEST

START_TEST(test_ListInsertAtIndexListNULL)
    {
	char a = 'A';
        // Test inserting at index -1
        dplist_t *result = dpl_insert_at_index(NULL, &a, -1);
        ck_assert_msg(result == NULL, "Failure: expected list to be NULL");
        // Test inserting at index 0
        result = dpl_create();
        result = dpl_insert_at_index(result,&a,0);
	element_t temp = &a;
        ck_assert_msg(temp == dpl_get_element_at_index(result,0), "Failure: expect element to be &a on index 0");
        dpl_remove_at_index(result,0);
        dpl_free(&result);
        // Test inserting at index 99
        result = dpl_create();
        result = dpl_insert_at_index(result,&a,99);
        ck_assert_msg(temp  == dpl_get_element_at_index(result,99), "Failure: expect element to be &a on index 9");
        dpl_remove_at_index(result,99);
        dpl_free(&result);
    }
END_TEST

START_TEST(test_ListInsertAtIndexListEmpty)
{
	char a = 'A';
    	// Test inserting at index -1
    	dplist_t* list = dpl_create();
    	dplist_t* result = dpl_insert_at_index(list, &a, -1);
    	ck_assert_msg(dpl_size(result) == 1, "Failure: expected list to have size of 1, got a size of %d", dpl_size(result));
    	dpl_free(&list);
    	// Test inserting at index 0, already tested in previous insert at index 0;

    	// Test inserting at index 99, already tested in previous insert at index 99;
}
END_TEST

//START_TEST(test_nameOfYourTest)
//  Add other testcases here...
//END_TEST


int main(void) {
    Suite *s1 = suite_create("LIST_EX1");
    TCase *tc1_1 = tcase_create("Core");
    SRunner *sr = srunner_create(s1);
    int nf;

    suite_add_tcase(s1, tc1_1);
    tcase_add_checked_fixture(tc1_1, setup, teardown);
    tcase_add_test(tc1_1, test_ListFree);
    tcase_add_test(tc1_1, test_ListInsertAtIndexListNULL);
    tcase_add_test(tc1_1, test_ListInsertAtIndexListEmpty);
    // Add other tests here...

    srunner_run_all(sr, CK_VERBOSE);

    nf = srunner_ntests_failed(sr);
    srunner_free(sr);

    return nf == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}

