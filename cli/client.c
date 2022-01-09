#include <unistd.h>
#include <sys/types.h>

#include "client.h"
#include "connection.h"
#include "../util.h"
#include "../error.h"
#include "data.h"
#include "../message.h"

static int handle_events(int tcp_sockfd, fd_set readfds, int max_fd,
                         char *cmd_buffer, char *tcp_buffer,
                         int *client_is_up)
{
    int err_code;

    for (int i = 0; i <= max_fd && *client_is_up; i++) {
        if (FD_ISSET(i, &readfds)) {
            if (i == STDIN_FILENO) {
                // read from stdin
                err_code = read_from_stdin(cmd_buffer, CMD_BUFFER_SIZE);

                if (err_code < 0)
                    return err_code;

                err_code = process_command(cmd_buffer, CMD_BUFFER_SIZE,
                                           client_is_up, tcp_sockfd);

                // silence unknown command error
                if (err_code < 0 && err_code != -EUNKCMD)
                    return err_code;

            } else if (i == tcp_sockfd) {
                // got input from server
                err_code = tcp_recv(tcp_sockfd, tcp_buffer,
                                    sizeof(struct message));

                if (err_code < 0) {
                    return err_code;
                }

                if (err_code == 0)
                    return -ECLOSEDCON;

                // process input from server
                process_message(tcp_buffer);
            }
        }
    }
    return 0;
}


static int do_logic(int tcp_sockfd)
{
    int err_code;

    // prepare buffers
    char cmd_buffer[CMD_BUFFER_SIZE];
    int tcp_buffer_size = sizeof(struct message);
    char tcp_buffer[tcp_buffer_size];

    // prepare variables for multiplexing
    fd_set readfds, tmp_readfds;
    int max_fd = tcp_sockfd;

    // clean set of descriptors
    FD_ZERO(&readfds);

    // add descriptors to set
    FD_SET(tcp_sockfd, &readfds);
    FD_SET(STDIN_FILENO, &readfds);

    int client_is_up = 1;

    while (client_is_up) {
        tmp_readfds = readfds;

        err_code = select(max_fd + 1, &tmp_readfds,
                          NULL, NULL, NULL);

        if (err_code == -1)
            return -ESYSCALL;

        err_code = handle_events(tcp_sockfd, tmp_readfds, max_fd,
                                 cmd_buffer, tcp_buffer, &client_is_up);

        if (err_code < 0)
            return err_code;
    }

    return 0;
}


int run_client(char *id, char *addr, char *port_string)
{
    int err_code;

    int tcp_sockfd = tcp_socket(id, addr, port_string);

    if (tcp_sockfd < 0) {
        err_code = tcp_sockfd;
        return err_code;
    }

    err_code = do_logic(tcp_sockfd);

    if (err_code < 0) {
        close(tcp_sockfd);
        return err_code;
    }

    close(tcp_sockfd);
    return 0;
}