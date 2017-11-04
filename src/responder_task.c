/**
 * Copyright 2016 Comcast Cable Communications Management, LLC
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
#include "responder_task.h"
#include <pthread.h>



static pthread_t thread_id;

static void responder(void *data);
static int the_socket;

int responder_thread_start(int socket)
{
    the_socket = socket;
    return pthread_create(&thread_id, NULL, responder, (void *) &the_socket);
}


void responder(void *data)
{
    (void ) data;
    while (1) {
        sleep(10);
        printf("responder running\n");
    }
    
}
