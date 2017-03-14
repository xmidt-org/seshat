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
#define TEST1_INC_SP    false
#define TEST1_STATUS    200
#define TEST1_RDR       0
#define TEST1_PATH      "path1"
#define TEST1_PAYLOAD   "payload1"

/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
ssize_t wrp_to_struct( const void *bytes, const size_t length,
                       const enum wrp_format fmt,
                       wrp_msg_t **msg )
{
    wrp_msg_t *m;

    *msg = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
    m = *msg;

    m->msg_type = WRP_MSG_TYPE__RETREIVE;
    m->u.crud.transaction_uuid = TEST1_TRAN;
    m->u.crud.source = TEST1_SOURCE;
    m->u.crud.dest = TEST1_DEST;
    m->u.crud.path = TEST1_PATH;

    (void) bytes; (void) length; (void) fmt;

    return 1;
}

int ji_add_entry( const char *entry, const char *value )
{
    (void) entry; (void) value;
    return EXIT_SUCCESS;
}

int ji_retrieve_entry( const char *entry, char **object )
{
    (void) entry; 
    *object = TEST1_PAYLOAD;
    return EXIT_SUCCESS;
}

void wrp_free_struct( wrp_msg_t *msg )
{
    free(msg);
}

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/
void test_wi_create_response_to_message_ret()
{
    wrp_msg_t msg;

    wi_create_response_to_message(NULL, 0, &msg);
    CU_ASSERT(WRP_MSG_TYPE__RETREIVE == msg.msg_type);
    CU_ASSERT(0 == strcmp(msg.u.crud.transaction_uuid, TEST1_TRAN));
    CU_ASSERT(0 == strcmp(msg.u.crud.source, TEST1_DEST));
    CU_ASSERT(0 == strcmp(msg.u.crud.dest, TEST1_SOURCE));
    CU_ASSERT(NULL == msg.u.crud.headers);
    CU_ASSERT(NULL == msg.u.crud.metadata);
    CU_ASSERT(TEST1_INC_SP == msg.u.crud.include_spans);
    CU_ASSERT(TEST1_STATUS == msg.u.crud.status);
    CU_ASSERT(TEST1_RDR == msg.u.crud.rdr);
    CU_ASSERT(0 == strcmp(msg.u.crud.path, TEST1_PATH));
    CU_ASSERT(0 == strcmp(msg.u.crud.payload, TEST1_PAYLOAD));
}

void add_suites( CU_pSuite *suite )
{
    printf("--------Start of Test Cases Execution ---------\n");
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Test 1", test_wi_create_response_to_message_ret );
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
