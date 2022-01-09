#ifndef _SERVER_H
#define _SERVER_H

/**
 * @brief Core function used to start the server.
 *
 * @param port_string Port on which the server will be running
 *
 * @returns 0 - Function executed successfully
 * @returns -ESYSCALL - Failed system call
 * @returns -ENEGPORt - Negative port value
 * @returns -ECLOSEDCONN - Client closed connection
 * @returns -EFAILEDCONV - Failed port conversion
 */
int run_server(char *port_string);

#endif /* _SERVER_H */