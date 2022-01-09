#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "user.h"
#include "../message.h"
#include "../error.h"
#include "connection.h"

struct user *create_user()
{
    struct user *user = calloc(1, sizeof(struct user));

    if (user == NULL)
        return NULL;

    // upon creation user is active
    user->_is_active = 1;

    return user;
}

struct topic *create_topic()
{
    return calloc(1, sizeof(struct topic));
}

void free_user(void *info)
{
    struct user *usr = (struct user*)info;
    free_list(&usr->_subscriptions, free);
    free_list(&usr->_unsent_data, free);
    free(usr);
}

int send_message(char *buffer, int buffer_size, struct sockaddr_in info,
        struct node *users)
{
    int err_code;

    // prepare transfer buffer
    int transfer_buffer_size = sizeof(struct message);
    char transfer_buffer[transfer_buffer_size];

    // prepare message
    struct message *msg = create_message();

    if (msg == NULL) {
        err_code = -ESYSCALL;
        goto err_ret;
    }

    memcpy(msg->_payload, buffer, buffer_size);
    msg->_src_address = info.sin_addr;
    msg->_src_port = ntohs(info.sin_port);

    // extract topic name from buffer
    char topic_name[TOPIC_NAME_SIZE];
    memcpy(topic_name, buffer, TOPIC_NAME_SIZE);

    // dump contents of msg into transfer buffer
    memcpy(transfer_buffer, msg, transfer_buffer_size);

    while (users != NULL) {
        struct user *usr = (struct user*)users->_info;

        struct node *i = usr->_subscriptions;

        while (i != NULL) {
            struct topic *crt_topic = (struct topic*)i->_info;

            if (strncmp(crt_topic->_name, topic_name, TOPIC_NAME_SIZE) == 0) {
                if (usr->_is_active) {
                    // if user is active, send message
                    err_code = tcp_send(usr->_socket, transfer_buffer,
                                        transfer_buffer_size);
                    if (err_code < 0)
                        goto err_free_message;

                } else {
                    if (crt_topic->_enable_sf) {
                        // if sf policy is enabled, store copy
                        struct message *cpy = create_message();

                        if (cpy == NULL) {
                            err_code = -ESYSCALL;
                            goto err_free_message;
                        }

                        memcpy(cpy, msg, sizeof(struct message));

                        err_code = push_front(&usr->_unsent_data, cpy);

                        if (err_code < 0)
                            goto err_free_message;
                    }
                }
            }

            i = i->_next;
        }
        users = users->_next;
    }

    free(msg);
    return 0;

err_free_message:
    free(msg);
err_ret:
    return err_code;
}


static int send_stored_data(struct user *usr)
{
    int buffer_size = sizeof(struct message);
    char transfer_buf[buffer_size];

    int err_code;

    while (usr->_unsent_data != NULL) {
        struct message *crt = pop_front(&usr->_unsent_data);

        memcpy(transfer_buf, crt, sizeof(struct message));

        err_code = tcp_send(usr->_socket, transfer_buf, buffer_size);

        if (err_code < 0)
            return -ESYSCALL;
    }
    return 0;
}

int add_user_to_list(struct node **users, char *id, struct in_addr addr,
        int socket)
{
    int err_code;

    struct user *usr = create_user();

    if (usr == NULL) {
        err_code = -ESYSCALL;
        goto err_ret;
    }

    // fill remaining user info
    usr->_address = addr;
    usr->_socket = socket;
    memcpy(usr->_id, id, ID_SIZE);

    if (*users == NULL) {
        // no users, just push back
        err_code = push_back(users, usr);

        if (err_code < 0)
            goto err_free_usr;
    } else {
        struct node *i = *users;

        while (i != NULL) {
            struct user *crt = (struct user*)i->_info;

            if (strcmp(crt->_id, id) == 0) {
                // same id, check if active
                if (crt->_is_active) {
                    err_code = -EUSEDID;
                    goto err_free_usr;
                } else {
                    if (usr->_address.s_addr == crt->_address.s_addr) {
                        // if matching addresses, activate
                        crt->_is_active = 1;
                        crt->_socket = socket;
                        free(usr);

                        // send stored data
                        err_code = send_stored_data(crt);

                        if (err_code < 0)
                            goto err_ret;
                    } else {
                        // same id but diff ip addresses, update value
                        void *tmp = i->_info;
                        i->_info = usr;
                        free(tmp);
                    }
                    return 0;
                }
            }
            i = i->_next;
        }
        // if reached this point just push front user
        err_code = push_front(users, usr);

        if (err_code < 0)
            goto err_free_usr;
    }

    return 0;

err_free_usr:
    free(usr);
err_ret:
    return err_code;
}


void mark_as_inactive(int sockfd, struct node *users)
{
    while (users != NULL) {
        struct user *crt = (struct user*)users->_info;

        if (crt->_socket == sockfd) {
            // got match, mark as inactive
            crt->_is_active = 0;
            crt->_socket = -1;

            // announce disconnection
            printf("Client %s disconnected.\n", crt->_id);

            break;
        }
        users = users->_next;
    }
}

static int topic_cmp(void* info1, void* info2)
{
    struct topic *t1 = (struct topic*)info1;
    struct topic *t2 = (struct topic*)info2;

    if (strcmp(t1->_name, t2->_name) == 0)
        return 1;

    return 0;
}

int add_topic(int sockfd, struct node *users, char *name, int enable_sf)
{
    int err_code;

    // create new topic
    struct topic *new = create_topic();

    if (new == NULL)
        return -ESYSCALL;

    new->_enable_sf = enable_sf;
    memcpy(new->_name, name, strlen(name));

    while (users != NULL) {
        struct user *crt_user = (struct user*)users->_info;

        if (crt_user->_socket == sockfd) {

            // create dummy topic for search purposes
            struct topic tmp;
            memset(tmp._name, 0, TOPIC_NAME_SIZE);
            memcpy(tmp._name, name, strlen(name));

            struct topic *res =
                    pop_element(&crt_user->_subscriptions, &tmp, topic_cmp);

            if (res == NULL) {
                // add topic if not already there
                err_code = push_front(&crt_user->_subscriptions, new);

                if (err_code < 0) {
                    free(new);
                    return err_code;
                }

            } else {
                // add subscription back to list
                err_code = push_front(&crt_user->_subscriptions, res);

                if (err_code < 0)
                    return err_code;
            }

            break;
        }
        users = users->_next;
    }

    return 0;
}

void remove_topic(int sockfd, struct node *users, char *name)
{
    while (users != NULL) {
        struct user *crt_user = (struct user*)users->_info;

        if (crt_user->_socket == sockfd) {
            // create dummy topic for search purposes
            struct topic tmp;
            memset(tmp._name, 0, TOPIC_NAME_SIZE);
            memcpy(tmp._name, name, strlen(name));

            // search and free wanted element
            struct topic *res =
                    pop_element(&crt_user->_subscriptions, &tmp, topic_cmp);

            // topic was not found
            if (res == NULL)
                break;

            free(res);
            break;
        }
        users = users->_next;
    }
}