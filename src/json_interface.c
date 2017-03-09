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
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <cJSON.h>

#include "json_interface.h"

/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define ITEM "url"

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
extern FILE *g_file_handle;

typedef struct __ll_node {
    char *entry;
    char *value;
    struct __ll_node *next;
} ji_ll_t;

/*----------------------------------------------------------------------------*/
/*                             File Scoped Variables                          */
/*----------------------------------------------------------------------------*/
static ji_ll_t *head = NULL;
static bool cjson_init = false;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static jir_t __ji_persist(void);
static char *__ji_cjson_create(ji_ll_t *node);
static void  __ji_cjson_init(void);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
jir_t ji_add_entry( const char *entry, const char *value )
{
    ji_ll_t *current = head;
    size_t entry_len = strlen(entry);
    size_t value_len = strlen(value);

    while( NULL != current )
    {
        if( 0 == strncmp( entry, current->entry, entry_len ) )
        {
            if( 0 == strncmp( value, current->value, value_len ) )
            {
                return JIRT__ENTRY_ALREADY_PRESENT;
            } 
            else
            {
                free(current->value);
                current->value = (char *) malloc( value_len + 1 );
                strncpy( current->value, value, (value_len + 1) ); 
                current->value[value_len] = '\0';
                return __ji_persist();
            }
        }
        current = current->next;
    }

    current = (ji_ll_t *) malloc( sizeof(ji_ll_t) );
    current->entry = (char *) malloc( entry_len + 1 );
    current->value = (char *) malloc( value_len + 1 );

    strncpy( current->entry, entry, entry_len );
    current->entry[entry_len] = '\0';
    strncpy( current->value, value, value_len );
    current->value[value_len] = '\0';
    current->next = NULL;

    if( NULL == head ) {
        head = current;
    }
    printf("head = %p\n", head);
    printf("head entry = %s, head value = %s\n", head->entry, head->value);

    return __ji_persist();
}

jir_t ji_retrieve_entry( const char *entry_name, char **entry )
{
    ji_ll_t *current = head;

    while( NULL != current )
    {
        if( 0 == strncmp(entry_name, current->entry, strlen(entry_name)) )
        {
            *entry = __ji_cjson_create(current);
            return JIRT__SUCCESS;
        }
        current = current->next;
    }

    *entry = NULL;
    return JIRT__ENTRY_NOT_FOUND;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/**
 * @brief Write out linked list to file.
 * 
 * @return status
 */
static jir_t __ji_persist()
{
    cJSON *root = NULL;
    ji_ll_t *current = head;
    char *buf = NULL;

    if( false == cjson_init )
    {
        __ji_cjson_init();
    }
    root = cJSON_CreateObject();

    while( NULL != current )
    {
        cJSON *service = cJSON_CreateObject();

        cJSON_AddItemToObject(root, current->entry, service);
        cJSON_AddStringToObject(service, ITEM, current->value);

        current = current->next;
    }

    buf = cJSON_Print(root);
    cJSON_Delete(root);

    fwrite(buf, sizeof(char), strlen(buf), g_file_handle);

    return JIRT__SUCCESS;
}

/**
 * @brief Creates a cJSON object from contents of a given linked list node
 * 
 * @note return buffer needs to be free()-ed by caller
 *
 * @param[in] Node of linked list that needs to be cJSON-ised.
 *
 * @return  buffer of the JSON tree
 */
static char *__ji_cjson_create(ji_ll_t *node)
{
    cJSON *root = NULL;
    cJSON *service = NULL;
    char *buf = NULL;

    if( false == cjson_init )
    {
        __ji_cjson_init();
    }
    root = cJSON_CreateObject();
    service = cJSON_CreateObject();
    cJSON_AddItemToObject(root, node->entry, service);
    cJSON_AddStringToObject(service, ITEM, node->value);

    buf = cJSON_Print(service);
    cJSON_Delete(root);
    return buf;
}

static void __ji_cjson_init()
{
    cJSON_Hooks cjhooks;

    cjhooks.malloc_fn = malloc;
    cjhooks.free_fn = free;
    cJSON_InitHooks( &cjhooks );
}
