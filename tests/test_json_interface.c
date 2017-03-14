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
#define TEST_ENTRY1     "service1"
#define TEST_VALUE1     "url1"

#define TEST_ENTRY2     "service2"
#define TEST_VALUE2     "tcp://127.0.0.2:1234"

#define TEST_ENTRY3     "foobar"
#define TEST_VALUE3     "tcp://172.10.99.23:1234"

#define TEST2_FILE_NAME "test2.json"

/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
extern ll_t *head;

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/
void test_ji_add_and_retrieve_entry()
{
    char *buf; char *ver_buf;
    cJSON *ver_buf_JSON;
    cJSON *service;

    printf("test_ji_add_and_retrieve_entry\n");

    ji_add_entry(TEST_ENTRY1, TEST_VALUE1);
    ji_add_entry(TEST_ENTRY2, TEST_VALUE2);
    ji_add_entry(TEST_ENTRY3, TEST_VALUE3);
 
    ji_retrieve_entry(TEST_ENTRY1, &buf);
    printf("buf1 = %s\n", buf);
    ver_buf_JSON = cJSON_CreateObject();
    service = cJSON_CreateObject();
    cJSON_AddItemToObject(ver_buf_JSON, TEST_ENTRY1, service);
    cJSON_AddStringToObject(service, "url", TEST_VALUE1);
    ver_buf = cJSON_Print(ver_buf_JSON);
    cJSON_Delete(ver_buf_JSON);
    CU_ASSERT(0 == strcmp(ver_buf, buf));
    free(buf); free(ver_buf);

    ji_retrieve_entry(TEST_ENTRY2, &buf);
    printf("buf2 = %s\n", buf);
    ver_buf_JSON = cJSON_CreateObject();
    service = cJSON_CreateObject();
    cJSON_AddItemToObject(ver_buf_JSON, TEST_ENTRY2, service);
    cJSON_AddStringToObject(service, "url", TEST_VALUE2);
    ver_buf = cJSON_Print(ver_buf_JSON);
    cJSON_Delete(ver_buf_JSON);
    CU_ASSERT(0 == strcmp(ver_buf, buf));
    free(buf); free(ver_buf);

    ji_retrieve_entry(TEST_ENTRY3, &buf);
    printf("buf3 = %s\n", buf);
    ver_buf_JSON = cJSON_CreateObject();
    service = cJSON_CreateObject();
    cJSON_AddItemToObject(ver_buf_JSON, TEST_ENTRY3, service);
    cJSON_AddStringToObject(service, "url", TEST_VALUE3);
    ver_buf = cJSON_Print(ver_buf_JSON);
    cJSON_Delete(ver_buf_JSON);
    CU_ASSERT(0 == strcmp(ver_buf, buf));
    free(buf); free(ver_buf);
}

void test_ji_init()
{
/*
    FILE *fp = fopen(TEST2_FILE_NAME, "w");
    ll_t *current; int i;
    char *entry[] = {TEST_ENTRY1, TEST_ENTRY2, TEST_ENTRY3};
    char *value[] = {TEST_VALUE1, TEST_VALUE2, TEST_VALUE3}; 

    printf("test_ji_init\n");

    fprintf(fp, "%s,%s\n", TEST_ENTRY1, TEST_VALUE1);
    fprintf(fp, "%s,%s\n", TEST_ENTRY2, TEST_VALUE2);
    fprintf(fp, "%s,%s\n", TEST_ENTRY3, TEST_VALUE3);
    fclose(fp);

    ji_init(TEST2_FILE_NAME);
    current = head;
    while( NULL != current ) {
        CU_ASSERT(0 == strncmp(entry[i], current->entry, strlen(entry[i])));
        CU_ASSERT(0 == strncmp(value[i], current->value, strlen(value[i])));
        current = current->next; i++;
    }
 */
}

void test_ji_destroy()
{
/*
    printf("test_ji_destory\n");
    CU_ASSERT(NULL != head);
    ji_destroy();
    CU_ASSERT(NULL == head);
 */
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
