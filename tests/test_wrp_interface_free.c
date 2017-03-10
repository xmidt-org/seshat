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
#include <wrp-c.h>

#include <CUnit/Basic.h>

#include "../src/json_interface.h"
#include "../src/wrp_interface.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define TEST1_TRAN      "tran1"
#define TEST1_SOURCE    "source1"
#define TEST1_DEST      "dest1"
#define TEST1_PATH      "path1"
#define TEST1_PAYLOAD   "payload1"

/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
jir_t ji_add_entry( const char *entry, const char *value )
{
    (void) entry; (void) value;
    return JIRT__SUCCESS;
}

jir_t ji_retrieve_entry( const char *entry, char **object )
{
    (void) entry; (void) object;
    return JIRT__SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/
void test_wi_free()
{
    wrp_msg_t msg;

    memset(&msg, 0, sizeof(wrp_msg_t));
    msg.msg_type = WRP_MSG_TYPE__RETREIVE;
    msg.u.crud.transaction_uuid = (char *) malloc(1);
    msg.u.crud.source = (char *) malloc(1);
    msg.u.crud.dest = (char *) malloc(1);
    msg.u.crud.path = (char *) malloc(1);
    msg.u.crud.payload = (char *) malloc(1);
    wi_free(&msg);
    CU_ASSERT(NULL == msg.u.crud.transaction_uuid);
    CU_ASSERT(NULL == msg.u.crud.source);
    CU_ASSERT(NULL == msg.u.crud.dest);
    CU_ASSERT(NULL == msg.u.crud.path);
    CU_ASSERT(NULL == msg.u.crud.payload);

    memset(&msg, 0, sizeof(wrp_msg_t));
    msg.msg_type = WRP_MSG_TYPE__CREATE;
    msg.u.crud.transaction_uuid = TEST1_TRAN;
    msg.u.crud.source = TEST1_SOURCE;
    msg.u.crud.dest = TEST1_DEST;
    msg.u.crud.path = TEST1_PATH;
    msg.u.crud.payload = TEST1_PAYLOAD;
    wi_free(&msg);
    CU_ASSERT(0 == strcmp(msg.u.crud.transaction_uuid, TEST1_TRAN));
    CU_ASSERT(0 == strcmp(msg.u.crud.source, TEST1_SOURCE));
    CU_ASSERT(0 == strcmp(msg.u.crud.dest, TEST1_DEST));
    CU_ASSERT(0 == strcmp(msg.u.crud.path, TEST1_PATH));
    CU_ASSERT(0 == strcmp(msg.u.crud.payload, TEST1_PAYLOAD));
}

void add_suites( CU_pSuite *suite )
{
    printf("--------Start of Test Cases Execution ---------\n");
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Test 1", test_wi_free );
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
