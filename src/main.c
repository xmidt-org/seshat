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
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <getopt.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include "wrp-c.h"
#include "nmsg.h"

void __attribute__((weak)) __cimplog(const char *module, int level, const char *msg, ...);

void __cimplog(const char *module, int level, const char *msg, ...) {}


/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define OPTIONS_STRING_FORMAT "u:"

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
    {"url", required_argument, 0, 'u'},
    {0,0,0,0}
};

static char *file_name = NULL;
static char *url = NULL;

static void __sig_handler(int sig);
static void init_signal_handler(void);
static void _exit_process_(int signum);

int recv_socket, send_socket;



int main( int argc, char **argv)
{
    int item;
    int options_index = 0;
    int str_len = 0;
    
    init_signal_handler();
    
    while (-1 != (item = getopt_long(argc, argv, OPTIONS_STRING_FORMAT,
                         command_line_options, &options_index)))
    {
        switch (item) {
            case 'f':
                str_len = strlen(optarg);
                file_name = (char *) malloc(str_len + 1);
                memset(file_name, 0, str_len);
                strncpy(file_name, optarg, str_len);
                break;
            case 'u':
                str_len = strlen(optarg);
                url = (char *) malloc(str_len + 1);
                memset(url, 0, str_len);
                strncpy(url, optarg, str_len);
                break;
        }
        
    }
    
    if (NULL == url) {
     printf("%s:url is a required parameter!\n", argv[0]);
     if (file_name != NULL) {
         free(file_name);
     }
     exit(-1);
    } else {
        if ( -1 == (recv_socket = connect_receiver(url)) ) {
            printf("Failed top open a listener socket on %s\n", url);
            exit (-2);
        }
        if ( -1 == (send_socket = connect_sender(url))) {
             printf("Failed top open a sender socket on %s\n", url);
             shutdown_receiver(send_socket);
             exit (-3);
        }
    }
    
    if (NULL == file_name) {
        file_name = "/tmp/seshat_services";
    }
    
    // AddMe: start working!
    while (1) {
        int bytes_received = 0;
        char *bytes = receive_msg (recv_socket, &bytes_received);
        
        if (NULL != bytes) {
            // wrp_msg_t *msg;
            // if (0 < wrp_to_struct( bytes, bytes_received, WRP_BYTES, &msg )) {...}
            // log the message in the file?
            //
            // respond_to_message(send_socket, bytes, received_bytes);
            nn_freemsg(bytes);
        }
    }
    
    shutdown_receiver(recv_socket);
    shutdown_receiver(send_socket);
    // fclose(file_handle);)

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
        printf("seshat SIGINT received!\n");
        _exit_process_(sig);
    }
    else if ( sig == SIGUSR1 ) {
        signal(SIGUSR1, __sig_handler); /* reset it to this function */
        printf("WEBPA SIGUSR1 received!\n");
    }
    else if ( sig == SIGUSR2 ) {
        printf("seshat SIGUSR2 received!\n");
    }
    else if ( sig == SIGCHLD ) {
        signal(SIGCHLD, __sig_handler); /* reset it to this function */
        printf("seshat SIGHLD received!\n");
    }
    else if ( sig == SIGPIPE ) {
        signal(SIGPIPE, __sig_handler); /* reset it to this function */
        printf("seshat SIGPIPE received!\n");
    }
    else if ( sig == SIGALRM ) {
        signal(SIGALRM, __sig_handler); /* reset it to this function */
        printf("seshat SIGALRM received!\n");
    }
    else if( sig == SIGTERM ) {
      //  signal(SIGTERM, __terminate_listener);
        printf("seshat SIGTERM received!\n");
        _exit_process_(sig);
    }
    else {
        printf("seshat Signal %d received!\n", sig);
        _exit_process_(sig);
    }
}

void _exit_process_(int signum)
{
  printf("seshat ready to exit!\n");
  signal(signum, SIG_DFL);
  kill(getpid(), signum);
}

