/**
 * @file test_fs.c
 * @brief Unit-test suite for the file system lab
 *
 * @author Matteo Rizzo
 */
#include <check.h>
#include <stdio.h>
#include <stdlib.h>

#include "blk_io.h"
#include "disk.h"
#include "fs_api.h"
#include "fs_util.h"
#include "open_file_table.h"

char *fss_path = NULL;
char *my_fs = NULL;

char* make_string_to_open(const char* fs)
{
    size_t path_len = strlen(fs) + strlen(fss_path) + 1;
    char *fs1 = malloc(path_len);
    strcpy(fs1,fss_path);
    return strcat(fs1,fs);
}

START_TEST(test_for_empty_fs)
{
    ck_assert_int_eq(open_emu_disk(my_fs), 0);

    struct lab3_inode *root_inode = find_inode_by_path("/");

    /* Check that the root inode looks fine */
    ck_assert_ptr_ne(root_inode, NULL);
    ck_assert_uint_eq(root_inode->id, 0);
    ck_assert_str_eq(root_inode->name, "root");
    ck_assert(root_inode->is_directory);
    ck_assert_uint_eq(root_inode->directory.num_children, 0);

    for (int i = 0; i < MAX_DATA_BLOCKS_PER_INODE; i++) {
        ck_assert_uint_eq(root_inode->directory.children_offsets[i], 0);
    }

    ck_assert_ptr_eq(find_inode_by_path("/foo/bar"), NULL);
    free(root_inode);
}
END_TEST


void cleanup(void)
{
    if (the_disk != NULL) {
        close_emu_disk();
    }

    /* Reset the open file table */
    for (int i = 0; i < MAX_OPEN_FILES; i++) {
        if (open_file_table[i].inode != NULL) {
            free(open_file_table[i].inode);
            open_file_table[i].inode = NULL;
        }
    }
}

int main(int argc, char **argv)
{
    if(argc != 2) {
        printf("Usage: ./test_fs <absolute path to test filesystems>\n");
        exit(1);
    }
    fss_path = strdup(argv[1]);
    size_t len = strlen(fss_path);
    if (fss_path[len-1] != '/') {
        fss_path = realloc(fss_path,len+2);
        assert(fss_path != NULL);
        strcat(fss_path,"/");
    } else {
        assert(fss_path != NULL);
    }

    /* Before run tests
     * use the generate_test_fs.py script to generate a my_fs_img.fs image
     * put this image into the a directory (parent directory or mount point)
     * run test_fs, giving it the path to the parent directory as a first argument
     */

    const char *my_fs_suffix = "my_fs_img.fs";
    my_fs = make_string_to_open(my_fs_suffix);

    printf("Running fsys tests using test filesystems in dir: %s\n",fss_path);

    Suite *s = suite_create("File System Tests");
    TCase *tc1 = tcase_create("testcase");
    tcase_add_checked_fixture(tc1, NULL, cleanup);
    suite_add_tcase(s, tc1);

    tcase_add_test(tc1, test_for_empty_fs);



    SRunner *sr = srunner_create(s);
    srunner_run_all(sr, CK_VERBOSE);

    int number_failed = srunner_ntests_failed(sr);
    srunner_free(sr);
    free(fss_path);
    free(my_fs);
    return (number_failed == 0) ? EXIT_SUCCESS : EXIT_FAILURE;
}
