#ifndef __HTTP_TASK_H
#define __HTTP_TASK_H

#include "http_ipc.h"
#include "list.h" //========>freebsd kernelcode
#include "types.h"
#include "http_error.h"
#include "http_epoll.h"
#include "sys/wait.h"
#include <sys/resource.h>
#include <pthread.h>
#include <err.h>

#define NPROC_MAX_NUM 10
#define MAX_FD_NUM 200

#define PROC_POOL proc_pool

typedef struct __processor__node {
  i_32 pid;
  i_32 nleft;
  SLIST_ENTRY(__processor__node) entry;
} processor_t;

/*@global processor pool object@*/
SLIST_HEAD(s_pool_t, __processor__node)
proc_pool = SLIST_HEAD_INITIALIZER(proc_pool);

/*@global processor pool object@*/

void open_max_fd() {
  struct rlimit rlim;
  if ((getrlimit(RLIMIT_NOFILE, &rlim)) < 0) {
    err(1, "error:%s", "getrlimit");
    exit(0);
  }
  if (rlim.rlim_max > MAX_FD_NUM) {
    rlim.rlim_max = MAX_FD_NUM;
  }
}

processor_t *proc_alloc() {
  processor_t *rva = NULL;
  if ((rva = (processor_t *)malloc(sizeof(processor_t))) == NULL) {
    unix_error("malloc failed!");
  }
  rva->pid = -1;
  rva->nleft = MAX_FD_NUM;
  return rva;
}

/*@create list @*/
void create_proc_pool() {
  processor_t *item = NULL;
  int i = 0;
  for (i = 0; i < NPROC_MAX_NUM; i++) {
    item = proc_alloc();
    SLIST_INSERT_HEAD(&proc_pool, item, entry);
  }
}
/*@create list end@*/

/*@insert object to pool@*/
void insert_pool_obj(pid_t pid) {
  processor_t *item = NULL;
  SLIST_FOREACH(item, &PROC_POOL, entry) {
    if (item->pid == -1) {
      item->pid = pid;
      PROC_POOL.n_proc++;
      break;
    }
  }
}
/*@insert object to pool end@*/

void delete_pool_obj(int pid) {
  processor_t *item = NULL;
  processor_t *pre = SLIST_FIRST(&PROC_POOL);
  SLIST_FOREACH(item, &proc_pool, entry) {
    if (item->pid == pid) {
      if (item == SLIST_FIRST(&PROC_POOL)) {
        SLIST_FIRST(&PROC_POOL) = SLIST_NEXT(item, entry);
        free(item);
        item = NULL;
        break;
      }
      SLIST_NEXT(pre, entry) = SLIST_NEXT(item, entry);
      free(item);
      item = NULL;
      break;
    }
    pre = item;
  }
  PROC_POOL.n_proc--;
}
/*@delete_pool_object@*/

void insert_pool(pid_t pid) {
  processor_t *item = NULL;
  if ((PROC_POOL.n_proc) < NPROC_MAX_NUM) {
    item = proc_alloc();
    item->pid = pid;
    SLIST_INSERT_HEAD(&PROC_POOL, item, entry);
  }
}

void sig_handler(int sig) {
  msg_t msg;
  int connfd;
  msg.mtype = 2;
  int qid2 = open_queue2();
  rcv_queue(qid2, &msg, 2);
  sscanf(msg.mcontext, "%d", &connfd);
  // core
  Open_epoll(connfd);
}

void *pthread_handler(void *argv) {
  int connfd = *((int *)argv);
  Open_epoll(connfd);
  return NULL;
}

// global var

int g_oconnfd;

pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t conflock = PTHREAD_COND_INITIALIZER;

