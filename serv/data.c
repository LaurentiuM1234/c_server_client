#include <netinet/in.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>


#include "data.h"
#include "../error.h"
#include "user.h"
#include "connection.h"

int new_tcp_user(int tcp_sockfd, fd_set *readfds, int *maxfd,
                 struct node **users)
{
    int err_code;

    struct sockaddr_in usr_info;
    socklen_t len = sizeof(usr_info);

    int connection_fd = accept(tcp_sockfd, (struct sockaddr*)&usr_info,
            &len);

    if (connection_fd == -1) {
        err_code = -ESYSCALL;
        goto err_ret;
    }

    // prepare buffer for id
    char id[ID_SIZE];
    memset(id, 0, ID_SIZE);

    // receive user id (users send it immediately after connection)
    err_code = tcp_recv(connection_fd, id, ID_SIZE);

    if (err_code < 0)
        goto err_close_socket;

    // user is not allowed to close connection before sending id
    if (err_code == 0) {
        err_code = -ECLOSEDCON;
        goto err_close_socket;
    }

    err_code = add_user_to_list(users, id, usr_info.sin_addr, connection_fd);

    if (err_code == -EUSEDID) {
        // print message and silence error
        printf("Client %s already connected.\n", id);
        // close connection socket
        close(connection_fd);
        err_code = 0;
        goto err_ret;
    }

    if (err_code < 0)
        goto err_close_socket;

    // notify user connection
    printf("New client %s connected from %s:%d.\n", id,
           inet_ntoa(usr_info.sin_addr), ntohs(usr_info.sin_port));

    // add connection fd to set
    FD_SET(connection_fd, readfds);

    // update max descriptor value if need be
    if (connection_fd > *maxfd)
        *maxfd = connection_fd;

    return 0;

err_close_socket:
    close(connection_fd);
err_ret:
    return err_code;
}

int new_udp_message(int udp_sockfd, char *udp_buffer, int buffer_size,
                    struct node *users)
{
    int err_code;

    // first clean the buffer
    memset(udp_buffer, 0, buffer_size);

    struct sockaddr_in sender_info;
    socklen_t len = sizeof(sender_info);

    err_code = recvfrom(udp_sockfd, udp_buffer, buffer_size,
                        0, (struct sockaddr*)&sender_info, &len);

    if (err_code == -1)
        return -ESYSCALL;

    err_code = send_message(udp_buffer, buffer_size, sender_info,
                            users);

    if (err_code < 0)
        return err_code;

    return 0;
}

static int process_tcp_message(char *cmd_buffer, int buffer_size,
                               int tcp_sockfd, struct node *users)
{
    int err_code = 0;

    // make dummy cmd buffer for arg vector construction
    char dummy[buffer_size];
    memcpy(dummy, cmd_buffer, buffer_size);

    // build argument vector
    int argc = 0;
    char **argv = build_argv(dummy, &argc);

    if (argv == NULL) {
        err_code = -ESYSCALL;
        goto err_ret;
    }

    if (strcmp(argv[0], "subscribe") == 0) {
        if (argc != 3) {
            // wrong parameter count, unknown command
            err_code = -EUNKCMD;
            goto err_free_argv;

        }

        int enable_sf = to_port(argv[2]);

        if (enable_sf < 0)
            goto err_free_argv;

        if (enable_sf > 1) {
            err_code = -EUNKCMD;
            goto err_free_argv;
        }

        err_code = add_topic(tcp_sockfd, users, argv[1], enable_sf);

        if (err_code < 0)
            goto err_free_argv;

    } else if (strcmp(argv[0], "unsubscribe") == 0) {
        if (argc != 2) {
            // wrong parameter count, wrong command
            err_code = -EUNKCMD;
            goto err_free_argv;
        }

        remove_topic(tcp_sockfd, users, argv[1]);
    } else {
        err_code = -EUNKCMD;
        goto err_free_argv;
    }

    free_argv(argv);
    return 0;

err_free_argv:
    free_argv(argv);
err_ret:
    return err_code;
}


int new_tcp_message(int tcp_sockfd, char *cmd_buffer, int buffer_size,
                    struct node *users)
{
    int err_code;

    // prepare command buffer
    memset(cmd_buffer, 0, buffer_size);

    err_code = tcp_recv(tcp_sockfd, cmd_buffer, buffer_size);

    if (err_code < 0)
        return err_code;

    if (err_code == 0) {
        mark_as_inactive(tcp_sockfd, users);
        return -ECLOSEDCON;
    }

    err_code = process_tcp_message(cmd_buffer, buffer_size, tcp_sockfd,
                                   users);

    // silence unknown command error
    if (err_code < 0 && err_code != -EUNKCMD)
        return err_code;

    return 0;
}
