#include"sock.h"

#include"http_ipc.h"
#if 0

int main(){
   
   key_t key;
   
   msg_t msg;
   
  

   memcpy(msg.context,"1234",sizeof("1234"));
   key=Ftok();
   int qid=open_queue(key);
   
   printf("%d",qid);
    msg.mtype=1;
   send_queue(qid,&msg);


  

  //int clientfd=open_clientfd("127.0.0.1","8080");
  msg_t r_msg;
  rcv_queue(qid,&r_msg,1);
  printf("%s\n",r_msg.context);

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