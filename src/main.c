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
#include "listener_task.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define OPTIONS_STRING_FORMAT "u:f:"

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

int recv_socket; //, send_socket;

#define FAKE_SERVER_CODE
#ifdef FAKE_SERVER_CODE

/*
 * For some reason '///' is required otherwise I gt a bind error ;-(/!
 ./seshat -f foo.txt -u ipc:///tmp/foo.ipc
 */

#include <pthread.h>

int fake_server_socket;
listener_data_t fake_server_data;
int fake_server_thread_start(listener_data_t *data);    
#endif


listener_data_t the_data;

int main( int argc, char **argv)
{
    int item;
    int options_index = 0;
    int str_len = 0;
    FILE *file_handle;
    int timeout_val;
    int nn_err;
    
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
    } 
    

    recv_socket = nn_socket (AF_SP, NN_PULL);
    if ( -1 == recv_socket) {
        printf("Failed to open a listener socket on %s\n", url);
        exit (-2);
    } else if (0 > (nn_err = nn_bind(recv_socket, url))) {
        printf("Failed to bind listener socket on %s, %d\n", url, nn_err);
        shutdown_receiver(recv_socket);
        exit (-3);            
    }

    timeout_val = 5000; // ms    
    if (0 != nn_setsockopt (recv_socket, NN_SOL_SOCKET, NN_RCVTIMEO,
            &timeout_val, sizeof(timeout_val))) {
        printf("Failed to set wait time out!\n");
    }

    printf("Socket %d set to listen to %s\n", recv_socket, url);

    if (NULL == file_name) {
        file_name = "/tmp/seshat_services";
    }
    
    file_handle = fopen(file_name, "w");
    
    the_data.socket = recv_socket;
    the_data.url = url;
    the_data.file_handle = file_handle;

    listener_thread_start(&the_data);   
        
#ifdef FAKE_SERVER_CODE
    sleep(1);
    printf("About to start the fake server\n");
    fake_server_socket = nn_socket(AF_SP, NN_PUSH);
    if (fake_server_socket < 0) {
        printf("Fake Server Socket Failed!\n");
        exit(-11);
    }
  
    if (nn_connect(fake_server_socket, url) < 0) {
        printf("Fake Server connect failed!\n");
        nn_shutdown(fake_server_socket, 0);
        exit(-12);
    }
    
    fake_server_data.socket = fake_server_socket;
    fake_server_data.url = url;
    fake_server_data.file_handle = NULL;   
    
    fake_server_thread_start(&fake_server_data);
    
#endif   
    
    // AddMe: start working!
    while (1) {
        
        printf("main() waiting ...\n");
        sleep(15);
        //int bytes_received = 0;
        //char *bytes = receive_msg (recv_socket, &bytes_received);
        
        //if (NULL != bytes) {
            // wrp_msg_t *msg;
            // if (0 < wrp_to_struct( bytes, bytes_received, WRP_BYTES, &msg )) {...}
            // log the message in the file?
            //
            // respond_to_message(send_socket, bytes, received_bytes);
          //  nn_freemsg(bytes);
        //}
    }
    
    shutdown_receiver(recv_socket);
    fclose(file_handle);

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


#ifdef FAKE_SERVER_CODE
static pthread_t fake_thread_id;

static void *fake_server(void *data);
static listener_data_t fake_data;
int fake_server_thread_start(listener_data_t *data)
{
    fake_data = *data;
    return pthread_create(&fake_thread_id, NULL, fake_server, (void *) &fake_data);
}


void *fake_server(void *data)
{
    listener_data_t in_data = * (listener_data_t *) data;
     
    while (1) {
        wrp_msg_t *response = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
        int bytes_sent;
        response->msg_type = WRP_MSG_TYPE__AUTH;
        response->u.auth.status = 200;
        if ((bytes_sent =  nn_send(in_data.socket, response, sizeof(wrp_msg_t), 0)) > 0) {
            printf("Sent %d bytes (size of struct %d)\n", bytes_sent, (int ) sizeof(wrp_msg_t));
        }
        else {
            printf("Sender failed to send message\n");
        }
        
        sleep(5);
        printf("Fake Server running\n");
    }
    
    return NULL;
}


#endif