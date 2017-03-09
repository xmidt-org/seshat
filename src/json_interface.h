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
#ifndef _JSON_INTERFACE_H_
#define _JSON_INTERFACE_H_

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef enum __json_interface_return_type {
    JIRT__SUCCESS,
    JIRT__FILE_BUFFER_NULL,
    JIRT__ROOT_NULL,
    JIRT__ENTRY_ALREADY_PRESENT,
    JIRT__ENTRY_NOT_FOUND
} jir_t;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/**
 *  Adds JSON name value entry to the full JSON object passed in through buffer
 *  and returned with the entry added to the buffer.
 *
 *  @param entry      [in] name of the JSON entry.
 *  @param value      [in] value of the JSON entry.
 *
 *  @return status of add operation
 */
jir_t ji_add_entry( const char *entry, const char *value );

/**
 *  Retrieves JSON name value entry from the full JSON object passed in through buffer.
 *
 *  @note If entry is not NULL, entry needs to be free()-ed by the caller.
 *
 *  @param entry  [in]  name of the JSON entry.
 *  @param entry  [out] retrieved JSON object. 
 *
 *  @return status of add operation
 */
jir_t ji_retrieve_entry( const char *entry_name, char **entry );

#ifdef __cplusplus
}
#endif

#endif

