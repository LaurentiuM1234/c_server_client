#ifndef _CONNECTION_H
#define _CONNECTION_H

/**
 * @brief Create TCP socket used to communicate with the server.
 *
 * @param id String used to identify client when talking to server.
 * @param addr Address of the server the client will connect to.
 * @param port_string Port of the server the client will connect to.
 *
 *
 * @return Socket file descriptor upon success
 * @return -ESYSCALL - Failed system call
 * @return -ENEGPORT - Negative port value
 * @return -ECLOSEDCON - Server closed connection
 * @return -EFAILEDCONV - Failed port conversion
 */
int tcp_socket(char *id, char *addr, char *port_string);

/**
 * @brief Send data using TCP.
 *
 * @param tcp_sockfd Connection socket used to send data to server.
 * @param buffer Contents which will be sent to the server.
 * @param expected_bcount Total amount of bytes expected to be sent.
 *
 * @return Number of bytes sent upon success
 * @return -ESYSCALL - Failed system call
 */
int tcp_send(int tcp_sockfd, char *buffer, int expected_bcount);

/**
 * @brief Receive data using TCP.
 *
 * @param tcp_sockfd Connection socket used to receive data from server.
 * @param buffer Buffer in which received data will be stored.
 * @param expected_bcount Total amount of bytes expected to be received.
 *
 * @return Number of bytes received upon success
 * @return 0 - Server closed the connection
 * @return -ESYSCALL - Failed system call
 */
int tcp_recv(int tcp_sockfd, char *buffer, int expected_bcount);

#endif /* _CONNECTION_H */