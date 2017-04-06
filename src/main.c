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
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <getopt.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include <time.h>
#include <wrp-c/wrp-c.h>
#include "nmsg.h"
#include "listener_task.h"
#include "json_interface.h"
#include "seshat_log.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define OPTIONS_STRING_FORMAT "u:f:t:"

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* Notes: getopts is case-sensitive */
static struct option command_line_options[] = {
    {"temp-file", required_argument, 0, 'f'},
    {"termination-time", required_argument, 0, 't'}, // use only for unit test
    {"url", required_argument, 0, 'u'},
    {0,0,0,0}
};

static char *file_name = NULL;
static char *url = NULL;

static void __sig_handler(int sig);
static void init_signal_handler(void);
static void _exit_process_(int signum);

int recv_socket; 
listener_data_t the_data;

static bool terminate_seshat;

int main( int argc, char **argv)
{
    int item;
    int options_index = 0;
    int timeout_val;
    int nn_err;
    int lifetime = 0;
    int time_passed = 0;
    time_t start_time = time(&start_time);
    
    init_signal_handler();
    terminate_seshat = false;
    while (-1 != (item = getopt_long(argc, argv, OPTIONS_STRING_FORMAT,
                         command_line_options, &options_index)))
    {
        switch (item) {
            case 'f':
                // str_len = strlen(optarg);
                file_name = strdup(optarg);
                break;
            case 'u':
                // str_len = strlen(optarg);
                url = strdup(optarg);
                break;
            case 't':
            {
                lifetime = atoi(optarg);
                if (lifetime < 0) {
                    lifetime = 0;
                }
            }
                break;
        }    
    }
    
    if (NULL == url) {
     SeshatError("%s:url is a required parameter!\n", argv[0]);
     if (file_name != NULL) {
         free(file_name);
     }
     return -1;
    } 
    

    recv_socket = nn_socket (AF_SP, NN_REP);
    if ( -1 == recv_socket) {
        SeshatError("Failed to open a listener socket on %s\n", url);
        free(url);
        return -2;
    } else if (0 > (nn_err = nn_bind(recv_socket, url))) {
        SeshatError("Failed to bind listener socket on %s, %d\n", url, nn_err);
        shutdown_receiver(recv_socket);
        free(url);
        return -3;           
    }

    timeout_val = 5000; // ms    
    if (0 != nn_setsockopt (recv_socket, NN_SOL_SOCKET, NN_RCVTIMEO,
            &timeout_val, sizeof(timeout_val))) {
        SeshatError("Failed to set wait time out!\n");
    }

    SeshatInfo("Socket %d set to listen to %s\n", recv_socket, url);

    if (NULL == file_name) {
        file_name = "/tmp/seshat_services";
    }
    
    ji_init(file_name);
    
    the_data.socket = recv_socket;
    the_data.url = url;

    listener_thread_start(&the_data);   
        
    while (false == terminate_seshat) {
	static int cnt = 0;
        
	if (0 == cnt++ % 5) {
        	SeshatInfo("main() waiting ...lifetime %d, passed_time %d\n",
 	               (int ) lifetime, time_passed);
	}
        sleep(5);
        if (lifetime > 0) {
            time_t time_now = time(&time_now);
            time_passed += time_now - start_time; 
            SeshatInfo("seshat: main(): time_passed is %d\n", time_passed);
            if ( lifetime <= time_passed ) {
                SeshatInfo("Seshat: End of Life, so sooooooon ....\n");
                break;
            }
        }
    }
    
    shutdown_receiver(recv_socket);
    ji_destroy();
    free(file_name);
    free(url);

   return 0;
}


void init_signal_handler(void)
{
      signal(SIGTERM, __sig_handler);
      signal(SIGINT, __sig_handler);
      signal(SIGUSR1, __sig_handler);
      signal(SIGUSR2, __sig_handler);
      signal(SIGSEGV, __sig_handler);
      signal(SIGBUS, __sig_handler);
      signal(SIGKILL, __sig_handler);
      signal(SIGFPE, __sig_handler);
      signal(SIGILL, __sig_handler);
      signal(SIGQUIT, __sig_handler);
      signal(SIGHUP, __sig_handler);
      signal(SIGALRM, __sig_handler);
}


void __sig_handler(int sig)
{

    if ( sig == SIGINT ) {
        signal(SIGINT, __sig_handler); /* reset it to this function */
        SeshatInfo("seshat SIGINT received!\n");
        _exit_process_(sig);
    }
    else if ( sig == SIGUSR1 ) {
        signal(SIGUSR1, __sig_handler); /* reset it to this function */
        SeshatInfo("WEBPA SIGUSR1 received!\n");
    }
    else if ( sig == SIGUSR2 ) {
        SeshatInfo("seshat SIGUSR2 received!\n");
    }
    else if ( sig == SIGCHLD ) {
        signal(SIGCHLD, __sig_handler); /* reset it to this function */
        SeshatInfo("seshat SIGHLD received!\n");
    }
    else if ( sig == SIGPIPE ) {
        signal(SIGPIPE, __sig_handler); /* reset it to this function */
        SeshatInfo("seshat SIGPIPE received!\n");
    }
    else if ( sig == SIGALRM ) {
        signal(SIGALRM, __sig_handler); /* reset it to this function */
        SeshatInfo("seshat SIGALRM received!\n");
    }
    else if( sig == SIGTERM ) {
        SeshatInfo("seshat SIGTERM received!\n");
        _exit_process_(sig);
    } else if ( sig == SIGABRT ) {
         SeshatInfo("seshat SIGABRT received!\n");
        _exit_process_(sig);       
    }
    else {
        SeshatInfo("seshat Signal %d received!\n", sig);
        _exit_process_(sig);
    }
}

void _exit_process_(int signum)
{
  SeshatInfo("seshat ready to exit!\n");
  terminate_seshat = true;
  sleep(1);
  signal(signum, SIG_DFL);
  kill(getpid(), signum);
}

const char *rdk_logger_module_fetch(void)
{
    return "LOG.RDK.Seshat";
}

