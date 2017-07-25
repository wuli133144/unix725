

#ifndef __UNIX_ERROR__H
#define __UNIX_ERROR__H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include<stdarg.h>


static void inline_unix_error(const char *msg) {
  fprintf(stderr, "error::%s::%s\n", msg, strerror(errno));
  exit(0);
}



void unix_error(const char *msg){
      inline_unix_error(msg);
}

#endif
