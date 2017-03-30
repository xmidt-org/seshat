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
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
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
void test_all( void )
{
    char *response;
    int pid;
    int seshat_pid;
    int seshat_service;
#define SESHAT_URL "ipc:///tmp/sehsat_services.ipc"
#define SESHAT_PATH_NAME  "../../src/seshat"  // FixMe: Can this be done by CMake?
    char *argv[] = {
    SESHAT_PATH_NAME,
    "-f", "seshat_json.txt",
    "-u" , SESHAT_URL,
    NULL};
 
    pid = fork();
    printf("test_all:Forked %d \n", pid);
    if (0 == pid) { // child process, run seshat service
        seshat_pid = getpid();
        seshat_service = execv(SESHAT_PATH_NAME, argv);
        
    } else {// test_all main process       
     // test init routine of the library
    sleep(2);
    CU_ASSERT(0 == init_lib_seshat(SESHAT_URL));
    CU_ASSERT(0 == init_lib_seshat(SESHAT_URL));
    CU_ASSERT(0 != init_lib_seshat("ipc:///tmp/foo1.ipc"));
    
    CU_ASSERT(NULL == seshat_discover("WebPa"));    
    
    CU_ASSERT(0 == seshat_register("WebPa1", "https://WebPa1.comcast.com/webpa_"));
    CU_ASSERT(0 == seshat_register("WebPa1", "https://WebPa1.comcast.com/webpa_"));
    response = seshat_discover("WebPa1");
    CU_ASSERT(NULL != response);
    free(response);
    
    CU_ASSERT(0 == seshat_register("WebPa2", "https://WebPa2.comcast.com/webpa_"));
    CU_ASSERT(0 == seshat_register("WebPa2", "https://WebPa2.comcast.com/webpa_"));     
    response = seshat_discover("WebPa2");
    CU_ASSERT(NULL != response);
    free(response);
    
    CU_ASSERT(0 == shutdown_seshat_lib());       
    }
    
    
    /*
     * AddMe:: send_kill_signal_to_seshat_services();
     */
    (void ) seshat_service;
    kill(seshat_pid, SIGKILL);
}

void add_suites( CU_pSuite *suite )
{
    *suite = CU_add_suite( "libseshat tests", NULL, NULL );
    CU_add_test( *suite, "Test all", test_all );
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

    if( 0 != rv ) {
        return 1;
    }

    return 0;
}
