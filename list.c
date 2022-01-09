#include <stdlib.h>

#include "list.h"
#include "error.h"


static struct node *alloc_node(void *info)
{
    struct node *node = calloc(1, sizeof(struct node));

    if (node == NULL)
        return NULL;

    node->_info = info;

    return node;
}


int push_back(struct node **list, void *info)
{
    struct node *node = alloc_node(info);

    if (node == NULL)
        return -ESYSCALL;

    if (*list == NULL) {
        *list = node;
    } else {
        struct node *i = *list;

        while (i->_next != NULL)
            i = i->_next;

        i->_next = node;
    }
    return 0;
}

int push_front(struct node **list, void *info)
{
    struct node *node = alloc_node(info);

    if (node == NULL)
        return -ESYSCALL;

    if (*list == NULL) {
        *list = node;
    } else {
        node->_next = *list;
        *list = node;
    }
    return 0;
}

void *pop_back(struct node **list)
{
    if (*list == NULL)
        return NULL;

    if ((*list)->_next == NULL) {
        void *info = (*list)->_info;
        free(*list);
        *list = NULL;
        return info;
    }

    struct node *crt = *list;
    struct node *prev = NULL;

    while (crt->_next != NULL) {
        prev = crt;
        crt = crt->_next;
    }

    prev->_next = NULL;
    void *info = crt->_info;
    free(crt);
    return info;
}

void *pop_front(struct node **list)
{
    if (*list == NULL)
        return NULL;


    struct node *tmp = *list;
    (*list) = (*list)->_next;
    void *info = tmp->_info;
    free(tmp);
    return info;
}

void free_list(struct node **list, void (*delete_handle)(void*))
{
    if (delete_handle == NULL) {
        // no delete handle provided, don't delete info
        struct node *i = *list;

        while (i != NULL) {
            struct node *tmp = i;
            i = i->_next;
            free(tmp);
        }
        *list = NULL;
    } else {
        struct node *i = *list;

        while (i != NULL) {
            struct node *tmp = i;
            i = i->_next;
            (*delete_handle)(tmp->_info);
            free(tmp);
        }
        *list = NULL;
    }
}

void *pop_element(struct node **list, void *ref,
        int (*cmp_handle)(void*, void*))
{
    if (*list == NULL)
        return NULL;

    if (cmp_handle(ref, (*list)->_info)) {
        // first element, just pop front
        return pop_front(list);
    }

    struct node *crt = *list;
    struct node *prev = NULL;

    while (crt != NULL) {
        if (cmp_handle(crt->_info, ref)) {
            if (crt->_next == NULL)
                return pop_back(list);

            struct node *tmp = crt;
            prev->_next = crt->_next;
            void *info = tmp->_info;
            free(tmp);
            return info;
        }
        prev = crt;
        crt = crt->_next;
    }

    // if function reaches this point => no match
    return NULL;
}