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
#include <pthread.h>

#include "listener_task.h"
#include "wrp_interface.h"

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
static pthread_t thread_id;
static listener_data_t the_data;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static void *listener(void *data);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int listener_thread_start(listener_data_t *data)
{
    the_data = *data;
    return pthread_create(&thread_id, NULL, listener, (void *) &the_data);
}

void *listener(void *data)
{
    listener_data_t in_data = * (listener_data_t *) data;
    char *buf = NULL;
    
    sleep(10);
    
    while (1) {
        int bytes = nn_recv (in_data.socket, &buf, NN_MSG, 0);
        if( bytes > 0 ) {
            wrp_msg_t response;
#ifndef FAKE_SERVER_CODE
            response.msg_type = WRP_MSG_TYPE__AUTH;
#else
            response.msg_type = 911;            
#endif
             printf("listener got %d bytes\n", bytes);
             response.u.auth.status = create_response_to_message(buf, bytes);
             nn_send(in_data.socket, &response, sizeof(wrp_msg_t), 0);
             nn_freemsg(buf);
        } else {
            // do we need to check for socket error other than timed out ?
            printf("listener timed out or socket error?\n");
            continue;
        }
        sleep(1);
        printf("listener running\n");
    }
    
    return NULL;
}
