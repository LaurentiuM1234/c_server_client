#ifndef _SERVER_LIST_H
#define _SERVER_LIST_H

/**
 * @brief Node used to build a list.
 *
 * @member _info Information held by the node.
 * @member _next Pointer to the next node in the list.
 */
struct node {
    void *_info;
    struct node *_next;
};

/**
 * @brief Add to the end of the list.
 *
 * @param list Pointer to the list on which the operation is done.
 * @param info Information which will be added to the list.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 */
int push_back(struct node **list, void *info);

/**
 * @brief Add to the beginning of the list.
 *
 * @param list Pointer to the list on which the operation is done.
 * @param info Information which will be added to the list.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 */
int push_front(struct node **list, void *info);

/**
 * @brief Retrieve information found at the end of the list.
 *
 * @param list Pointer to the list on which the operation is done.
 *
 * @return Pointer to retrieved information upon success
 * @return NULL upon failure
 */
void *pop_back(struct node **list);

/**
 * @brief Retrieve information found at the beginning of the list.
 *
 * @param list Pointer to the list on which the operation is done.
 *
 * @return Pointer to retrieved information upon success
 * @return NULL upon failure
 */
void *pop_front(struct node **list);

/**
 * @brief Free memory used by list.
 *
 * @param list Pointer to the list on which the operation is done.
 * @param delete_handle Pointer to function which will be used to free
 * memory used by information contained in the nodes.
 */
void free_list(struct node **list, void (*delete_handle)(void*));

/**
 * @brief Retrieve information using compare function.
 *
 * @param list Pointer to the list on which the operation is done.
 * @param ref Information which will be used as reference in the comparison.
 * @param cmp_handle Function which will be used to compare current node's
 * info and reference.
 *
 * @return Pointer to retrieved information upon success
 * @return NULL upon failure
 */
void *pop_element(struct node **list, void *ref,
        int (*cmp_handle)(void*, void*));


#endif /* _SERVER_LIST_H */
