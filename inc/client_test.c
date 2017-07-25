#include"sock.h"

#include"http_ipc.h"

#include"http_task.h"

#if 1

int main(){
    

    printf("%d",open_max_fd());

   return 1;

}
#endif
#if 0

int main(int argc, char *argv[])    
{    
  
    
    int msgid = open_queue();  
     
     struct _msg msg;
     msg.mtype=1;
     int len=sizeof(struct _msg)-sizeof(long);

    // struct msgbuf *buf;    
    // buf = (struct msgbuf *)malloc(len + sizeof(msgbuf::mtype));    
    // buf->mtype = type;    
    if (msgsnd(msgid, &msg, len, IPC_NOWAIT) == -1)  //非阻塞   
        printf("23q4w5");  
      // send_queue(msgid,&msg);
}    

#endif