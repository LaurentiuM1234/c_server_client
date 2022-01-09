#ifndef _UTIL_H
#define _UTIL_H

#define UDP_BUFFER_SIZE 1551
#define CMD_BUFFER_SIZE 200
#define CONTENT_SIZE 1500
#define ID_SIZE 11
#define TOPIC_NAME_SIZE 50
#define MAX_ARGC 5
#define CMD_DELIM " "


/**
 * @brief Convert string representation of port to integer.
 *
 * @param string String representation of the port.
 *
 * @return Integer holding the value of the port upon success
 * @return -ENEGPORT - Negative port value
 * @return -EFAILEDCONV - Failed port conversion
 */
int to_port(char *string);

/**
 * @brief Compute max value.
 *
 * @param v1 First value.
 * @param v2 Second value.
 *
 * @return The max value
 */
int max_value(int v1, int v2);

/**
 * @brief Read input from stdin.
 *
 * @param buffer Buffer in which the input will be stored.
 * @param buffer_size Size of the buffer.
 *
 * @return 0 - Function executed successfully
 * @return -ESYSCALL - Failed system call
 */
int read_from_stdin(char *buffer, int buffer_size);

/**
 * @brief Build argument vector.
 *
 * @param buffer Buffer holding the arguments used to build the vector.
 * @param argc Pointer to variable holding the number of arguments.
 *
 * @return Argument vector upon success
 * @return NULL - Failed memory allocation
 * */
char **build_argv(char *buffer, int *argc);

/**
 * @brief Free memory used by the argument vector.
 *
 * @param argv The argument vector to be freed.
 */
void free_argv(char **argv);


#endif /* _UTIL_H */