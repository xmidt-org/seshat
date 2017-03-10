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
#include "wrp-c.h"
#include "nmsg.h"
    
/*----------------------------------------------------------------------------*/
/*                             Function Prototypes                            */
/*----------------------------------------------------------------------------*/
/**
 *  Process an input wrp message.
 *
 *  @note If object is not NULL, object needs to be free()-ed by the caller.
 *
 *  @param data      [in]  wrp message.
 *  @param value     [in]  size of the wrp message.
 *  @param response  [out] wrp response.
 *
 */
void wi_create_response_to_message(char *data, size_t cnt, wrp_msg_t *response);

/**
 *  Free heap allocations for wrp message.
 *
 *  @param msg [in]  wrp message.
 *
 */
void wi_free(wrp_msg_t *msg);

#ifdef __cplusplus
}
#endif

#endif
