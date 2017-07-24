
#ifndef ___HTTP__MODULE__H
#define ___HTTP__MODULE__H

#include "../../inc/unix_error.h"
#include "../../inc/http_conf.h"
#include "../../utils/utils.h"

/*@http handler start@*/
static void http_module_handler_request(void *arg);
static void http_module_handler_response(const char *filename, void *arg);
//static void http_module_recv(void *arg);
static void http_module_error(FILE *arg);

/*@http handler end@*/

#endif
