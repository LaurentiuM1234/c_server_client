#ifndef _USER_H
#define _USER_H

#include <arpa/inet.h>

#include "../list.h"
#include "../util.h"

/**
 * @brief User of the server.
 *
 * @member _address IP address of the user.
 * @member _socket TCP socket used to communicate with the user.
 * @member _id ID of the user.
 * @member _is_active State variable showing if the user is currently active.
 * @member _subscriptions List of all topics a user is subscribed to.
 * @member _unsent_data List of all unsent messages.
 */
struct user {
    struct in_addr _address;
    int _socket;
    char _id[ID_SIZE];
    int _is_active;
    struct node *_subscriptions;
    struct node *_unsent_data;
};

/**
 * @brief Topic a user can be subscribed to.
 *
 * @member _name Name of the topic
 * @member _enable_sf State variable showing if SF is enabled for this topic.
 */
struct topic {
    char _name[TOPIC_NAME_SIZE];
    int _enable_sf;
};

/**
 * @brief Allocate memory for user.
 *
 * @return Pointer to newly allocated user upon success
 * @return NULL upon failure
 */
struct user *create_user();

/**
 * @brief Allocate memory for topic.
 *
 * @return Pointer to newly allocated topic upon success
 * @return NULL upon failure
 */
struct topic *create_topic();

/**
 * @brief Add user to list of users.
 *
 * @param users List of users.
 * @param id ID of the user to be added.
 * @param addr IP address of the user to be added.
 * @param socket Connection socket of the user to be added.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 * @return -EUSEDID - Used client id
 * @return -ECLOSEDCONN - Client closed connection
 */
int add_user_to_list(struct node **users, char *id, struct in_addr addr,
        int socket);

/**
 * @brief Send message to TCP clients.
 *
 * @param buffer Buffer holding contents of the message to be sent.
 * @param buffer_size Size of the buffer.
 * @param info Information about the sender of the message.
 * @param users List of all users.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 */
int send_message(char *buffer, int buffer_size, struct sockaddr_in info,
                 struct node *users);

/**
 * @brief Add new topic to a user's list of subscriptions.
 *
 * @param sockfd Connection socket used to communicate with the user.
 * @param users List of all users.
 * @param name Name of the topic.
 * @param enable_sf State variable showing if SF is enabled for the topic.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 */
int add_topic(int sockfd, struct node *users, char *name, int enable_sf);

/**
 * @brief Remove topic from a user's list of subscriptions.
 *
 * @param sockfd Connection socket used to communicate with the user.
 * @param users List of all users.
 * @param name Name of the topic.
 */
void remove_topic(int sockfd, struct node *users, char *name);

/**
 * @brief Free memory used by user object.
 *
 * @param info Pointer to memory used by user object.
 */
void free_user(void *info);




#endif /* _USER_H */
