#ifndef _CLIENT_H
#define _CLIENT_H

/**
 * @brief Core function used to start a client.
 *
 * @param id String used to identify client when talking to server.
 * @param addr Address of the server the client will connect to.
 * @param port_string Port of the server the client will connect to.
 *
 * @returns 0 - Function executed successfully
 * @returns -ESYSCALL - Failed system call
 * @returns -ENEGPORT - Negative port value
 * @returns -ECLOSEDCON - Server closed connection
 * @returns -EFAILEDCONV - Failed port conversion
 */
int run_client(char *id, char *addr, char *port_string);

#endif /* _CLIENT_H */