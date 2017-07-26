


#include"../../inc/http_task.h"
#include "../../http_module/http_module.h"
#include "../../utils/utils.h"


void sig_quit(int sig){
    exit(0);
    return ;
}

void sig_int_kill(int sig){
      
        exit(0);
        return ;
}

int start(int argc, char **argv)
{

    /*@socket description@*/


    
    int listenfd;
    int clientfd;
    socklen_t client_len;
    struct sockaddr_in clientsock;
    /*@socket description@*/

    /*@epoll var@*/

    int epfd;
    int i;
    struct epoll_event *pevent;
    struct epoll_event event;
    char buf[BUFSIZE];
    msg_t msg;
    msg.mtype=1;
    int qid;
    
    /*@signal handler@*/
    signal(SIGPIPE,SIG_IGN);
    signal(SIGCHLD,SIG_DFL);
    signal(SIGINT,sig_int_kill);
    signal(SIGQUIT,sig_quit);
    /*@signal hander end@*/
   // Init_sockpair();
    init_main_proc();//init proc pool
    
    qid=open_queue();
     /*@epoll var @*/
    /*@get socket filefd@*/
 
    printf("%s %s",sockpair.ip_addr,sockpair.port);
    listenfd = open_listenfd("127.0.0.1", "8080");
    Setnoblock(listenfd, O_NONBLOCK);
    /*@get socket filefd end@*/

    /*@epoll @*/
    bzero(buf, BUFSIZE);
    event.data.fd = listenfd;
    event.events = EPOLLIN;

    pevent = (struct epoll_event *)Malloc(BUFSIZE * sizeof(struct epoll_event));

    epfd = Epoll_create(BUFSIZE);
    
    Epoll_ctl(epfd, EPOLL_CTL_ADD, listenfd, &event);
    /*@epoll end@*/

    while (1)
    {

        int event_cnt = Epoll_wait(epfd, pevent, BUFSIZE, -1);
        for (i = 0; i < event_cnt; i++)
        {
            if (pevent[i].events & EPOLLERR || pevent[i].events & EPOLLHUP)
            {
                free(pevent);
                close(listenfd);
                unix_error("there is something wrong with socketfd");
            }
            else if (pevent[i].data.fd == listenfd)
            {
                while (1)
                {
                    printf("listenfd=%d\n",listenfd);
                     
                    clientfd = Accept(listenfd, (struct sockaddr *)&clientsock, (socklen_t *)&client_len);
                    __info("clientfd");
                    Setnoblock(clientfd, O_NONBLOCK);
                    
                    sprintf(msg.mcontext,"%d",clientfd);
                    
                    send_queue(qid,&msg);

                    // event.events = EPOLLIN | EPOLLET; //
                    // event.data.fd = clientfd;
                    // Epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &event); 
                    printf("connect fd=%d\n", clientfd);
                }
               
            }
            else if (pevent[i].events & EPOLLIN)
            {   //Read
                //int read_cnt=0;
                //dup->http_module.c
                //http_module_handler_request(pevent[i].data.fd);
                event.events = EPOLLOUT | EPOLLET; //
                event.data.fd = clientfd;
                Epoll_ctl(epfd, EPOLL_CTL_ADD, clientfd, &event);
            }
            else if (pevent[i].events & EPOLLOUT)
            { //Write
            }
            else
            {   //default
                break;
            }
        }
        
        free(pevent);
        close(listenfd);
        close(clientfd);

        return 0;
    }
}