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
#define TEST2_FILE_NAME "test2.json"

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
extern ll_t *head;

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/
void test_ji_add_and_retrieve_entry()
{
    typedef struct {
        char *e;
        char *v;
        char *expected;
    } test_t;

    test_t tests[] = {
        { .e = "service1", .v = "url1",                    .expected = "{\"service1\":{\"url\":\"url1\"}}", },
        { .e = "service2", .v = "tcp://127.0.0.2:1234",    .expected = "{\"service2\":{\"url\":\"tcp://127.0.0.2:1234\"}}", },
        { .e = "foobar",   .v = "tcp://172.10.99.23:1234", .expected = "{\"foobar\":{\"url\":\"tcp://172.10.99.23:1234\"}}" },
    };
    char *buf;
    size_t i = 0;

    for( i = 0; i < sizeof(tests)/sizeof(test_t); i++ ) {
        ji_add_entry( tests[i].e, tests[i].v );
    }

    for( i = 0; i < sizeof(tests)/sizeof(test_t); i++ ) {
        ji_retrieve_entry( tests[i].e, &buf );
        printf("buf = %s\n", buf);
        CU_ASSERT(0 == strcmp( tests[i].expected, buf) );
        free(buf);
    }  
}

void test_ji_init()
{
    FILE *fp = fopen(TEST2_FILE_NAME, "w");
    char *entry[] = { "service1", "service2",             "foobar" };
    char *value[] = { "url1",     "tcp://127.0.0.2:1234", "tcp://172.10.99.23:1234" }; 
    ll_t *current;
    int i;

    for(i = 0;i < 3; i++) {
        fprintf(fp, "%s,%s\n", entry[i], value[i]);
    }
    fclose(fp);

    ji_init(TEST2_FILE_NAME);
    current = head;
    i = 0;
    while( NULL != current ) {
        CU_ASSERT(0 == strcmp(entry[i], current->entry));
        CU_ASSERT(0 == strcmp(value[i], current->value));
        current = current->next;
        i++;
    }
}

void test_ji_destroy()
{
    CU_ASSERT(NULL != head);
    ji_destroy();
    CU_ASSERT(NULL == head);
}

void add_suites( CU_pSuite *suite )
{
    printf("--------Start of Test Cases Execution ---------\n");
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Test 1", test_ji_add_and_retrieve_entry );
    CU_add_test( *suite, "Test 2", test_ji_init);
    CU_add_test( *suite, "Test 3", test_ji_destroy);
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
