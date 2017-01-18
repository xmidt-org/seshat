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
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <getopt.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include "wrp-c.h"


/*----------------------------------------------------------------------------*/
/*                                   Macros                                   */
/*----------------------------------------------------------------------------*/
#define OPTIONS_STRING_FORMAT "u:"

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                            File Scoped Variables                           */
/*----------------------------------------------------------------------------*/
/* Notes: getopts is case-sensitive */
static struct option command_line_options[] = {
    {"temp-file", required_argument, 0, 'f'},
    {"url", required_argument, 0, 'u'},
    {0,0,0,0}
};

static char *file_name = NULL;
static char *url = NULL;

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/* none */

/*----------------------------------------------------------------------------*/
/*                             External Functions                             */
/*----------------------------------------------------------------------------*/
int main( int argc, char **argv)
{
    int item;
    int options_index = 0;
    int str_len = 0;
    
    // AddMe: init_signal_handlers();
    while (-1 != (item = getopt_long(argc, argv, OPTIONS_STRING_FORMAT,
                         command_line_options, &options_index)))
    {
        switch (item) {
            case 'f':
                str_len = strlen(optarg);
                file_name = (char *) malloc(str_len + 1);
                memset(file_name, 0, str_len);
                strncpy(file_name, optarg, str_len);
                break;
            case 'u':
                str_len = strlen(optarg);
                url = (char *) malloc(str_len + 1);
                memset(url, 0, str_len);
                strncpy(url, optarg, str_len);
                break;
        }
        
    }
    
    if (NULL == url) {
     printf("%s:url is a required parameter!\n", argv[0]);
     if (file_name != NULL) {
         free(file_name);
     }
     exit(-1);
    }
    
    if (NULL == file_name) {
        file_name = "/tmp/seshat_services";
    }
    
    // AddMe: start working!
    
    return 0;
}

/*----------------------------------------------------------------------------*/
/*                             Internal functions                             */
/*----------------------------------------------------------------------------*/
/* none */
