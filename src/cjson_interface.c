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
#include <stdlib.h>
#include <cJSON.h>

#include "cjson_interface.h"

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static void __cji_init(const char *buffer, cJSON **cjson);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
cjir_t cji_add_entry( const char *entry, const char *value, char **buffer_in_out )
{
    cJSON *root = NULL;
    cJSON *object = NULL;

    if( NULL == *buffer_in_out ) {
        return CJIRT__FILE_BUFFER_NULL;
    }

    __cji_init(*buffer_in_out, &root);
    if( NULL == root ) {
        return CJIRT__CJSON_ROOT_NULL; 
    }

    object = cJSON_GetObjectItem(root, entry);
    if( NULL == object ) { 
        cJSON_AddStringToObject(root, entry, value);
    } else {
        cJSON_DeleteItemFromObject(root, entry);
        cJSON_AddStringToObject(root, entry, value);
    }

    *buffer_in_out = cJSON_Print(root);

    cJSON_Delete(object);
    cJSON_Delete(root);

    return CJIRT__SUCCESS;
}

cjir_t cji_retrieve_entry( const char *entry_name, const char *buffer_in, char **entry_out )
{
    cJSON *root = NULL;

    if( NULL == buffer_in ) {
        return CJIRT__FILE_BUFFER_NULL;
    }

    __cji_init(buffer_in, &root);
    if( NULL == root ) {
        return CJIRT__CJSON_ROOT_NULL;
    }

    *entry_out = (char *)cJSON_Duplicate(cJSON_GetObjectItem(root, entry_name), 0);

    cJSON_Delete(root);

    return CJIRT__SUCCESS;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
static void __cji_init(const char *buffer, cJSON **root)
{
   cJSON_Hooks cjhooks;

   cjhooks.malloc_fn = malloc;
   cjhooks.free_fn = free;
   cJSON_InitHooks( &cjhooks );

   *root = cJSON_Parse(buffer);
   if( NULL == root ) {
       cJSON_CreateObject();
   }
}

