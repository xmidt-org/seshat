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
#include <stdbool.h>
#include <cJSON.h>

#include <CUnit/Basic.h>

#include "../src/json_interface.h"

/*----------------------------------------------------------------------------*/
/*                                  Macros                                    */
/*----------------------------------------------------------------------------*/
#define TEST_FILE_NAME  "test.json"

#define TEST1_ENTRY     "service1"
#define TEST1_VALUE     "url1"

/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
FILE *g_file_handle;

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/
void test_ji_add_and_retrieve_entry()
{
    char *buf; char *ver_buf;
    cJSON *ver_buf_JSON = cJSON_CreateObject();
    cJSON *service = cJSON_CreateObject();

    cJSON_AddItemToObject(ver_buf_JSON, TEST1_ENTRY, service);
    cJSON_AddStringToObject(service, "url", TEST1_VALUE);

    ver_buf = cJSON_Print(ver_buf_JSON);
    cJSON_Delete(ver_buf_JSON);
 
    g_file_handle = fopen(TEST_FILE_NAME, "w");
    ji_add_entry(TEST1_ENTRY, TEST1_VALUE);
 
    ji_retrieve_entry(TEST1_ENTRY, &buf);
    printf("buf = %s\n", buf);
    CU_ASSERT(0 == strcmp(ver_buf, buf));

    free(buf); free(ver_buf);
    fclose(g_file_handle);
}

void add_suites( CU_pSuite *suite )
{
    printf("--------Start of Test Cases Execution ---------\n");
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Test 1", test_ji_add_and_retrieve_entry );
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
