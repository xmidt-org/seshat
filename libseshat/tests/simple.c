/**
 * Copyright 2017 Comcast Cable Communications Management, LLC
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 */
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <CUnit/Basic.h>

#include "../src/libseshat.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
#define SESHAT_URL "ipc:///tmp/sehsat_services.ipc"
#define SESHAT_PATH_NAME  "../../src/seshat"  // FixMe: Can this be done by CMake?
static char time_out_value_string[64];


void test_all( void )
{
    char *response;
    int pid;
    int seshat_pid;
    int seshat_service;
    int cnt = 0;
    int max_counter;
    time_t start_time;
    int cli_time_out_value = atoi(time_out_value_string);
    
    max_counter =  cli_time_out_value / 10;
    
    if (max_counter < 6) {
        max_counter = 6;
    }
    
    char *argv[] = {
    SESHAT_PATH_NAME,
    "-f", "seshat_json.txt",
    "-u" , SESHAT_URL, "-t", time_out_value_string, // lifetime in seconds
    NULL};
 
    pid = fork();
    printf("test_all:Forked %d \n", pid);
    if (0 == pid) { // child process, run seshat service
        seshat_pid = getpid();
        seshat_service = execv(SESHAT_PATH_NAME, argv);
    }
    else {// main process             
        sleep(4);
        start_time = time(&start_time);

        for (cnt = 0; cnt < max_counter; cnt++) {
            char test_buffer[64];
            char test_url[256];
            char test_service[32];

            sprintf(test_buffer, "WebPa1%d", cnt);
            response = seshat_discover(test_buffer);
            CU_ASSERT(NULL == response);
            free(response);

            CU_ASSERT(0 == init_lib_seshat(SESHAT_URL));
            CU_ASSERT(0 == init_lib_seshat(SESHAT_URL));
            CU_ASSERT(0 != init_lib_seshat("ipc:///tmp/foo1.ipc"));

            CU_ASSERT(NULL == seshat_discover("WebPa"));    
            
            sprintf(test_url, "https://WebPa1.comcast.com/webpa_%d", cnt);
            sprintf(test_service, "WepPa1_%d", cnt);
            CU_ASSERT(0 == seshat_register(test_service, test_url));
            response = seshat_discover(test_service);
            CU_ASSERT(0 != strstr(response, test_url));
            free(response);
            CU_ASSERT(0 == seshat_register(test_service, test_url));

            CU_ASSERT(0 != seshat_register(NULL, "https://WebPa2.comcast.com/webpa_"));     

            if (cli_time_out_value > 0) {
                time_t time_now = time(&time_now);
                if ( cli_time_out_value <= (time_now - start_time) ) {
                    printf("seshatlib: Ending test\n");
                    break;
                }
            }
        }

        } // main process exits
 
    CU_ASSERT(0 == shutdown_seshat_lib());       

    (void ) seshat_service;
    (void ) seshat_pid;    
   
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "libseshat tests", NULL, NULL );
    CU_add_test( *suite, "Test all", test_all );
}

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( int argc, char **argv )
{
    unsigned rv = 1;
    CU_pSuite suite = NULL;
    
    if (argc > 1) {
        snprintf(time_out_value_string, 63, "%s", argv[1]);
    } else {
        sprintf(time_out_value_string, "60");
    }
    
    printf("time_out_value_string %s\n", time_out_value_string);

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

    if( 0 != rv ) {
        return 1;
    }

    return 0;
}
