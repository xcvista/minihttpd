/**
 * @file    main.c
 * @author  Maxthon Chan <xcvista@me.com>
 *
 * Main entry point of minihttpd.
 */

#include "main.h"

#include <getopt.h>

bool keep_alive;

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

int main(int argc, const char **argv, const char **envp)
{
    char *root = malloc(1024);
    int port = 0;
    int opt;

    if (!root)
        FAIL(NULL);

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

    if (getuid() && port < 1024)
        FAIL("Cannot open port %d with user %s.\n", port, getenv("USER"));
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
        "USAGE: minihttpd [-p <port>] -r <root>\n"
        "\n"
        "-p, --port: listening port. Default to 80 (root) or 8080 (non-root)\n"
        "-r, --root: web root.\n"
        "\n"
        );
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

