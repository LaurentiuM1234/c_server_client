#include <stdlib.h>
#include <stdio.h>

#include "../error.h"
#include "server.h"



int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    int err_code = run_server(argv[1]);

    if (err_code < 0) {
        print_error("run_server", err_code);
        exit(EXIT_FAILURE);
    }

    return 0;
}
