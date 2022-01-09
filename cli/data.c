#include <string.h>
#include <stdio.h>
#include <math.h>

#include "data.h"
#include "../util.h"
#include "../error.h"
#include "connection.h"
#include "../message.h"


int process_command(char *cmd_buffer, int buffer_size, int *client_is_up,
                    int tcp_sockfd)
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

    if (strcmp(argv[0], "exit") == 0) {
        if (argc != 1) {
            // wrong parameter count
            err_code = -EUNKCMD;
            goto err_free_argv;
        }
        *client_is_up = 0;
    } else if (strcmp(argv[0], "subscribe") == 0) {
        if (argc != 3) {
            // wrong parameter count
            err_code = -EUNKCMD;
            goto err_free_argv;
        }
        int enable_sf = to_port(argv[2]);

        if (enable_sf > 1 || enable_sf == -ENEGPORT) {
            err_code = -EUNKCMD;
            goto err_free_argv;
        }

        if (enable_sf < 0)
            goto err_free_argv;

        err_code = tcp_send(tcp_sockfd, cmd_buffer, buffer_size);

        if (err_code < 0)
            goto err_free_argv;


        printf("Subscribed to topic.\n");
    } else if (strcmp(argv[0], "unsubscribe") == 0) {
        if (argc != 2) {
            // wrong parameter count
            err_code = -EUNKCMD;
            goto err_free_argv;
        }

        err_code = tcp_send(tcp_sockfd, cmd_buffer, buffer_size);

        if (err_code < 0)
            goto err_free_argv;

        printf("Unsubscribed from topic.\n");
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

static void process_content(char *content, uint8_t type)
{
    if (type == 0) {
        uint8_t sign;
        uint32_t value;

        // extract values
        memcpy(&sign, content, 1);
        memcpy(&value, content + 1, 4);

        value = ntohl(value);

        if (sign)
            printf("INT - -%u\n", value);
        else
            printf("INT - %u\n", value);
    } else if (type == 1) {
        uint16_t value;

        // extract value
        memcpy(&value, content, 2);

        value = ntohs(value);

        printf("SHORT_REAL - %.2f\n", ((float)value / 100));
    } else if (type == 2) {
        uint8_t sign, power;
        uint32_t value;

        // extract values
        memcpy(&sign, content, 1);
        memcpy(&value, content + 1, 4);
        memcpy(&power, content + 5, 1);

        value = ntohl(value);

        double result = (double)value * pow(10, (-1) * (int)power);

        if (sign)
            printf("FLOAT - -%lf\n", result);
        else
            printf("FLOAT - %lf\n", result);
    } else if (type == 3) {
        printf("STRING - %s\n", content);
    }
}

void process_message(const char *buffer)
{
    // extract message from buffer
    struct message msg = *(struct message*)buffer;

    // preparing to extract data
    char topic[TOPIC_NAME_SIZE + 1];
    memset(topic, 0, TOPIC_NAME_SIZE + 1);
    uint8_t type;
    char content[CONTENT_SIZE + 1];
    memset(content, 0, CONTENT_SIZE + 1);

    get_topic(&msg, topic);
    get_type(&msg, &type);
    get_content(&msg, content);

    printf("%s:%d - %s - ", inet_ntoa(msg._src_address),
           msg._src_port, topic);

    process_content(content, type);
}