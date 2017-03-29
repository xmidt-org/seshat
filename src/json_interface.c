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
ll_t *head = NULL;

/*----------------------------------------------------------------------------*/
/*                             File Scoped Variables                          */
/*----------------------------------------------------------------------------*/
static char *f_name = NULL;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
static int __add_node(const char *entry, const char *value);

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/

/* See json_interface.h for details. */
int ji_init(const char *file_name)
{
    size_t length = 0;// required
    char *buf = NULL;// required
    char *service, *url;
    FILE *file_handle;
    cJSON_Hooks cjhooks;

    if (file_name && file_name[0]) {
        f_name = strdup(file_name);
    } else {
        f_name = NULL;
        return EXIT_FAILURE;
    }
    
    file_handle  = fopen(f_name, "r");
    if( NULL == file_handle ) {
        return EXIT_FAILURE;
    }

    while( -1 != getline(&buf, &length, file_handle)) {
        char *c = strchr(buf, COMMA);
        char *n = strchr(buf, '\n');
        if( NULL == c || NULL == n ) {
            break;
        }
        *c = '\0';
        service = buf;
        *n = '\0';
        url = c + 1;
        __add_node(service, url);
    }

    free(buf);
    fclose(file_handle);

    cjhooks.malloc_fn = malloc;
    cjhooks.free_fn = free;
    cJSON_InitHooks( &cjhooks );

    return EXIT_SUCCESS;
}

/* See json_interface.h for details. */
void ji_destroy()
{
    ll_t *current = head;
    ll_t *next;

    while( NULL != current ) {
        free(current->entry);
        free(current->value);

        next = current->next;
        free(current);
        current = next;
    }
    head = NULL;
} 

/* See json_interface.h for details. */
int ji_add_entry( const char *entry, const char *value )
{
    int rval = __add_node(entry, value);
    if( 1 == rval ) {
        FILE *file_handle = fopen(f_name, "w");
        ll_t *current = head;

        if( NULL == file_handle ) {
            return EXIT_FAILURE;
        }

        while( NULL != current ) {
            fprintf(file_handle, "%s,%s\n", current->entry, current->value);
            current = current->next;
        }

        fclose(file_handle);
        return EXIT_SUCCESS;
    }
    return EXIT_SUCCESS;
}

/* See json_interface.h for details. */
int ji_retrieve_entry( const char *entry, char **object )
{
    ll_t *current = head;

    while( NULL != current ) {
        if( 0 == strcmp(entry, current->entry) ) {
            cJSON *root = NULL;
            cJSON *service = NULL;
    
            root = cJSON_CreateObject();
            service = cJSON_CreateObject();
            cJSON_AddItemToObject(root, current->entry, service);
            cJSON_AddStringToObject(service, ITEM, current->value);

            *object = cJSON_PrintUnformatted(root);
            cJSON_Delete(root);
            return EXIT_SUCCESS;
        }
        current = current->next;
    }

    *object = NULL;
    return EXIT_FAILURE;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/**
 * @brief Helper function to add/create linked list node for a given entry, value pair.
 * 
 * @note return buffer needs to be free()-ed by caller
 *
 * @param[in] name of the entry.
 * @param[in] value of the entry.
 *
 * @return 1 when the linked list has been changed, 0 otherwise.
 */
static int __add_node(const char *entry, const char *value)
{
    ll_t *current = head;
    ll_t *prev = NULL;

    while( NULL != current ) {
        if( 0 == strcmp( entry, current->entry ) ) {
            if( 0 == strcmp( value, current->value) ) {
                return 0;
            }
            else {
                free(current->value);
                current->value = strdup(value);
                return 1;
            }
        }
        prev = current;
        current = current->next;
    }

    current = (ll_t *) malloc( sizeof(ll_t) );
    current->entry = strdup(entry);
    current->value = strdup(value);
    current->next = NULL;

    if( NULL == head ) {
        head = current;
    } else {
        prev->next = current;
    }

    return 1;
}
