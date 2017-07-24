

#include<stdio.h>
#include<string.h>
#include<unistd.h>


#include<dirent.h>

#include<stdlib.h>
#include<sys/types.h> 

#include"../../inc/unix_error.h"

#define BUFSIZE  1024

#define zero_to_buffer(buf)    bzero((void *)buf,sizeof(buf))

#define test_out(x)        fprintf(stdout,"buf=%s\n",x)
  
#define test_error_null(x)                        \
        if((x)==(void *)0){                       \
            unix_error(#x"==NULL error!");        \
        }


int isfiletype_ok(char *filename){
    //  if()
}

int isfile_ok(char *filename,char *req_file){
   
      test_error_null(filename);
      
      char *newfilename=strdup(filename);
      int pos=strspn(newfilename," ");
      newfilename+=pos;
     

      char *res=strtok(newfilename,"/");
      char *oldres=res;

     while((res=strtok(NULL,"/"))!=NULL){
              //test_out(res);
             if(strstr(oldres,".")!=NULL){
                 free(newfilename);
                 newfilename=NULL;
                 return -1;
             }
             oldres=res;
             
     }
    
    //test_out(oldres);
    if(filename[strlen(filename)-1]=='/'){
          strcpy(req_file,"index.html");
    }else{
        strcpy(req_file,oldres);
    }

    free(newfilename);
    newfilename=NULL;
     return 0;

       
}  




void http_param(int clientfd,char *req_file){
      
     // test_error_null(fp)
   
      char buf[BUFSIZE];
      char filename[BUFSIZE];
      char method[BUFSIZE];
      char content_type[BUFSIZE];
      int res=0;
      zero_to_buffer(buf);
      zero_to_buffer(filename);
      zero_to_buffer(method);
      zero_to_buffer(content_type);
      // if(clientfd<0){unix_error("clientfd error");}

      FILE *read_fp = fdopen(clientfd, "r");
      
      FILE *write_fp = fdopen(dup(clientfd), "w");
      fgets(buf, BUFSIZE, read_fp);

      if ((strstr(buf, "HTTP/")) == NULL)
      {
            // http_module_error(write_fp);
            printf("error method\n");
            close(clientfd);
            fclose(read_fp);
            fclose(write_fp);
            return;
      }
      
      //test_out(buf);
      strcpy(method,strtok(buf," "));
      //test_out(method);//success
      strcpy(filename,strtok(NULL," "));
      res=isfile_ok(filename,req_file);
      if(res<0){
          unix_error("filepath error!");
      }
      if(filename[strlen(filename)-1]=='/'){
           strcat(filename,"index.html");
       }

}

// void getfilename(int fd,const char *filename){
//       char filename[BUFSIZE];
//       zero_to_buffer(filename);
   
//      // if(clientfd<0){unix_error("clientfd error");}
// }



int getfile_type(const char *file)

#include<fcntl.h>

int main(int argc,char **argv){
   

   char req_file[BUFSIZE];
   char buf[BUFSIZE];
   int read_cnt=0;
   zero_to_buffer(buf);
   zero_to_buffer(req_file);

   FILE* fp=fopen("param","r");
   test_error_null(fp)
   int fd=fileno(fp);

   if(fd==0){
       unix_error("fdopen error");
   }
    
    http_param(fd,req_file);

    int res=chdir("/home/www/asserts/video/");
    DIR *dir=opendir("./");

        //test_out(req_file);
        int fdreq=open(req_file,O_RDONLY);
        if(fdreq<0){
            unix_error("open error");
        }
     
    // setvbuf(stdout,buf,_IONBF,BUFSIZE);
    // read(fdreq,buf,BUFSIZE);te
    // fputs(buf,stdout);
             
     while((read_cnt=read(fdreq,buf,BUFSIZE))!=0){
                      fputs(buf,stdout);
                      fflush(stdout);
     }
     

   close(fdreq);
    
    return 0;
}
