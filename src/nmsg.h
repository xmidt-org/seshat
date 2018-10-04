/**
 * Copyright 2015 Comcast Cable Communications Management, LLC
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
#ifndef __NMSG_H__
#define __NMSG_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <nng/compat/nanomsg/nn.h>
#include <nng/compat/nanomsg/reqrep.h>
    
int connect_receiver (char *url);
int connect_sender (char *url);
void shutdown_receiver (int sock);
void shutdown_sender (int sock);
    
char *receive_msg (int sock, int *number_of_bytes);
int send_msg (const char *msg, int sock);
   
    
#ifdef __cplusplus
}
#endif


#endif
