#ifndef _ERROR_H
#define _ERROR_H

/**
 * @brief Error codes used by the application.
 *
 * @member ESYSCALL - Failed system call.
 * @member ENEGPORT - Negative port value.
 * @member EUSEDID  - Used client id.
 * @member ECLOSEDCONN - Closed connection.
 * @member EUNKCMD - Unknown command.
 * @member EFAILEDCONV - Failed port conversion.
 */
enum err_codes{
	ESYSCALL = 1,
	ENEGPORT = 2,
	EUSEDID = 3,
	ECLOSEDCON = 4,
	EUNKCMD = 5,
	EFAILEDCONV = 6
};

/**
 * @brief Print error message according to given code.
 *
 * @param msg Message to be printed
 * @param err_code The error code
 */
void print_error(const char *msg, int err_code);


#endif
