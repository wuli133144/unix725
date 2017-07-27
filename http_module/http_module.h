
#ifndef ___HTTP__MODULE__H
#define ___HTTP__MODULE__H

#include "../inc/http_error.h"
#include "../inc/http_config.h"
#include "../utils/utils.h"


///////////////////////

#define  HTTP_ERROR_NOT_FIND      404
#define  HTTP_ERROR_METHOD        500
#define  HTTP_ERROR_REQUEST       400
#define  HTTP_SUCCESS             200
///////////////////////




/*@http handler start@*/
 void http_module_handler_request(int epollfd,void *arg);
 void http_module_handler_response(const char *filename, int arg);
//static void http_module_recv(void *arg);
 void http_module_error(FILE *arg);

 static ssize_t readline(int fd,void *buf,size_t maxlen);
 static ssize_t http_rcv(int fd,void *buf,size_t len);
 static ssize_t http_send(int fd,void *buf,size_t len);

/*@http handler end@*/
/*@param_accept@*/




#define SERVER_TYPE "httpd of file get and sent system"
#define BUFFSIZE 1024
#define PARTSIZE 512

int  accept_request(int socketFd,char *localPath){
    char buf[BUFFSIZE];
    char method[PARTSIZE];
    char url[PARTSIZE];
    struct stat myStat;
    int i=0,j=0;
    int nread=0;
    //测试数据解析，读取数据函数协调加
    nread=readline(socketFd,buf,BUFFSIZE);
    
    while(nread==-1){
         nread=readline(socketFd,buf,BUFFSIZE);
    }

    while (!isspace(buf[j])&&i<PARTSIZE) {
        method[i++]=buf[j++];
    }
    method[i]='\0';//读取出get 或 post 方法
    printf("%s\n",method);
    i=0;
    while (isspace(buf[j])&&j<BUFFSIZE) {
        j++;
    }
    while (!isspace(buf[j])&&i<PARTSIZE) {
        url[i++]=buf[j++];
    }
    url[i]='\0';//读取出网络路径
    printf("%s\n",url);
    
    if (strcasecmp(method, "GET") && strcasecmp(method, "POST"))
    {
        printf("not post and get \n");
        // error : 5
        localPath=NULL;
        return HTTP_ERROR_METHOD;
    }
    
    sprintf(localPath, "/home/jackwu/Desktop/test%s", url); //获取本地路径，加上本地前缀
    if(localPath[strlen(localPath)-1]=='/'){
        strcat(localPath, "index.html");
    }//获得本地文件路径，开始寻找文件
    if(stat(localPath, &myStat)==-1){
        printf("404 unkonow \n");
        //丢弃header信息
        localPath=NULL;
        return HTTP_ERROR_NOT_FIND;
        // error : 404
    }
   else{
        if((myStat.st_mode & S_IFMT)==S_IFDIR){//是个文件夹
            strcat(localPath, "/index.html");
        }
        printf("最终路径：%s\n",localPath);        
        return HTTP_SUCCESS;
    }
}

void response_ok(int sockfd)
{
    char buf[BUFFSIZE];
    // 200
    strcpy(buf, "HTTP/1.0 200 OK\r\n");
    http_send(sockfd, buf, strlen(buf));
    strcpy(buf, SERVER_TYPE);
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "Content-Type: text/html\r\n");
    http_send(sockfd, buf, strlen(buf));
    strcpy(buf, "\r\n");
    http_send(sockfd, buf, strlen(buf));
}

//各种错误消息返回：
void unknow(int sockfd){
    //404
    char buf[BUFFSIZE];
    sprintf(buf,"HTTP/1.0 404 NOT FOUND\r\n");
    http_send(sockfd,buf,strlen(buf));
    sprintf(buf,SERVER_TYPE);
    http_send(sockfd,buf,strlen(buf));
    sprintf(buf,"Content-Type: text/html\r\n");
    http_send(sockfd,buf,strlen(buf));
    sprintf(buf,"\r\n");
    http_send(sockfd,buf,strlen(buf));
    sprintf(buf, "<HTML><TITLE>Not Found</TITLE>\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "<BODY><P>The server could not fulfill\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "your request because the resource specified\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "is unavailable or nonexistent.\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "</BODY></HTML>\r\n");
    http_send(sockfd, buf, strlen(buf));
}

