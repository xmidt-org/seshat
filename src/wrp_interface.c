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
#include <stdio.h>

#include "cjson_interface.h"
#include "wrp_interface.h"

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef struct wrp_svc_registration_msg reg_msg_t;
typedef struct wrp_crud_msg             ret_msg_t;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
size_t file_to_string( FILE *handle, char **buffer );
void process_message_reg( reg_msg_t *msg, char **buf );
char *process_message_ret( ret_msg_t *msg, char *buf );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
size_t create_response_to_message( char *data, size_t cnt, FILE *file_handle )
{
    wrp_msg_t *msg;
    size_t response_code = 400;
    
    if( 0 < wrp_to_struct(data, cnt, WRP_BYTES, &msg) ) 
    {
        if( msg->msg_type == WRP_MSG_TYPE__SVC_REGISTRATION || msg->msg_type == WRP_MSG_TYPE__RETREIVE ) 
        {
            char *buf;
            file_to_string(file_handle, &buf);
            if( WRP_MSG_TYPE__SVC_REGISTRATION == msg->msg_type ) {
                process_message_reg( &(msg->u.reg), &buf );
            } else {
                char *object = process_message_ret( &(msg->u.crud), buf );
                (void) object;
                /* TODO - Retrieved JSON object should be sent to requester. */
            }
            response_code = 200;
            fwrite(buf, sizeof(char), strlen(buf), file_handle);
            free(buf);
        }
        wrp_free_struct(msg);
    }
    
    return response_code;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
size_t file_to_string( FILE *handle, char **buffer )
{
    size_t length;
    if( NULL == handle )
    {
        return 0;
    }

    fseek(handle, 0, SEEK_END);
    length = ftell(handle);
    fseek(handle, 0, SEEK_SET);
    *buffer = (char *) malloc(length);
    if( NULL == buffer )
    {
        return 0;
    }
    return fread(*buffer, 1, length, handle);
}

void process_message_reg( reg_msg_t *msg, char **buffer )
{
    char *service = msg->service_name;
    char *url     = msg->url;

    cji_add_entry(service, url, buffer);
}

char *process_message_ret( ret_msg_t *msg, char *buffer )
{
    char *service = (char *)msg->payload;
    char *object = NULL;

    if( CJIRT__SUCCESS == cji_retrieve_entry(service, buffer, &object) )
    {
        return object;
    }
    return NULL;
}
