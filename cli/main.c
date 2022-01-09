#include <stdlib.h>
#include <stdio.h>

#include "../error.h"
#include "client.h"



int main(int argc, char *argv[])
{
    setvbuf(stdout, NULL, _IONBF, BUFSIZ);

    int err_code = run_client(argv[1], argv[2], argv[3]);

    if (err_code < 0) {
        print_error("run_client", err_code);
        exit(EXIT_FAILURE);
    }

    return 0;
}