void error_method(int sockfd)
{
    char buf[BUFFSIZE];
    // 501  not post and get
    sprintf(buf, "HTTP/1.0 501 Method Not Implemented\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, SERVER_TYPE);
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "Content-Type: text/html\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "<HTML><HEAD><TITLE>Method Not Implemented\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "</TITLE></HEAD>\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "<BODY><P>HTTP request method not supported.\r\n");
    http_send(sockfd, buf, strlen(buf));
    sprintf(buf, "</BODY></HTML>\r\n");
    http_send(sockfd, buf, strlen(buf));
}

void bad_request(int sockfd)
{
    char buf[BUFFSIZE];
    // 400  请求有问题
    sprintf(buf, "HTTP/1.0 400 BAD REQUEST\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "<P>Your browser sent a bad request, ");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    http_send(sockfd, buf, sizeof(buf));
}

void Conti_request(int sockfd)
{
    char buf[BUFFSIZE];
    // 100 继续传递
    sprintf(buf, "HTTP/1.0 100 REQUEST not full\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "<P>Your browser sent a missing data, ");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    http_send(sockfd, buf, sizeof(buf));
}

void wrong_path(int sockfd)
{
    char buf[BUFFSIZE];
    // 303  换个位置
    sprintf(buf, "HTTP/1.0 303 BAD PATH\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "<P>Your browser sent a wrong localpath, ");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    http_send(sockfd, buf, sizeof(buf));
}


void toolongUrl(int sockfd)
{
    char buf[BUFFSIZE];
    // 411  请求有问题
    sprintf(buf, "HTTP/1.0 411 BAD REQUEST\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "<P>Your browser sent a so long request that cant read full, ");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    http_send(sockfd, buf, sizeof(buf));
}


void bad_http(int sockfd)
{
    char buf[BUFFSIZE];
    // 505  http协议不支持
    sprintf(buf, "HTTP/1.0 505 BAD REQUEST\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "Content-type: text/html\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "\r\n");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "<P>Your http is not fit, ");
    http_send(sockfd, buf, sizeof(buf));
    sprintf(buf, "such as a POST without a Content-Length.\r\n");
    http_send(sockfd, buf, sizeof(buf));
}

/*@param_accept end@*/






static ssize_t  http_rcv(int fd,void *buf,size_t len){
       
       int nleft=len;
       void *cur_ptr=buf;
       ssize_t nread=0;
       while(nleft>0){
           
            if((nread=recv(fd,cur_ptr,nleft,0)<0)){
                  if(errno==EINTR){//intrupt
                    
                     nread=0;
                     continue;
                  }

                  //break;
                  return (-1);//error
            }else if(nread==0){//eof  file end
              break;
            }

           nleft-=nread;// socket buffer
           cur_ptr+=nread;               
       }

       return (cur_ptr-buf);
}


//signal(SIGPIPE,SIG_DFL);
static ssize_t http_send(int fd,void *buf,size_t len){
     int nleft=len;
     void *cur_ptr=buf;
     int nsend=0;

     while(nleft>0){
           if((nsend=send(fd,cur_ptr,nleft,0))<0){
                 if(errno==EINTR){
                    nsend=0;
                    continue;
                 }
                 return (-1);

           }else if(nsend==0){
                 break;
           }
           cur_ptr+=nsend;
           nleft-=nsend;
     }
       return (cur_ptr-buf);
}

static ssize_t readline(int fd,void *buf,size_t maxlen){
      size_t read_cnt=0;
      size_t i=0;
      char c,*cur_ptr=buf;
      
      for(;i<maxlen;i++){
         
           if((read_cnt=read(fd,&c,1))==1){//0k
                
                *cur_ptr++=c;
                if(c=='\n'){//line
                     break;
                }

           }else if(read_cnt==0){//eof
                
                 *cur_ptr='\0';
                  break;

           }else {
              if(errno==EINTR){
                  read_cnt=0;
                  continue;
              }
              return (-1);//errno
           }

      }
      *cur_ptr='\0';
      return (i-1);
}


#endif
