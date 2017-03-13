
#include <stdbool.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <assert.h>
#include <nanomsg/nn.h>
#include <nanomsg/pipeline.h>
#include "wrp-c.h"
#include "nmsg.h"
#include <lib_seshat.h>

int seshat_register(const char *service, const char *url)
{
    (void) url; (void) service;
    return 0;
}

char* seshat_discover(const char *service)
{
    (void) service;
    retunr NULL;
}
