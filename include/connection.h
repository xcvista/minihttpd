
#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include "main.h"

typedef struct user_connection
{
    int fd;
    struct sockaddr_in6 *address;
    socklen_t length;
} user_connection_t;

__BEGIN_DECLS

user_connection_t *user_connection_accept(int socket);
void user_connection_terminate(user_connection_t *conn);

__END_DECLS

#endif
