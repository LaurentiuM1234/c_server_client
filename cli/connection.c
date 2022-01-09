#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <string.h>

#include "connection.h"
#include "../util.h"
#include "../error.h"

int tcp_socket(char *id, char *addr, char *port_string)
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

    struct sockaddr_in sock_info;
    sock_info.sin_port = htons(port);
    sock_info.sin_family = AF_INET;
    sock_info.sin_addr.s_addr = inet_addr(addr);

    err_code = connect(sockfd, (const struct sockaddr*)&sock_info,
            sizeof(sock_info));

    if (err_code == -1)
        goto err_close_socket;

    // send id right after establishing connection
    char id_buffer[ID_SIZE];
    memset(id_buffer, 0, ID_SIZE);
    strcpy(id_buffer, id);

    err_code = tcp_send(sockfd, id_buffer, ID_SIZE);

    if (err_code < 0)
        goto err_close_socket;

    if (err_code == 0) {
        err_code = -ECLOSEDCON;
        goto err_close_socket;
    }

    return sockfd;

err_close_socket:
    close(sockfd);
err_ret:
    return err_code;
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

