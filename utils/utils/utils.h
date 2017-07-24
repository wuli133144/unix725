#ifndef __UTILS__H
#define __UTILS__H

#include "../inc/unix_error.h"
#include "../inc/http_config.h"

#define SCPY(a, b) strcpy(a, b)
#define MMCP(a, b) memcpy(a, b, siseof(b))

#define BUFSIZE 1024
#define zero_to_buffer(buf) bzero(buf, sizeof(buf))



#define error_msg(fmt, ...) \
    fprintf(stderr,"[%s:%s:%d][error %d:%s]" fmt "\r\n",\
         __FILE__, __func__, __LINE__, errno, strerror(errno),##__VA_ARGS__)


FILE *Fdopen(int fd, const char *mode)
{
    FILE *rval = NULL;
    if ((rval = fdopen(fd, mode)) == NULL)
    {
        unix_error("fdopen");
    }
    return rval;
}

char *Fgets(char *buf, int size, FILE *stream)
{
    char *pret = NULL;
    if ((pret = fgets(buf, size, stream)) == NULL)
    {
        unix_error("fgets error");
    }
    return pret;
}

int get_current_time(char *buf, int size)
{

    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    return strftime(buf, size, "%Y-%m-%d %H:%M:%S", timenow);
}

/*@get file length@*/
void get_file_length(const char *path, int *len)
{

    int fd = open(path, O_RDONLY, S_IRUSR | S_IROTH | S_IRGRP);
    if (fd < 0)
    {
        goto err;
    }
    struct stat st;
    int rval = fstat(fd, &st);
    if (rval < 0)
    {
        goto err_fstat;
    }
    *len = st.st_size;

    close(fd);
// memcpy((void *)len,(void *)&st.st_size,sizeof(st.st_size));
err:
    unix_error("open error");
err_fstat:
    unix_error("fstat error");
}
/*@get file length@*/

/*@Malloc operation start@*/

void *Malloc(size_t length)
{

    void *ptr = NULL;
    if ((ptr = malloc(length)) == NULL)
    {
        unix_error("malloc error");
    }
    return ptr;
}

/*@Malloc operation end@*/

/*@Realloc  memory@*/

void *Realloc(void *base, size_t sz)
{
    void *ptr = NULL;
    if ((ptr = realloc(base, sz)) == NULL)
    {
        free(base);
        base = NULL;
    }
    return ptr;
}
/*@Realloc memory@*/
/*@calloc @*/
void *Calloc(unsigned int n, ssize_t length)
{
    void *rptr = NULL;
    if ((rptr = calloc(n, length)) == NULL)
    {
        unix_error("calloc error");
    }
    return rptr;
}
/*@calloc end@*/

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





#endif