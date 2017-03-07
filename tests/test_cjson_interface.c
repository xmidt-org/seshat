/**
 *  Copyright 2017 Comcast Cable Communications Management, LLC
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <assert.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <cJSON.h>

#include <CUnit/Basic.h>

#include "../src/cjson_interface.h"

/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
#define TEST1_FILE_NAME "test1.json"
#define TEST1_ENTRY     "service1"
#define TEST1_VALUE     "url1"

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/
size_t file_to_string( FILE *handle, char **buffer )
{
    size_t length;
    if( NULL == handle )
    {
        return 0;
    }

    fseek(handle, 0, SEEK_END);
    length = ftell(handle);
    fseek(handle, 0, SEEK_SET);
    *buffer = (char *) malloc(length);
    if( NULL == buffer )
    {
        return 0;
    }
    return fread(*buffer, 1, length, handle);
}

void test_cji_add_entry()
{
    FILE *file_handle;
    char *buf;

    file_handle = fopen(TEST1_FILE_NAME, "w+");
    file_to_string(file_handle, &buf);

    cji_add_entry(TEST1_ENTRY, TEST1_VALUE, &buf);
    printf("buf = %s\n", buf);

    fwrite(buf, sizeof(char), strlen(buf), file_handle);
    free(buf);
    fclose(file_handle);
}

void add_suites( CU_pSuite *suite )
{
    printf("--------Start of Test Cases Execution ---------\n");
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Test 1", test_cji_add_entry );
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( void )
{
    unsigned rv = 1;
    CU_pSuite suite = NULL;

    if( CUE_SUCCESS == CU_initialize_registry() ) {
        add_suites( &suite );

        if( NULL != suite ) {
            CU_basic_set_mode( CU_BRM_VERBOSE );
            CU_basic_run_tests();
            printf( "\n" );
            CU_basic_show_failures( CU_get_failure_list() );
            printf( "\n\n" );
            rv = CU_get_number_of_tests_failed();
        }

        CU_cleanup_registry();

    }

    return rv;
}
