#include <stdlib.h>
#include <string.h>

#include "message.h"


struct message *create_message()
{
    return calloc(1, sizeof(struct message));
}

void get_topic(struct message *msg, char *buffer)
{
    memcpy(buffer, msg->_payload, TOPIC_NAME_SIZE);
}

void get_type(struct message *msg, uint8_t *type)
{
    memcpy(type, msg->_payload + TOPIC_NAME_SIZE, 1);
}

void get_content(struct message *msg, char *buffer)
{
    memcpy(buffer, msg->_payload + TOPIC_NAME_SIZE + 1, CONTENT_SIZE);
}