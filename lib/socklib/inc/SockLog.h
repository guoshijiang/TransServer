#ifndef _SOCKETLOG_H_
#define _SOCKETLOG_H_

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

extern int  SocketLevel[5];
void Socket_Log(const char *file, int line, int level, int status, const char *fmt, ...);

#endif
