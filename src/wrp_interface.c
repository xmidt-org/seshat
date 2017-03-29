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

#include "json_interface.h"
#include "wrp_interface.h"

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef struct wrp_svc_registration_msg reg_msg_t;
typedef struct wrp_crud_msg             ret_msg_t;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static int process_message_reg( reg_msg_t *msg );
static char *process_message_ret( ret_msg_t *msg );

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
ssize_t wi_create_response_to_message(void *data, size_t cnt, void **message)
{
    wrp_msg_t *in_msg = NULL;
    wrp_msg_t response;
    ssize_t   message_size;

    memset(&response, 0, sizeof(wrp_msg_t));

    if( 0 < wrp_to_struct(data, cnt, WRP_BYTES, &in_msg) ) {
        switch (in_msg->msg_type) {
            case (WRP_MSG_TYPE__SVC_REGISTRATION): {
		response.msg_type = WRP_MSG_TYPE__AUTH;                 
		if ( 0 == process_message_reg( &(in_msg->u.reg))) {
                    response.u.auth.status =200;
                } else {
			response.u.auth.status = 400;
			}
	    }
            break;
            
            case (WRP_MSG_TYPE__RETREIVE):
            {
                ret_msg_t *in_crud  = &(in_msg->u.crud);
                ret_msg_t *out_crud = &(response.u.crud);

    		out_crud->status = 400; // default to failed
                response.msg_type = WRP_MSG_TYPE__RETREIVE;
                out_crud->transaction_uuid = strdup(in_crud->transaction_uuid);
                out_crud->source  = strdup(in_crud->dest);
                out_crud->dest    = strdup(in_crud->source);
                out_crud->headers = NULL;
                out_crud->metadata = NULL;
                out_crud->include_spans = false;
                out_crud->spans.spans = NULL;
                out_crud->spans.count = 0;
                out_crud->status  = 400;
                out_crud->rdr     = 0;
                out_crud->path    = strdup(in_crud->path);
                out_crud->payload = process_message_ret( in_crud );
                if( NULL != out_crud->payload ) {
                    out_crud->status = 200;
                }        
            }
            break;
            
	    case (WRP_MSG_TYPE__AUTH): // useles but the tests want it and I am getting tired ;-(
		response.msg_type = WRP_MSG_TYPE__AUTH;                 
                response.u.auth.status = 400;
	    break;		
    
            default:
            	response.msg_type = WRP_MSG_TYPE__AUTH;                 
                response.u.auth.status = 501; // Not Implemented
	   break;
        }
    } else {
	return -1;
    }


    message_size = wrp_struct_to(&response, WRP_BYTES, message);    
    if( WRP_MSG_TYPE__RETREIVE == response.msg_type ) {
        ret_msg_t *ret = &(response.u.crud);
        if (ret->transaction_uuid)
            free(ret->transaction_uuid);
        if (ret->dest)             
            free(ret->dest);
        if (ret->path)             
            free(ret->path);
        if (ret->payload )         
            free(ret->payload);
        if (ret->source)           
            free(ret->source);
    }
    
    if (in_msg) {
        wrp_free_struct(in_msg);
    }
       
    return message_size;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/**
 * @brief Processes wrp registration message.
 *
 * @param[in] wrp registration message.
 */
int process_message_reg( reg_msg_t *msg )
{
    char *service = msg->service_name;
    char *url     = msg->url;
    
    if (service && url) {
        ji_add_entry(service, url); 
        return 0;
    }
    
    return -1;
}

/**
 * @brief Returns a JSON object for the service the wrp CRUD retrieval message.
 * 
 * @note return JSON object buffer needs to be free()-ed by caller.
 *
 * @param[in] wrp CRUD retrieval message.
 *
 * @return char buffer of the JSON object to be retrieved.
 */
char *process_message_ret( ret_msg_t *msg )
{
    char *service = msg->payload;
    char *object  = NULL;

    if( EXIT_SUCCESS == ji_retrieve_entry(service, &object) )
    {
        return object;
    }
    return NULL;
}

