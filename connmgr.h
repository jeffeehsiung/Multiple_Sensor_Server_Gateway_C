#ifndef _CONNMGR_H_
#define _CONNMGR_H_

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <ctype.h>
#include <stdint.h>
#include <aio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include "config.h"
#include "lib/tcpsock.h"

void* client_handler (void* param);
void connmgr_start(int server_port);

#endif /* _CONNMGR_H_ */