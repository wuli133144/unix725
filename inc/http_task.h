#ifndef __HTTP_TASK_H
#define __HTTP_TASK_H

#include "http_ipc.h"
#include "list.h" //========>freebsd kernelcode
#include "types.h"
#include "http_error.h"

#include<sys/resource.h>
#include<err.h>



#define NPROC_MAX_NUM        10

#define  MAX_FD_NUM          200     

#define  PROC_POOL           proc_pool


typedef struct __processor__node {
  i_32 pid;
  i_32 nleft;
  SLIST_ENTRY(__processor__node) entry;
} processor_t;


/*@global processor pool object@*/
   SLIST_HEAD(s_pool_t,__processor__node)proc_pool=SLIST_HEAD_INITIALIZER(proc_pool);

/*@global processor pool object@*/

void open_max_fd(){
     struct rlimit rlim;
     if((getrlimit(RLIMIT_NOFILE,&rlim))<0){
          err(1,"error:%s","getrlimit");
          exit(0);
     }
     if(rlim.rlim_max>MAX_FD_NUM){
         rlim.rlim_max=MAX_FD_NUM;
     }
}


processor_t *proc_alloc(){
     processor_t *rva=NULL;
     if((rval=(processor_t*)malloc(sizeof(processor_t)))==NULL){
         unix_error("malloc failed!");
     }
     return rval;
}





/*@create list @*/
void create_proc_pool(){
     processor_t *item=NULL;
     int i=0;
     for(i=0;i<NPROC_MAX_NUM;i++){
          item=proc_alloc();
          item->pid=-1;
          item->nleft=MAX_FD_NUM;
          SLIST_INSERT_HEAD(&proc_pool,item,entry);
     }
}
/*@create list end@*/

/*@insert object to pool@*/
void insert_pool_obj(pid_t pid){
      processor_t *item=NULL;
      SLIST_FOREACH(item,&PROC_POOL,entry){
           if(item->pid==-1){
                item->pid=pid;
                //lock()
                (PROC_POOL).n_proc++；
                //unlock()
                break;
           }
      } 
}
/*@insert object to pool end@*/


void delete_pool_obj(int pid){
      processor_t *item=NULL;
      processor_t *pre=SLIST_FIRST(&PROC_POOL);
      SLIST_FOREACH(item,&proc_pool,entry){
                  if(item->pid==pid){
                     if(item==SLIST_FIRST(&PROC_POOL)){
                             SLIST_FIRST(&PROC_POOL)=SLIST_NEXT(item,entry);
                             free(item);
                             item=NULL;
                             break; 
                     }
                     
                            SLIST_NEXT(pre,entry)=SLIST_NEXT(item,entry);
                            free(item);
                            item=NULL;
                            break; 

                  }
                  pre=item;
      }
      
}


/*@init manager proc@*/



void init_manager_proc(){

           //get msg queue info from main proc

         //create pool


        //create queue     
}
/*@init manager proc@*/

/*@start main processor@*/
 void init_main_proc(){
       pid_t pid;
       pid=fork();
       if(pid<0){
           unix_error("fork");
       }else if(pid==0){
            init_manager_proc();
       }
       return;
 }
/*@start main processor@*/












#endif