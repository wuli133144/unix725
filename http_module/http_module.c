#include "http_module.h"

/*
 GET / HTTP/1.1
Host: www.baidu.com
User-Agent: Mozilla/5.0 (Windows; U; Windows NT 5.1; en-US; rv:1.7.6)
Gecko/20050225 Firefox/1.0.1
Connection: Keep-Alive
*/

/*http handler*/
void http_module_handler_request(void *arg)
{

      int clientfd = *((int *)(arg));

      char buf[BUFSIZE];
      char filename[BUFSIZE];
      char method[BUFSIZE];
      char content_type[BUFSIZE];
      zero_to_buffer(buf);
      zero_to_buffer(filename);
      zero_to_buffer(method);
      zero_to_buffer(content_type);
      // if(clientfd<0){unix_error("clientfd error");}

      FILE *read_fp = Fdopen(clientfd, "r");
      FILE *write_fd = Fdopen(dup(clientfd), "w");
      Fgets(buf, BUFSIZE, read_fd);
      if ((strstr(buf, "HTTP/")) == NULL)
      {
            http_module_error(write_fp);
            close(clientfd);
            fclose(read_fp);
            fclose(write_fp);
            return;
            // exit(0);
      }
      SCPY(method, strtok(buf, '/'));//getSCPY(filename, strtok(NULL, '/'));
      SCPY(content_type, Get_file_type(filename));

      if (((strcasecmp(method, "GET") != 0) || (strcasecmp(method, "POST") != 0)))
      {
            http_module_error(write_fp);
            close(clientfd);
            fclose(read_fp);
            fclose(write_fp);
            return;
      }

      fclose(read_fp);

      http_module_handler_response(filename, write_fp, content_type);
}
/*http handler*/

/*
HTTP/1.1 200 OK
Date: Sat, 31 Dec 2005 23:59:59 GMT
Content-Type: text/html;charset=ISO-8859-1
Content-Length: 12
*/

void http_module_handler_response(const char *filename, FILE *fp, const char *filetype)
{
      int clientfd = *((int *)arg);
      char buffer[BUFSIZE];
      int read_cnt = 0;
      char date_tmp[50];
      char date[50];
      int file_len;
      char buffer[BUFSIZE];
      char cont_len[50];

      char Content_Type[BUFSIZE];
      char protocol[] = " HTTP/1.1 200 OK \r\n";

      zero_to_buffer(date);
      zero_to_buffer(date_tmp);
      zero_to_buffer(buffer);
      zero_to_buffer(Content_Type);

      get_current_date(date_tmp, sizeof(date));
      snprintf(date, strlen(date), "Date:%s\r\n", date);
      get_file_length(log_error_file, &len);
      snprintf(cont_len, strlen(cont_len), "Content-Length:%d\r\n", len);
      sprintf(Content_Type, "Content-Type:%s\r\n", filetype);

      fputs(protocol, fp);
      fputs(date, fp);
      fputs(cont_len, fp);
      fputs(content_type, fp);
      fflush(fp);

      if (filename == NULL)
      {
            http_module_error(clientfd);
            return;
      }
      FILE *Sendfp = fopen(filename, "r");
      
      if (Sendfp == NULL)
      {
            http_module_error(clientfd);
      }

      while ((Fgets(buffer, BUFSIZE, Sendfp)) != NULL)
      {

            fputs(buffer, fp);
            fflush(fp);
            // read_cnt=fread(buffer,BUFSIZE,1,Sendfp);
            //fwrite(buffer,strlen(buffer),1,clientfd);
      }
      fflush(fp);
      fclose(Sendfp);
      
}

/*@getfiletype@*/
char *Get_file_type(char *filename)
{
      if (filename == NULL)
      {
            return NULL;
      }
      char ex[BUFSIZE];
      zero_to_buffer(ex);
      SCPY(ex, strtok(filename, '.'));
      return ex;
}

/*@getfiletype end@*/
/*
 HTTP/1.1 400 Bad Request
Server: openresty
Date: Fri, 06 Jan 2017 03:06:53 GMT
Content-Type: text/html
Content-Length: 166
Connection: close
*/

/*@http error page return@*/
void http_module_error(FILE *fp)
{
      //int clientfd=*((int *)arg);
      char date_tmp[50];
      char date[50];
      int file_len;
      char buffer[BUFSIZE];
      char cont_len[50];
      char Connection[50];

      char protocol[] = " HTTP/1.1 400 bad Request \r\n";
      char serve[] = "openresty\r\n";

      zero_to_buffer(buffer);
      zero_to_buffer(date);
      zero_to_buffer(date_tmp);
      zero_to_buffer(cont_len);
      zero_to_buffer(Connection);

      get_current_date(date_tmp, sizeof(date));
      snprintf(date, strlen(date), "Date:%s\r\n", date);
      char Content_Type[] = "Content-Type:txt/html\r\n";
      // SCPY(cont_len,"Content-Length:")

      get_file_length(log_error_file, &len);
      snprintf(cont_len, strlen(cont_len), "Content-Length:%d\r\n", len);
      snprintf(Connection, strlen(Connection), "Connection: close\r\n");

      fputs(protocol, fp);
      fputs(serve, fp);
      fputs(date, fp);
      fputs(Content_Type, fp);
      fputs(cont_len, fp);
      fputs(Connection, fp);

      FILE *fp_error = fopen(log_error_file, "r");
      if (fp_error == NULL)
      {
            unix_error("fopen error!");
      }

      //    while((!feof(fp_error))&&(Fgets(fp_error,buffer,BUFSIZE))!=NULL){
      //            int len=strlen(buffer);
      //            fputs(buffer,fp);
      //            fseek(fp_error,0,SEEK_CUR);
      //            //fflush(fp);
      //    }
      while (!feof(fp_error))
      {
            if ((read_cnt = fread(buffer, BUFSIZE, 1, fp_error)) > 1)
            {
                  unix_error("fread error!");
            }
            else if (read_cnt == 1)
            { //eof
                  fwrite(buffer, strlen(buffer), 1, fp);
            }
      }
      fflush(fp);
      fclose(fp_error);
}
/*@http error page return end@*/