
#include "connection.h"

user_connection_t *user_connection_accept(int socket)
{
    user_connection_t *conn = trymalloc(sizeof(user_connection_t));
    memset(conn, 0, sizeof(user_connection_t));
    
    // Prepare for accept(2)
    socklen_t length = sizeof(struct sockaddr_in6);
    struct sockaddr_in6 *addr = trymalloc(length);
    int fd = accept(socket, (struct sockaddr *)addr, &length);
    if (fd < 0)
        FAIL(NULL);

    char *name = malloc(INET6_ADDRSTRLEN);
    if (name)
    {
        inet_ntop(addr->sin6_family, &addr->sin6_addr, name, INET6_ADDRSTRLEN);
        fprintf(stderr, "minihttpd: incoming from [%s]:%d\n", name, ntohs(addr->sin6_port));
        free(name);
    }
    conn->fd = fd;
    conn->address = addr;
    conn->length = length;

    return conn;
}

void user_connection_terminate(user_connection_t *conn)
{
    close(conn->fd);
    free(conn->address);
    free(conn);
}
