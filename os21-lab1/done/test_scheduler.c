/**
 * @brief Testing the scheduling policies
 *
 * @author Adrien Ghosn, Mark Sutherland
 */

#include <check.h>
#include <stdlib.h>

int main(int argc, char **argv) {
    Suite* s = suite_create("Threading lab");
    TCase *tc1 = tcase_create("basic"); 
    /* Your code here to test scheduling policies */

    SRunner *sr = srunner_create(s); 
    srunner_run_all(sr, CK_VERBOSE); 

    int number_failed = srunner_ntests_failed(sr); 
    srunner_free(sr); 
    return (number_failed ==0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