/*@child proc handler proc@*/
void jump_task_pool_obj() {

  // signal(SIGUSR1,sig_handler);
  // differ
  int i = 0;
  pthread_t pid;
  msg_t msg;
  msg.mtype = 2;
  int qid2;
  qid2 = open_queue2();

  rcv_queue(qid2, &msg, 2);
  pthread_mutex_trylock(&lock);
  sscanf(msg.mcontext, "%d", &g_oconnfd);
  pthread_mutex_unlock(&lock);
  if (msg.pid == getpid()) {

    pthread_create(&pid, NULL, pthread_handler, &g_oconnfd);

    for (;;) {
      __info("jump_task_pool_obj in loop :lock\n");
      rcv_queue(qid2, &msg, 2);
      __info("jump_task_pool_obj in loop :unlock\n");
      pthread_mutex_trylock(&lock);
      sscanf(msg.mcontext, "%d", &g_oconnfd);
      pthread_mutex_unlock(&lock);
    }

    pthread_join(pid, NULL);
  }
}
/*@child proc handler proc@*/

/*@ handler_dead_processor@*/
void handler_dead_processor(pid_t pid) {

  delete_pool_obj(pid);
  pid = fork();
  if (pid < 0) {
    unix_error("fork failed");
  } else if (pid == 0) {
    // new child
    jump_task_pool_obj();
  }
  insert_pool(pid);
  return;
}

// get best suitable proc
pid_t notice_child() {
  processor_t *item = NULL;
  pid_t pid;
  int max = SLIST_FIRST(&PROC_POOL)->nleft;

  SLIST_FOREACH(item, &PROC_POOL, entry) {
    if (item->nleft >= max) {
      max = item->nleft;
      pid = item->pid;
    }
  }
  item->nleft--;
  return pid;
}

/*@TELL CHLD TO EXIT WHEN ACCEPT SIGINT SIGNAL@*/
void tell_chld_exit() {

  processor_t *item = NULL;
  SLIST_FOREACH(item, &PROC_POOL, entry) { kill(item->pid, SIGINT); }
}
/*@TELL CHLD TO EXIT WHEN ACCEPT SIGINT SIGNAL END@*/

int common_handler_queue_impl(int msgid, int msgid2, int msgid3, msg_t *p) {
  int length, result;
  int pid = p->pid;
  length = sizeof(msg_t) - sizeof(long);
  while (1) {
    if ((result = msgrcv(msgid, p, length, 1, IPC_NOWAIT)) !=
        -1) { // main=======>master
      __info("common_handler_queue_impl");
      p->pid = pid;
      send_queue(msgid2, p); // master===>child
    } else if ((result = msgrcv(msgid2, p, length, 2, IPC_NOWAIT)) != -1) {
      //

    } else if ((result = msgrcv(msgid3, p, length, 3, IPC_NOWAIT)) !=
               -1) { // child===>main
      // error handler
      char tell[BUFSIZE];
      int i;
      bzero(tell, BUFSIZE);
      sscanf(p->mcontext, "%s", tell);
      if (strcasecmp(tell, "exit") == 0) {
        // tell child signal sigint
        tell_chld_exit();
        
         waitpid(-2,&i,0);
      }
    } else if ((result = msgrcv(msgid3, p, length, 4, IPC_NOWAIT)) !=
               -1) { // child===>main
      // error handler
      int childpid;
      sscanf(p->mcontext, "%d", &childpid);
      handler_dead_processor(childpid);
    }
  }
}

/*@init manager proc@*/

void init_manager_proc() {

  // get msg queue info from main proc
  int i = 0, clientfd;
  msg_t msg;
  pid_t pid;

  // create pool
  create_proc_pool();
  create_queue();
  for (; i < NPROC_MAX_NUM; i++) {

    pid = fork();
    if (pid < 0) {
      unix_error("fork failed!");
    } else if (pid == 0) {

      jump_task_pool_obj();
    }
    insert_pool_obj(pid);
  }
  __info("manager open");
  while (1) {
    // receive msg from main proc
    // lock()
    printf("parent function head\n");
    pid = notice_child();
    msg.pid = pid;
    common_handler_queue_impl(arr[0], arr[1], arr[2], &msg);
    // unlock()
    printf("parent function end");
  }

  destroy_queue(); // destroy
}
/*@init manager proc@*/

/*@start main processor@*/
void init_main_proc() {
  pid_t pid;
  pid = fork();
  if (pid < 0) {
    unix_error("fork");
  } else if (pid == 0) {
    init_manager_proc();
  }
  return;
}
/*@start main processor@*/

#endif