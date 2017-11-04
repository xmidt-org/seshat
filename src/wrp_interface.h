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
#ifndef _WRP_INTERFACE_H_
#define _WRP_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif
    
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include <wrp-c/wrp-c.h>
#include "nmsg.h"
    
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/**
 *  Process an input message.
 *
 *  @note If message is not NULL, it needs to be free()-ed by the caller using 
 *        wi_free().
 *
 *  @param data      [in]  data received that needs to be parsed.
 *  @param value     [in]  size of the data.
 *  @param message   [out] message in response to data received.
 *
 *  @return size of valid message, < 0 otherwise.
 */
ssize_t wi_create_response_to_message(void *data, size_t cnt, void **message);

#ifdef __cplusplus
}
#endif

#endif
