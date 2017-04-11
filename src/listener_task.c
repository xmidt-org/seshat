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
#include "seshat_log.h"

/*----------------------------------------------------------------------------*/
/*                             File Scoped Variables                          */
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

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/**
 *  @brief listener task.
 * 
 *  @param[in] required data.
 */
static void *listener(void *data)
{
    listener_data_t in_data = * (listener_data_t *) data;
    char *in_buf = NULL; 
    void *out_buf = NULL;
    
    sleep(10);
    
    while( 1 ) {
        int in_size = nn_recv (in_data.socket, &in_buf, NN_MSG, 0);
        SeshatInfo("listener got %d bytes\n", in_size);
        if( 0 < in_size ) {
            ssize_t out_size = wi_create_response_to_message(in_buf, in_size, &out_buf);
            SeshatInfo("listener to send %zd bytes\n", out_size);
            if( 0 < out_size ) {
                if ( 0 > nn_send(in_data.socket, out_buf, out_size, 0) ) {
                    free(out_buf);
                }
            }
            nn_freemsg(in_buf);
        } 
        else {
            // do we need to check for socket error other than timed out ?
            SeshatPrint("listener timed out or socket error?\n");
            continue;
        }
        sleep(1);
        SeshatPrint("listener running\n");
    }
    
    return NULL;
}
