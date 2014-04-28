/**
 * @file    main.h
 * @author  Maxthon Chan <xcvista@me.com>
 *
 * Header file for global variables and universal headers
 */

#ifndef _MAIN_H_
#define _MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdarg.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/cdefs.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#include "version.h"

__BEGIN_DECLS

/**
 * Keep threads alive flag. Set to @c true upon program starting, @c false upon
 * terminating.
 */
extern bool keep_alive;

extern char *root;

/**
 * Main extry point, declared here.
 */
extern int main(int argc, const char **argv, const char **envp)
    __attribute__((noreturn));

/**
 * Crash here
 */
extern void fail(const char *file, int line, const char *function,
        const char *fmt, ...)
    __attribute__((format(printf, 4, 5), noreturn));
#define FAIL(fmt, ...) fail(__FILE__, __LINE__, __PRETTY_FUNCTION__, \
        fmt, ##__VA_ARGS__)

/**
 * Try to allocate some memory, and crash if failed
 */
extern void *trymalloc(size_t size);

__END_DECLS

#endif
