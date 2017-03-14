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
/* none */

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/**
 *  Inits file handling.
 *
 *  @param file_name  [in]  name of the file.
 *
 *  @return status of add operation
 */
int ji_init(const char *file_name);

/**
 *  Shutdown file handling.
 */
void ji_destroy(void);

/**
 *  Adds item to JSON object.
 *
 *  @param entry     [in]  name of the JSON item.
 *  @param value     [in]  value of the JSON item.
 *
 *  @return status of add operation
 */
int ji_add_entry( const char *entry, const char *value );

/**
 *  Retrieves JSON object.
 *
 *  @note If object is not NULL, it needs to be free()-ed by the caller.
 *
 *  @param entry   [in]  name of the JSON item.
 *  @param object  [out] retrieved JSON object. 
 *
 *  @return status of retrieve operation
 */
int ji_retrieve_entry( const char *entry, char **object );

#ifdef __cplusplus
}
#endif

#endif
