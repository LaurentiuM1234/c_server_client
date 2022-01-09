#ifndef _DATA_H
#define _DATA_H

/**
 * @brief Process command given by the user.
 *
 * @param cmd_buffer Buffer holding the contents of the command.
 * @param buffer_size Size of the command buffer.
 * @param client_is_up Pointer to client state variable.
 * @param tcp_sockfd Connection socket used to send data to server.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 * @return -EUNKCMD - Unknown command
 * @return -EFAILEDCONV - Failed port conversion
 */
int process_command(char *cmd_buffer, int buffer_size, int *client_is_up,
                    int tcp_sockfd);

/**
 * @brief Process message received from the server.
 *
 * @param buffer Buffer holding the contents of the message.
 */
void process_message(const char *buffer);


#endif /* _DATA_H */