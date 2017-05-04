/**
 * @file seshat_log.h
 *
 * @description This header defines parodus log levels
 *
 * Copyright (c) 2017  Comcast
 */
 
#include <stdarg.h>
#include <cimplog/cimplog.h>

#define LOGGING_MODULE "Seshat"

/**
* @brief Enables or disables debug logs.
*/

#define SeshatError(...)   cimplog_error(LOGGING_MODULE, __VA_ARGS__)
#define SeshatInfo(...)    cimplog_info(LOGGING_MODULE, __VA_ARGS__)
#if BUILD_YOCTO
#define SeshatPrint(...)   (void)0
#else
#define SeshatPrint(...)   cimplog_debug(LOGGING_MODULE, __VA_ARGS__)
#endif 
