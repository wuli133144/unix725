
#include "../inc/http_config.h"

typedef struct http_sock_pair
{
      void *ip_addr;
      int port;
} sock_pair;

sock_pair sockpair;
/*@Getaddrconfig@*/
// void Init_sockpair(){
//       //st_sockpair->ip_addr
//       sockpair.ip_addr=Calloc(1,SIZE);
//       file_param(&sockpair);
// }

void file_param(sock_pair *st_sockpair)
{
      FILE *fp = fopen(conf, "rw");
      if (fp == NULL)
      {
            goto err;
      }
      char info[BUFSIZE];
      char ip[BUFSIZE];
      int port, pos;
      zero_to_buffer(ip);
      zero_to_buffer(info);

      while ((fgets(info, BUFSIZE, fp)) != NULL)
      {
            // printf("%s\n",info);
            pos = strspn(info, " ");
            pos = strspn(info + pos, "@");
            strcpy(ip, strtok(info + pos, ":"));
            port = atoi(strtok(NULL, "@"));
            strcpy(st_sockpair->ip_addr, ip);
            st_sockpair->port = port;
      }

      fclose(fp);
      return;
err:
      printf("open failed");
      exit(0);
}

void Init_sockpair()
{
      //st_sockpair->ip_addr
      sockpair.ip_addr = Calloc(1, SIZE);
      file_param(&sockpair);
}
