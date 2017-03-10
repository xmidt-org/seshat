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
#define TEST1_SERVICE    "service1"
#define TEST1_URL        "url1"
#define TEST1_STATUS     200

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static wrp_msg_t m;

/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
ssize_t wrp_to_struct( const void *bytes, const size_t length,
                       const enum wrp_format fmt,
                       wrp_msg_t **msg )
{
    *msg = &m;

    m.msg_type = WRP_MSG_TYPE__SVC_REGISTRATION;
    m.u.reg.service_name = TEST1_SERVICE;
    m.u.reg.url = TEST1_URL;

    (void) bytes; (void) length; (void) fmt;

    return 1;
}

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

void wrp_free_struct( wrp_msg_t *msg )
{
    (void) msg;
}

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/
void test_wi_create_response_to_message_reg()
{
    wrp_msg_t msg;

    wi_create_response_to_message(NULL, 0, &msg);
    CU_ASSERT(WRP_MSG_TYPE__AUTH == msg.msg_type);
    CU_ASSERT(TEST1_STATUS == msg.u.auth.status);
}

void add_suites( CU_pSuite *suite )
{
    printf("--------Start of Test Cases Execution ---------\n");
    *suite = CU_add_suite( "tests", NULL, NULL );
    CU_add_test( *suite, "Test 1", test_wi_create_response_to_message_reg );
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
