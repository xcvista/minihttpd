/**
 * @file    main.c
 * @author  Maxthon Chan <xcvista@me.com>
 *
 * Main entry point of minihttpd.
 */

#include "main.h"
#include "connection.h"

#include <getopt.h>

bool keep_alive;
char *root;
int listen_socket;

// Help message functions.

void copyright(void);
void usage(void);
void version(void);

// getopt_long(3) option array
static struct option options_long[] = {
//  { name,         has_arg,            flag,   val },
    { "root",       required_argument,  NULL,   'r' },
    { "port",       required_argument,  NULL,   'p' },
    { "help",       no_argument,        NULL,   'h' },
    { "version",    no_argument,        NULL,   'v' },
    { NULL,         0,                  NULL,   0   }
};

void sigint(int arg)
{
    keep_alive = false;
    close(listen_socket);
    const char *signame = strsignal(arg);
    if (signame)
        fprintf(stderr, "\rminihttpd: exiting with signal %s.\n", signame);
    else
        fprintf(stderr, "\rminihttpd: exiting with signal %d.\n", arg);
    exit(EXIT_SUCCESS);
}

int main(int argc, const char **argv, const char **envp)
{
    int port = 0;
    int opt = 0;

    // Parse the command line.
    while ((opt = getopt_long(argc, (char *const *)argv, 
                    "p:r:hHvV", options_long, NULL)) != -1)
    {
        switch (opt)
        {
            case 'h':
            case 'H':
                version();
                usage();
                copyright();
                exit(EXIT_SUCCESS);
                break;
            case 'v':
            case 'V':
                version();
                copyright();
                exit(EXIT_SUCCESS);
                break;
            case 'r':
                root = trymalloc(strlen(optarg) + 1);
                strncpy(root, optarg, 1024);
                break;
            case 'p':
                port = atoi(optarg);
                break;
            default:
                version();
                usage();
                copyright();
                exit(EXIT_FAILURE);
                break;
        }
    }

    // Validate the arguments

    if (port <= 0)
        port = getuid() ? 8080 : 80;

    if (!root || !strlen(root))
    {
        if (root)
            free(root);
        root = trymalloc(strlen(getenv("HOME")) + 1);
        strcpy(root, getenv("HOME"));
    }

    if (getuid() && port < 1024)
        FAIL("Cannot open port %d with user %s.\n", port, getenv("USER"));

    fprintf(stderr, "minihttpd: opening port %d serving path %s\n", port, root);

    // Catch signals
    signal(SIGHUP, SIG_IGN);
    signal(SIGINT, sigint);
    signal(SIGTERM, sigint);

    // Open the socket
    if ((listen_socket = socket(AF_INET6, SOCK_STREAM, 0)) < 0)
        FAIL(NULL);

    // Find the destinetion address
    struct sockaddr_in6 server_addr;
    memset(&server_addr, 0, sizeof(struct sockaddr_in6));
    server_addr.sin6_family = AF_INET6;
    server_addr.sin6_addr = in6addr_any;
    server_addr.sin6_port = htons((uint16_t)port);

    // Allow fast rebinding
    int t = 1;
    if (setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR, &t, sizeof(t)) < 0)
        FAIL(NULL);

    // Bind the socket to IPv6 [::]:port
    if (bind(listen_socket, (struct sockaddr *)&server_addr, sizeof(struct sockaddr_in6)) < 0)
        FAIL(NULL);

    // Start the socket listening
    if (listen(listen_socket, 5) < 0)
        FAIL(NULL);

    // Accept connections
    keep_alive = true;
    while (keep_alive)
    {
        user_connection_t *conn = user_connection_accept(listen_socket);
        user_connection_detach(conn);
    }

    // Clean up
    close(listen_socket);
}

void copyright(void)
{
    fprintf(stderr, "This is free software, licensed under 3-clause BSD license.\n");
}

void version(void)
{
    fprintf(stderr,
        "minihttpd %d.%d (%s)\n",
        MINIHTTPD_MAJOR,
        MINIHTTPD_MINOR,
        MINIHTTPD_BUILD
        );
    fprintf(stderr,
        "Minimal HTTP/1.0 Server\n"
        "Copyright (c) 2014 Maxthon Chan. All rights reserved.\n"
        "\n"
        );
}

void usage(void)
{
    fprintf(stderr,
        "USAGE: minihttpd [-p <port>] [-r <root>] [-hv]\n"
        "\n"
        "-p, --port:     listening port. Default to 80 (root) or 8080 (non-root)\n"
        "-r, --root:     web root directory. Default to $HOME\n"
        "-h, --help:     print this help information and exit\n"
        "-v, --version:  print server version and exit\n"
        "\n"
        );
}

void *trymalloc(size_t size)
{
    void *buf = malloc(size);
    if (!buf)
        FAIL(NULL);
    memset(buf, 0, size);
    return buf;
}

void fail(const char *file, int line, const char *function, const char *fmt, ...)
{
    int e = errno;
    if (e)
        fprintf(stderr, "CRASH: %s:%d %s: %s\n",
                file, line, function, strerror(e));
    else
        fprintf(stderr, "CRASH: %s:%d %s\n",
                file, line, function);

    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        vfprintf(stderr, fmt, args);
        va_end(args);
    }
    exit(EXIT_FAILURE);
}

