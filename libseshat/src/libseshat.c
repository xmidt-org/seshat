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
#include <assert.h>
#include <errno.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include <nanomsg/reqrep.h>
#include <uuid/uuid.h> // Fedora build requires "libuuid-devel"
#include "libseshat.h"

#include <wrp-c/wrp-c.h>

#include <cimplog/cimplog.h>

#define LOGGING_MODULE "_libseshat_"

#define LibSeshatError(...)   cimplog_error(LOGGING_MODULE, __VA_ARGS__)
#define LibSeshatInfo(...)    cimplog_info(LOGGING_MODULE, __VA_ARGS__)
#define LibSeshatPrint(...)   cimplog_debug(LOGGING_MODULE, __VA_ARGS__)

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define UUID_STRING_SIZE (36 + 2) // make it even boundary (vs 36 + 1)
#define URL_SUBSTR_MARKER      "\"url\":"

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static char *__current_url_ = NULL;
static int __socket_handle_ = -1;
static int __end_point_     = -1;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int init_lib_seshat(const char *url);
bool lib_seshat_is_initialized(void);
char *discover_service_data(const char *service);
int register_service_(const char *service, const char *url);
bool send_message(int wrp_request, const char *service,
                  const char *url, char *uuid);
int wait_for_reply(wrp_msg_t **msg, char *uuid_str);


/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
/* See libseshat.h for details. */
int shutdown_seshat_lib (void)
{
    int rv = nn_shutdown(__socket_handle_, __end_point_);
    free(__current_url_);
    __current_url_ = NULL;
    __socket_handle_ = -1;
    __end_point_     = -1;

    return rv;
}

/* See libseshat.h for details. */
/* returns 0 on success */
int seshat_register( const char *service, const char *url )
{
    int result = -1;
    
    if (service && url && __current_url_) {
        result = register_service_(service, url);
        errno = EAGAIN; // Need to set this appropriately
    }
    
    return result;
    
}

/* See libseshat.h for details. */
char *seshat_discover( const char *service )
{
    char *response = NULL;
    char *url = NULL;
    
    if (lib_seshat_is_initialized()) {
        if (NULL != (response = discover_service_data(service))) {
            char *substr = strstr(response, URL_SUBSTR_MARKER);
            substr += sizeof(URL_SUBSTR_MARKER);
            if (NULL != substr) {
                char *end = strstr(substr, "\"}");
                if (NULL != end) {
                    size_t size = (size_t)(end - substr);
                    url = strndup(substr, size);
                    url[size] = '\0';
                }
            }
            free(response);
            errno = 0; 
        } else {
            errno = EAGAIN;
        }
    }

    return url;
}

