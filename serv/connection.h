#ifndef _CONNECTION_H
#define _CONNECTION_H

/**
 * @brief Create UDP socket used to communicate with the clients.
 *
 * @param port_string Port on which the server will be running.
 *
 * @return Socket file descriptor upon success
 * @return -ESYSCALL - Failed system call
 * @return -ENEGPORT - Negative port value
 * @return -EFAILEDCONV - Failed port conversion
 */
int udp_socket(char *port_string);

/**
 * @brief Create TCP socket used to listen for connections.
 *
 * @param port_string Port on whcih the server will be running.
 *
 * @return Socket file descriptor upon success
 * @return -ESYSCALL - Failed system call
 * @return -ENEGPORt - Negative port value
 * @return -EFAILEDCONV - Failed port conversion
 */
int tcp_socket(char *port_string);

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
 * @brief Close connection sockets.
 *
 * @param tcp_sockfd Socket used to listen for connections.
 * @param udp_sockfd Socket used to communicate with UDP clients.
 * @param readfds Set of file descriptors.
 * @param max_fd Value of the highest file descriptor.
 */
void close_connections(int tcp_sockfd, int udp_sockfd,
                       fd_set readfds, int max_fd);

#endif /* _CONNECTION_H */
