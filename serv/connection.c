#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/socket.h>
#include <unistd.h>


#include "connection.h"
#include "../error.h"
#include "../util.h"

#define MAX_TCP_CLIENTS 10

static int bind_socket(int sockfd, int port)
{
    struct sockaddr_in sock_info;

    sock_info.sin_port = htons(port);
    sock_info.sin_addr.s_addr = INADDR_ANY;
    sock_info.sin_family = AF_INET;

    return bind(sockfd, (const struct sockaddr*)&sock_info,
            sizeof(sock_info));
}


int udp_socket(char *port_string)
{
    int err_code;

    int port = to_port(port_string);

    if (port < 0) {
        err_code = port;
        goto err_ret;
    }

    // open socket for udp
    int sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd == -1) {
        err_code = -ESYSCALL;
        goto err_ret;
    }

    err_code = bind_socket(sockfd, port);

    if (err_code < 0)
        goto err_close_socket;

    return sockfd;

err_close_socket:
    close(sockfd);
err_ret:
    return err_code;
}

int tcp_socket(char *port_string)
{
    int err_code;

    int port = to_port(port_string);

    if (port < 0) {
        err_code = port;
        goto err_ret;
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd == -1) {
        err_code = -ESYSCALL;
        goto err_ret;
    }

    // disable Neagle's algorithm
    int value = 1;
    err_code = setsockopt(sockfd, IPPROTO_TCP, TCP_NODELAY,
                          &value, sizeof(int));

    if (err_code == -1)
        goto err_close_socket;

    err_code = bind_socket(sockfd, port);

    if (err_code < 0)
        goto err_close_socket;

    err_code = listen(sockfd, MAX_TCP_CLIENTS);

    if (err_code == -1)
        goto err_close_socket;

    return sockfd;

err_close_socket:
    close(sockfd);
err_ret:
    return err_code;
}

int tcp_recv(int tcp_sockfd, char *buffer, int expected_bcount)
{
    int err_code;
    int bytes_received = 0;

    do {
        err_code = recv(tcp_sockfd, buffer + bytes_received,
                        expected_bcount - bytes_received, 0);

        if (err_code == -1 || err_code == 0)
            return err_code;

        bytes_received += err_code;
    } while (bytes_received < expected_bcount);

    return bytes_received;
}

int tcp_send(int tcp_sockfd, char *buffer, int expected_bcount)
{
    int err_code;
    int bytes_sent = 0;

    do {

        err_code = send(tcp_sockfd, buffer + bytes_sent,
                        expected_bcount - bytes_sent, 0);

        if (err_code == -1)
            return err_code;

        bytes_sent += err_code;
    } while (bytes_sent < expected_bcount);

    return bytes_sent;
}

void close_connections(int tcp_sockfd, int udp_sockfd,
                       fd_set readfds, int max_fd)
{
    for (int i = 0; i <= max_fd; i++) {
        if (FD_ISSET(i, &readfds)) {
            if (i != tcp_sockfd && i != udp_sockfd
            && i != STDIN_FILENO) {
                close(i);
            }
        }
    }
}


