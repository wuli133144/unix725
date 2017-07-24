

#include "epoll_circle.h"

int main(int argc, char **argv)
{
    Init_sockpair();
    return start(argc, argv);
}