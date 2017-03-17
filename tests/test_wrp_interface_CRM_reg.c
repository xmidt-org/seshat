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
/* none */

/*----------------------------------------------------------------------------*/
/*                                   Mocks                                    */
/*----------------------------------------------------------------------------*/
int ji_add_entry( const char *entry, const char *value )
{
    (void) entry; (void) value;
    return EXIT_SUCCESS;
}

int ji_retrieve_entry( const char *entry, char **object )
{
    (void) entry; (void) object;
    return EXIT_SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*                                   Tests                                    */
/*----------------------------------------------------------------------------*/
void test_wi_create_response_to_message_reg()
{
    typedef struct {
        wrp_msg_t s;
        wrp_msg_t r;
    } test_t;

    test_t tests[] = {
        {
            .s.msg_type = WRP_MSG_TYPE__AUTH,
            .s.u.auth.status = 0,

            .r.msg_type = WRP_MSG_TYPE__AUTH,
            .r.u.auth.status = 400,
        },
        {
            .s.msg_type = WRP_MSG_TYPE__SVC_REGISTRATION,
            .s.u.reg.service_name = "service1",
            .s.u.reg.url = "url1",
 
            .r.msg_type = WRP_MSG_TYPE__AUTH,
            .r.u.auth.status = 200,
        },
    };
    size_t t_size = sizeof(tests)/sizeof(test_t);
    size_t i;

    for( i = 0; i < t_size; i++ ) {
        void *data;
        void *bytes;
        wrp_msg_t *msg;
        ssize_t data_s = wrp_struct_to(&tests[i].s, WRP_BYTES, &data);
        ssize_t bytes_s = wi_create_response_to_message(data, data_s, &bytes);
        CU_ASSERT(0 < bytes_s);
        ssize_t msg_s = wrp_to_struct(bytes, bytes_s, WRP_BYTES, &msg);
        CU_ASSERT(0 < msg_s);
        CU_ASSERT(tests[i].r.msg_type == msg->msg_type);
        CU_ASSERT(tests[i].r.u.auth.status == msg->u.auth.status);
        wrp_free_struct(msg);
        free(bytes);
    }
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
