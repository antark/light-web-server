#ifndef _SOCK_H_
#define _SOCK_H_

#include "ws.h"

int open_clientfd(char *hostname, int port);
int open_listenfd(int port);

#endif
