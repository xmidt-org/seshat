#ifndef _LISTENER_TASK_H_
#define _LISTENER_TASK_H_

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
#include <wrp-c/wrp-c.h>
#include "nmsg.h"

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------*/
/*                               Data Structures                              */
/*----------------------------------------------------------------------------*/
typedef struct _listener_data_t {
    int socket;
    char *url;
} listener_data_t;    

/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
int listener_thread_start(listener_data_t *data);    
    
#ifdef __cplusplus
}
#endif

#endif
