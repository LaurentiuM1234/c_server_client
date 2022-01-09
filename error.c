#include <stdio.h>

#include "error.h"




void print_error(const char *msg, int err_code)
{
	if (err_code == -ESYSCALL)
		perror(msg);
	else if (err_code == -ENEGPORT)
	    fprintf(stderr, "%s: Negative port value.\n", msg);
	else if (err_code == -ECLOSEDCON)
	    fprintf(stderr, "%s: Closed connection.\n", msg);
}
