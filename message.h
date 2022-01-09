#ifndef _MESSAGE_H
#define _MESSAGE_H

#include <arpa/inet.h>

#include "util.h"

/**
 * @brief Message sent to the client by the server.
 *
 * @member _src_address IP address of the source of the message.
 * @member _src_port Port of the source of the message.
 * @member _payload Buffer containing actual contents of the message.
 */
struct message {
    struct in_addr _src_address;
    int _src_port;
    char _payload[UDP_BUFFER_SIZE];
} __attribute__((packed));

/**
 * @brief Allocate memory for message.
 *
 * @return Pointer to newly allocated message upon success.
 * @return NULL upon failure.
 */
struct message *create_message();

/**
 * @brief Extract topic name from message.
 *
 * @param msg Pointer to message received from the server.
 * @param buffer Buffer in which the topic will be stored.
 */
void get_topic(struct message *msg, char *buffer);

/**
 * @brief Extract content type from message.
 *
 * @param msg Pointer to message received from the server.
 * @param type Pointer to variable which will hold the content type.
 */
void get_type(struct message *msg, uint8_t *type);

/**
 * @brief Extract content from message.
 *
 * @param msg Pointer to message received from the server.
 * @param buffer Buffer in which the content will be stored.
 */
void get_content(struct message *msg, char *buffer);



#endif /* _MESSAGE_H */