/* See libseshat.h for details. */
int init_lib_seshat(const char *url)
{
    int timeout_val = 7001;

    assert(url);

    if (NULL != __current_url_) {
        if (0 == strcmp(url, __current_url_)) {
            LibSeshatInfo("init_lib_seshat: Already Initialized!\n");
            return 0;
        }

        LibSeshatError("init_lib_seshat: Re-Init with different URL not allowed!\n");
        return -1;
    }


    __current_url_ = strdup(url);

    __socket_handle_ = nn_socket(AF_SP, NN_REQ);

    assert(__socket_handle_ >= 0);

    if (0 != nn_setsockopt (__socket_handle_, NN_SOL_SOCKET, NN_RCVTIMEO,
            &timeout_val, sizeof(timeout_val))) {
        free(__current_url_);
        __current_url_ = NULL;
        __socket_handle_ = -1;
        __end_point_     = -1;
        return -1;
    }

    __end_point_ = nn_connect(__socket_handle_, __current_url_);
    if (0 > __end_point_) {
        nn_shutdown(__socket_handle_, 0);
        free(__current_url_);
        __current_url_ = NULL;
        __socket_handle_ = -1;
        __end_point_     = -1;
        return -1;
    }

    return 0;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
bool lib_seshat_is_initialized(void)
{
    
    return (__current_url_ && __socket_handle_ >= 0);
    
}


char *discover_service_data(const char *service)
{
    uuid_t uuid;
    char *uuid_str;
    char *response = NULL;
    
    if (NULL == service) {
            LibSeshatError("discover_service_data() null service!\n"); 
            return response;
    }
    
    uuid_str = (char *) malloc(UUID_STRING_SIZE);
    memset(uuid_str, 0, UUID_STRING_SIZE);
    uuid_generate_time_safe(uuid);
    uuid_unparse_lower(uuid, uuid_str);
    LibSeshatInfo("discover_service_data() uuid string: %s\n", uuid_str);   
    if (send_message(WRP_MSG_TYPE__RETREIVE, service,
                     (const char *) NULL, uuid_str))
    {
        wrp_msg_t *msg = NULL;
        LibSeshatInfo("discover_service_data() waiting ...\n");
        if (0 == wait_for_reply(&msg, uuid_str)) {   
            LibSeshatInfo("discover_service_data() status %d, type %d\n", msg->u.crud.status, msg->msg_type);
            if (WRP_MSG_TYPE__RETREIVE == msg->msg_type && 
                200 == msg->u.crud.status)
            {
              response = strdup(msg->u.crud.payload);
            }
            wrp_free_struct(msg);
       } else {
           LibSeshatError("discover_service_data() Failed!!\n"); 
       }
    }
    
    free(uuid_str);
    return response;
}

int register_service_(const char *service, const char *url)
{
    bool result;
    wrp_msg_t *msg = NULL;
    
    result = send_message(WRP_MSG_TYPE__SVC_REGISTRATION, service,
                          url, NULL
                          );

    if (result && wait_for_reply(&msg, NULL) > 0) {
        if (200 == msg->u.auth.status && 
            WRP_MSG_TYPE__SVC_REGISTRATION == msg->msg_type)
        {
            result = true;
        }
    }    
   
    if (msg) {
       wrp_free_struct(msg);
    }
    
    return (result ? 0 : -1);
}

bool send_message(int wrp_request, const char *service,
                  const char *url, char *uuid)
{
    wrp_msg_t *msg;
    int bytes_sent;
    int payload_size;
    char *payload_bytes;
    
    assert(service);
    msg = (wrp_msg_t *) malloc(sizeof(wrp_msg_t));
    memset(msg, 0, sizeof(wrp_msg_t));
    
    switch (wrp_request) {
        case WRP_MSG_TYPE__RETREIVE:
            msg->u.crud.path = (char *) service;
            msg->u.crud.transaction_uuid = uuid;
            msg->u.crud.source  = (char *) "lib://libseshat";
            msg->u.crud.dest    = (char *) __current_url_;
            msg->u.crud.payload = (char *) service;
            
            break;
        case WRP_MSG_TYPE__SVC_REGISTRATION:
            msg->u.reg.url             = (char *) url;
            msg->u.reg.service_name    = (char *) service;
            break;
        default : 
            free(msg);
            return false;
    }

    msg->msg_type = wrp_request;
    
    payload_size =  wrp_struct_to( (const wrp_msg_t *) msg, WRP_BYTES,
                                   (void **) &payload_bytes);
    
    free(msg);
    
    if (1 > payload_size) {
        return false;
    }
    
    if ((bytes_sent =  nn_send(__socket_handle_, payload_bytes, payload_size, 0)) > 0) {
        LibSeshatInfo("libseshat: Sent %d bytes (size of struct %d)\n", bytes_sent, (int ) payload_size);
    }

    free(payload_bytes);
    
    return (bytes_sent == (int ) payload_size);
}

/*
 * Caller must free msg on success return of 0.
 * returns -1 on failure 
 */
int wait_for_reply(wrp_msg_t **msg, char *uuid_str) 
{
    int bytes;
    ssize_t wrp_len;
    char *buf;    
    *msg = NULL;
    
    bytes = nn_recv (__socket_handle_, &buf, NN_MSG, 0);

    if (0 >= bytes) {
        LibSeshatError("wait_for_reply() nn_recv failed\n");
        return -1;
    }
   
    wrp_len = wrp_to_struct ( buf, bytes, WRP_BYTES, msg);
   
    nn_freemsg(buf);
   
    if (0 >= wrp_len || (NULL == *msg)) {
        LibSeshatError("wait_for_reply() wrp_to_struct failed\n");        
        return -1;
    }
 
    if (NULL == uuid_str) {
        LibSeshatInfo("wait_for_reply(): No UUID Required, passed.\n");
        return 0;
    }

    LibSeshatInfo("wait_for_reply() transaction_uuid %s, type %d",
           (*msg)->u.crud.transaction_uuid, (*msg)->msg_type);
    
    if ((*msg)->u.crud.transaction_uuid && 
        (*msg)->u.crud.transaction_uuid[0] && 
        (0 == strcmp(uuid_str, (*msg)->u.crud.transaction_uuid))) {
        LibSeshatInfo("wait_for_reply() Valid UUID\n");
        return 0;
    } else {
        wrp_free_struct(*msg);
        *msg = NULL;
    }
   
   return -1;   
}

