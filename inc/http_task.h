#ifndef __HTTP_TASK_H
#define __HTTP_TASK_H

#include "http_ipc.h"
#include "list.h" //========>freebsd kernelcode
#include "types.h"


#include<sys/resource.h>


#define NPROC_MAX_NUM        10

#define  MAX_FD_NUM          200         


typedef struct __processor__node {
  i_32 pid;
  i_32 nleft;
  SLIST_ENTRY(__processor__node) entry;
} processor_t;


/*@global processor pool object@*/
   SLIST_HEAD(s_pool_t,__processor__node)proc_pool=SLIST_INIT(&proc_pool);

/*@global processor pool object@*/

int open_max_fd(){
     struct rlimt rlim;
     if((getrlimit(RLIMIT_NOFILE,&rlim))<0){
          err(1,"error:%s","getrlimit");
          exit(0);
     }
     if(rlim.rlim_max>MAX_FD_NUM){
         rlim.rlim_max=MAX_FD_NUM;
     }
     return rlim.rlim_max;
}





#endif