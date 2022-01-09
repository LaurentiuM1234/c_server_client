#include <unistd.h>
#include <sys/types.h>
#include <string.h>


#include "server.h"
#include "connection.h"
#include "../util.h"
#include "../list.h"
#include "data.h"
#include "user.h"
#include "../error.h"

static int handle_events(int tcp_sockfd, int udp_sockfd, fd_set *readfds,
                         fd_set tmp_readfds, int *max_fd, int tmp_max_fd,
                         char *cmd_buffer, char *udp_buffer, int *server_is_up,
                         struct node **users)
{
    int err_code;

    for (int i = 0; i <= tmp_max_fd; i++) {
        if (FD_ISSET(i, &tmp_readfds)) {
            if (i == STDIN_FILENO) {
                // got input from stdin
                err_code = read_from_stdin(cmd_buffer, CMD_BUFFER_SIZE);

                if (err_code < 0)
                    return err_code;

                if (strcmp(cmd_buffer, "exit") == 0) {
                    // server got shut down
                    *server_is_up = 0;
                    break;
                }
            } else if (i == tcp_sockfd) {
                err_code = new_tcp_user(tcp_sockfd, readfds, max_fd, users);

                if (err_code < 0)
                    return err_code;

            } else if (i == udp_sockfd) {
                // got input from udp client
                err_code = new_udp_message(udp_sockfd, udp_buffer,
                                           UDP_BUFFER_SIZE, *users);

                if (err_code < 0)
                    return err_code;

            } else {
                // got input from tcp client
                err_code = new_tcp_message(i, cmd_buffer,
                                           CMD_BUFFER_SIZE, *users);

                if (err_code == -ECLOSEDCON) {
                    // user has closed connection
                    close(i);
                    FD_CLR(i, readfds);
                    // silence error
                    err_code = 0;
                }

                if (err_code < 0)
                    return err_code;

            }
        }
    }

    return 0;
}

static int do_logic(int udp_sockfd, int tcp_sockfd)
{
    int err_code;

    // prepare buffers
    char cmd_buffer[CMD_BUFFER_SIZE];
    char udp_buffer[UDP_BUFFER_SIZE];

    // prepare variables for multiplexing
    fd_set readfds, tmp_readfds;
    int max_fd = max_value(udp_sockfd, tcp_sockfd), tmp_max_fd;

    // clean set of descriptors
    FD_ZERO(&readfds);

    // add descriptors to set
    FD_SET(udp_sockfd, &readfds);
    FD_SET(tcp_sockfd, &readfds);
    FD_SET(STDIN_FILENO, &readfds);

    // prepare container
    struct node *users = NULL;

    int server_is_up = 1;

    while (server_is_up) {
        tmp_readfds = readfds;

        tmp_max_fd = max_fd;

        err_code = select(max_fd + 1, &tmp_readfds,
                          NULL, NULL, NULL);

        if (err_code == -1)
            goto err_release_resources;

        err_code = handle_events(tcp_sockfd, udp_sockfd, &readfds, tmp_readfds,
                                 &max_fd, tmp_max_fd, cmd_buffer, udp_buffer,
                                 &server_is_up, &users);

        if (err_code < 0)
            goto err_release_resources;
    }

    close_connections(tcp_sockfd, udp_sockfd, readfds, max_fd);
    free_list(&users, free_user);
    return 0;

err_release_resources:
    free_list(&users, free_user);
    close_connections(tcp_sockfd, udp_sockfd, readfds, max_fd);
    return err_code;
}


int run_server(char *port_string)
{
    int err_code;

    int udp_sockfd = udp_socket(port_string);

    if (udp_sockfd < 0) {
        err_code = udp_sockfd;
        goto err_ret;
    }

    int tcp_sockfd = tcp_socket(port_string);

    if (tcp_sockfd < 0) {
        err_code = tcp_sockfd;
        goto err_close_udp;
    }

    err_code = do_logic(udp_sockfd, tcp_sockfd);

    if (err_code < 0)
        goto err_close_tcp;

    close(tcp_sockfd);
    close(udp_sockfd);
    return 0;

err_close_tcp:
    close(tcp_sockfd);
err_close_udp:
    close(udp_sockfd);
err_ret:
    return err_code;
}