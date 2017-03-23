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
 *  Process an input message.
 *
 *  @note If message is not NULL, it needs to be free()-ed by the caller using 
 *        wi_free().
 *
 *  @param data      [in]  data received that needs to be parsed.
 *  @param value     [in]  size of the data.
 *  @param message   [out] message in response to data received.
 *
 *  @return size of valid message, < 0 otherwise.
 */
ssize_t wi_create_response_to_message(void *data, size_t cnt, void **message);

#ifdef __cplusplus
}
#endif

#endif
