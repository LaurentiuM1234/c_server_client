#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>


#include "util.h"
#include "error.h"

int to_port(char *string)
{
    int port = (int)strtol(string, NULL, 10);

    if (errno == ERANGE)
        return -EFAILEDCONV;

    if (port < 0)
        return -ENEGPORT;

    return port;
}

int max_value(int v1, int v2)
{
    return v1 >= v2 ? v1 : v2;
}


int read_from_stdin(char *buffer, int buffer_size)
{
    // first clean the buffer
    memset(buffer, 0, buffer_size);

    // read from stdin
    char *res = fgets(buffer, buffer_size, stdin);

    if (res == NULL)
        return -ESYSCALL;

    // remove tracing newline
    buffer[strlen(buffer) - 1] = 0;

    return 0;
}

static char **alloc_arg_vector()
{
    char **argv = calloc(MAX_ARGC, sizeof(char*));

    for (int i = 0; i < MAX_ARGC; i++) {
        argv[i] = calloc(CMD_BUFFER_SIZE, sizeof(char));

        if (!argv[i]) {
            free(argv);
            return NULL;
        }
    }
    return argv;
}

char **build_argv(char *buffer, int *argc)
{
    char **argv = alloc_arg_vector();

    if (argv == NULL)
        return NULL;

    *argc = 0;

    char *c_ptr = strtok(buffer, CMD_DELIM);

    while (c_ptr != NULL) {
        sprintf(argv[*argc], "%s", c_ptr);
        (*argc)++;
        c_ptr = strtok(NULL, CMD_DELIM);
    }

    return argv;
}

void free_argv(char **argv)
{
    for (int i = 0; i < MAX_ARGC; i++)
        free(argv[i]);

    free(argv);
}