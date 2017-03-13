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
#define COMMA          ','
#define ITEM           "url"

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef struct __ll_node {
    char *entry;
    char *value;
    struct __ll_node *next;
} ji_ll_t;

/*----------------------------------------------------------------------------*/
/*                             File Scoped Variables                          */
/*----------------------------------------------------------------------------*/
static FILE *file_handle = NULL;
static ji_ll_t *head = NULL;
static size_t ll_size = 0;
static bool cjson_init = false;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static void  __ji_cjson_init(void);
static jir_t __ji_add_node(const char *entry, const char *value);
static jir_t __ji_file_to_ll(void);
static char *__ji_node_to_cjson(ji_ll_t *node);
static jir_t __ji_ll_to_file(void);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
void ji_init(const char *file_name)
{
    file_handle = fopen(file_name, "a+");
    __ji_file_to_ll();
}

void ji_destroy()
{
    ji_ll_t *current = head;
    ji_ll_t *next;

    while( NULL != current ) {
        free(current->entry);
        free(current->value);

        next = current->next;
        free(current);
        current = next;
    } 
    ll_size = 0;

    fclose(file_handle);
} 

jir_t ji_add_entry( const char *entry, const char *value )
{
    jir_t rval = __ji_add_node(entry, value);
    if( JIRT__SUCCESS == rval ) {
        return __ji_ll_to_file();
    }
    return rval;
}

jir_t ji_retrieve_entry( const char *entry, char **object )
{
    ji_ll_t *current = head;

    while( NULL != current ) {
        if( 0 == strcmp(entry, current->entry) ) {
            *object = __ji_node_to_cjson(current);
            return JIRT__SUCCESS;
        }
        current = current->next;
    }

    *object = NULL;
    return JIRT__ENTRY_NOT_FOUND;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/**
 * @brief Initialization required for cJSON lib.
 * 
 */
static void __ji_cjson_init()
{
    cJSON_Hooks cjhooks;

    cjhooks.malloc_fn = malloc;
    cjhooks.free_fn = free;
    cJSON_InitHooks( &cjhooks );

    cjson_init = true;
}

/**
 * @brief Helper function to add/create linked list node for a given entry, value pair.
 * 
 * @note return buffer needs to be free()-ed by caller
 *
 * @param[in] name of the entry.
 * @param[in] value of the entry.
 *
 * @return status
 */
static jir_t __ji_add_node(const char *entry, const char *value)
{
    ji_ll_t *current = head;

    while( NULL != current )    {
        if( 0 == strcmp( entry, current->entry ) ) {
            if( 0 == strcmp( value, current->value) ) {
                return JIRT__ENTRY_ALREADY_PRESENT;
            }
            else {
                ll_size -= strlen(current->value) + 1;
                free(current->value);
                current->value = strdup(value);
                ll_size += strlen(current->value) + 1;
                return JIRT__SUCCESS;
            }
        }
        current = current->next;
    }

    current = (ji_ll_t *) malloc( sizeof(ji_ll_t) );
    current->entry = strdup(entry);
    current->value = strdup(value);
    ll_size += strlen(current->entry) + strlen(current->value) + 2;
    current->next = NULL;

    if( NULL == head ) {
        head = current;
    }

    return JIRT__SUCCESS;
}

/**
 * @brief Read comma delimited service/url pairs from file into linked list.
 *
 * @return status;
 */
static jir_t __ji_file_to_ll(void)
{
    size_t length;
    size_t read_size;
    char *buf, *ctr, *save;
    char *service, *url;

    if( NULL == file_handle ) {
        return JIRT__FILE_HANDLE_NULL;
    }

    fseek(file_handle, 0, SEEK_END);
    length = ftell(file_handle);
    fseek(file_handle, 0, SEEK_SET);
    buf = (char *) malloc(length);
    if( NULL == buf ) {
        return JIRT__NO_MEMORY;
    }

    read_size = fread(buf, sizeof(char), length, file_handle);
    if( 0 == read_size ) {
        return JIRT__FILE_READ_UNSUCCESSFUL;
    }

    ctr = buf;
    service = strtok_r(ctr, &(char){COMMA}, &save); ctr = NULL;
    while( NULL != service ) {
        url = strtok_r(ctr, &(char){COMMA}, &save); ctr = NULL;
        if( NULL == url ) {
            return JIRT__FILE_CONTENT_ERROR;
        }
        __ji_add_node(service, url);
        service = strtok_r(ctr, &(char){COMMA}, &save); ctr = NULL;
    }
    return JIRT__SUCCESS;
}

/**
 * @brief Creates a cJSON object from contents of a given linked list node
 * 
 * @note return buffer needs to be free()-ed by caller
 *
 * @param[in] Node of linked list that needs to be cJSON-ised.
 *
 * @return buffer of the JSON tree
 */
static char *__ji_node_to_cjson(ji_ll_t *node)
{
    cJSON *root = NULL;
    cJSON *service = NULL;
    char *buf = NULL;

    if( false == cjson_init ) {
        __ji_cjson_init();
    }
    root = cJSON_CreateObject();
    service = cJSON_CreateObject();
    cJSON_AddItemToObject(root, node->entry, service);
    cJSON_AddStringToObject(service, ITEM, node->value);

    buf = cJSON_Print(root);
    cJSON_Delete(root);
    return buf;
}

/**
 * @brief Write out linked list to file.
 * 
 * @return status
 */
static jir_t __ji_ll_to_file()
{
    ji_ll_t *current = head;
    char *buf = NULL, *ctr = NULL;
    size_t entry_len = 0, value_len = 0;

    if( false == cjson_init ) {
        __ji_cjson_init();
    }
    buf = (char *) malloc(ll_size * sizeof(char)); ctr = buf;

    while( NULL != current ) {
        entry_len = strlen(current->entry);
        value_len = strlen(current->value);

        strncpy(ctr, current->entry, entry_len);
        ctr += entry_len; *ctr = COMMA; ctr++;

        strncpy(ctr, current->value, value_len);
        ctr += value_len; *ctr = COMMA; ctr++;

        current = current->next;
    }

    if( NULL == file_handle ) {
        return JIRT__FILE_HANDLE_NULL;
    }
    fwrite(buf, sizeof(char), strlen(buf), file_handle);
    free(buf);

    return JIRT__SUCCESS;
}

