#ifndef __HTTP__IPC__H
#define __HTTP__IPC__H

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <err.h>

#define FILEPATH "."

#define MODE 1

#ifndef BUFSIZE
#define BUFSIZE 1024
#else
#undef BUFSIZE
#endif

typedef struct _msg {
  long mtype;
  char mcontext[BUFSIZE];
} msg_t;

// ftok;
key_t Ftok() {
  key_t rval;
  if ((rval = ftok(FILEPATH, MODE)) == -1) {
    err(1, "error:%s", "ftok");
  }
  return rval;
}

// int    msgget ( key_t  key , int  msgflg );
int open_queue() {
  int qid;
  if ((qid == msgget(Ftok(".", "1"), O_CREAT | 0666)) == -1) {
    err(1, "error:%s", "msgget");
  }
  return qid;
}

int open_queue2(){
  int qid;
   if ((qid == msgget(Ftok(".", "2"), O_CREAT | 0666)) == -1) {
    err(1, "error:%s", "msgget");
  }
  return qid;
  
}
int open_queue3(){
   int qid;
   if ((qid == msgget(Ftok(".", "3"), O_CREAT | 0666)) == -1) {
    err(1, "error:%s", "msgget");
  }
  return qid;
}

// int  msgsnd ( int msgid ,  struct msgbuf*msgp , int msgsz, int msgflg );

int send_queue(int msgid, msg_t *p) {
  int length, result;

  length = sizeof(msg_t) - sizeof(long);

  if ((result = msgsnd(msgid, p, length, 0) == -1)) {
    err(1, "error:%s", "msgsnd");
  }
  return result;
}

// int  msgrcv( int  msgid , struct   msgbuf*  msgp ,  int msgsz ,  long msgtyp,
// int msgflg);

int rcv_queue(int msgid, msg_t *p, long type) {
  int length, result;
  length = sizeof(msg_t) - sizeof(long);
  if ((result = msgrcv(msgid, p, length, type, 0)) == -1) {
    err(1, "error:%s", "msgrcv");
  }
  return result;
}




#endif