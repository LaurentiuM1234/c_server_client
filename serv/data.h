#ifndef _DATA_H
#define _DATA_H

#include <sys/types.h>

#include "../list.h"

/**
 * @brief Handle new TCP client.
 *
 * @param tcp_sockfd Socket used to listen for connections.
 * @param readfds Set of file descriptors.
 * @param maxfd Value of the highest file descriptor.
 * @param users List of all users.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 * @return -ECLOSEDCONN - Client closed connection
 */
int new_tcp_user(int tcp_sockfd, fd_set *readfds, int *maxfd,
                 struct node **users);

/**
 * @brief Handle new message from UDP client.
 *
 * @param udp_sockfd Socket used to receive data from UDP clients.
 * @param udp_buffer Buffer used to hold message received from UDP clients.
 * @param buffer_size Size of the buffer
 * @param users List of all users.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 * @return -ECLOSEDCONN - Client closed connection
 */
int new_udp_message(int udp_sockfd, char *udp_buffer, int buffer_size,
                    struct node *users);

/**
 * @brief Handle new message from TCP client.
 *
 * @param tcp_sockfd Socket used to receive data from TCP clients.
 * @param cmd_buffer Buffer used to hold command received from TCP clients.
 * @param buffer_size Size of the buffer
 * @param users List of all users.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 * @return -ECLOSEDCONN - Client closed connection
 */
int new_tcp_message(int tcp_sockfd, char *cmd_buffer, int buffer_size, struct node *users);
void mark_as_inactive(int sockfd, struct node *users);

#endif /* _DATA_H */
