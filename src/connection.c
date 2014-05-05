
#include "connection.h"

struct user_connection
{
    // Socket information
    int fd;
    struct sockaddr_in6 *address;
    socklen_t length;

    // Thread information
    pthread_attr_t thread_attr;
    pthread_t thread;
};

user_connection_t *user_connection_accept(int socket)
{
    // Allocate memory for the new connection object
    user_connection_t *conn = trymalloc(sizeof(user_connection_t));
    memset(conn, 0, sizeof(user_connection_t));
    
    // Prepare for accept(2)
    socklen_t length = sizeof(struct sockaddr_in6);
    struct sockaddr_in6 *addr = trymalloc(length);

    // accept(2)
    int fd = accept(socket, (struct sockaddr *)addr, &length);
    if (fd < 0)
    {
        free(addr);
        free(conn);
        return NULL;
    }

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

void user_connection_terminate(user_connection_t *conn);

void *user_connection_main(user_connection_t *conn)
{
    int readfd = dup(conn->fd);
    if (readfd < 0)
        FAIL(NULL);

    FILE *infile = fdopen(readfd, "r+b");
    
    char *line = trymalloc(BUFSIZ);
    int state = 0; // 0 = reading URI header, 1 = reading header fields, 2 = done reading.
    
    char method[10];
    char *uri = trymalloc(BUFSIZ);
    char protver[20];

    //char *key = trymalloc(BUFSIZ);
    //char *value = trymalloc(BUFSIZ);

    while (fgets(line, BUFSIZ, infile) > 0 && state < 2)
    {
        fprintf(stderr, "%s", line);
        switch (state)
        {
            case 0:
                sscanf("%s %s %s", method, uri, protver);
                state = 1;
                break;
            case 1:
                if (!strcmp(line, "\n") || !strcmp(line, "\r\n"))
                {
                    state = 2;
                    break;
                }
                //char *cp = strchr(line, ':');
                // ...
                break;
            default:
                FAIL("What is this state?!");
        }
    }

    // Handle the path, commericial software style.
    //int level = 0;
    char *path = trymalloc(BUFSIZ);
    strcpy(path, root);

    /*
    char *cp = path + strlen(path);
    if (*(cp - 1) == '/')
    {
        cp--;
        *cp = 0;
    }

    char *up = uri;
    do
    {
        char *up2 = strchr(up + 1, '/');
        if (!up2)
        {
            strcpy(cp, up);
        }
        else if (up2 - up > 1)
        {
            char *l = up + 1;
            int len = up2 - up - 1;
            if (len == 1 && *l == '.')
            {
                up = up2;
                continue;
            }
            else if (len == 2 && !strncmp(l, "..", 2))
            {
                if (!level)
                {
                    up = up2;
                    continue;
                }
                else
                {
                    level--;
                    cp = strrchr(path, '/');
                    *cp = 0;
                }
            }
            else
            {
                *cp = '/';
                memcpy(++cp, l, len);
                cp += len;
                *cp = 0;
                level++;
            }
        }
        else
        {
            up++;
            continue;
        }
    } while (*up);
    */

    strcat(path, uri);

    fprintf(stderr, "method: %s uri: %s path: %s\n", method, uri, path);

    user_connection_terminate(conn);
    return NULL;
}

void user_connection_detach(user_connection_t *conn)
{
    if ((errno = pthread_attr_init(&conn->thread_attr)))
        FAIL(NULL);

    if ((errno = pthread_attr_setdetachstate(&conn->thread_attr, PTHREAD_CREATE_DETACHED)))
        FAIL(NULL);

    if ((errno = pthread_create(&conn->thread, &conn->thread_attr, (void *(*)(void *))user_connection_main, conn)))
        FAIL(NULL);
}

void user_connection_terminate(user_connection_t *conn)
{
    close(conn->fd);
    pthread_attr_destroy(&conn->thread_attr);
    free(conn->address);
    free(conn);

    pthread_exit(NULL);
}
