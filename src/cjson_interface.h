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
#ifndef _CJSON_INTERFACE_H_
#define _CJSON_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef enum __cj_interface_return_type {
    CJIRT__SUCCESS,
    CJIRT__FILE_BUFFER_NULL,
    CJIRT__CJSON_ROOT_NULL
} cjir_t;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/**
 *  Adds JSON name value entry to the full JSON object passed in through buffer
 *  and returned with the entry added to the buffer.
 *
 *  @param entry           [in]      name of the JSON entry.
 *  @param value           [in]      value of the JSON entry.
 *  @param buffer_in_out   [in][out] input to the JSON object and also that of the 
 *                               JSON object with name/value pair added
 *
 *  @return status of add operation
 */
cjir_t cji_add_entry( const char *entry, const char *value, char **buffer_in_out );

/**
 *  Retrieves JSON name value entry from the full JSON object passed in through buffer.
 *
 *  @param entry           [in]  name of the JSON entry.
 *  @param buffer          [in]  string buffer in which to find JSON entry.
 *  @param entry_out       [out] retrieved JSON object. 
 *
 *  @return status of add operation
 */
cjir_t cji_retrieve_entry( const char *entry_name, const char *buffer_in, char **entry_out );

#ifdef __cplusplus
}
#endif

#endif
