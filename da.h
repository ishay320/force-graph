#ifndef DA_H
#define DA_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/**
 * @file da.h
 *
 * @brief How to use the file:
 *  1. Include the `da.h` header file in your `C` code.
 *  2. Initialize a dynamic array using `da_init`.
 *  3. Push elements into the dynamic array using `da_push`.
 *  4. Use the elements by accessing their location `array.data[i]`.
 *  5. Free memory associated with the dynamic array using `da_free`.
 */

/**
 * @struct da
 * @brief Example of structure representing a dynamic array.
 * (replace the int with your own)
 */
struct da {
    int* data;  /**< Pointer to the array data */
    size_t len; /**< Length of the used space in the array */
    size_t cap; /**< Capacity of all the data array */
};

/* get:
                the user can get it himself
*/

/**
 * @brief Initialize a dynamic array.
 *
 * @param array The dynamic array struct to initialize.
 * @param initial_cap The initial cap of the array.
 */
#define da_init(array, initial_cap)                                         \
    do {                                                                    \
        (array).data = malloc(sizeof((array).data) * (initial_cap));        \
        if (!(array).data) {                                                \
            fprintf(stderr, "%s:%d could not init the dynamic array: %s\n", \
                    __FILE__, __LINE__, strerror(errno));                   \
            exit(1);                                                        \
        }                                                                   \
        (array).cap = (initial_cap);                                        \
        (array).len = 0;                                                    \
    } while (0)

/**
 * @brief Free memory associated with a dynamic array.
 *
 * @param dynamic_array The dynamic array struct to free.
 */
#define da_free(dynamic_array)    \
    do {                          \
        dynamic_array.len = 0;    \
        dynamic_array.cap = 0;    \
        free(dynamic_array.data); \
    } while (0)

/**
 * @brief Push an item to the end of the dynamic array.
 * if no space available then realloc and push.
 *
 * @param array The dynamic array.
 * @param item The item to push.
 *
 * @todo return error
 */
#define da_push(array, item)                                                \
    do {                                                                    \
        if ((array).cap == (array).len) {                                   \
            typeof((array).data) data = realloc(                            \
                (array).data,                                               \
                (sizeof((array).data) * ((array).cap + !(array).cap)) * 2); \
            if (!data) {                                                    \
                fprintf(                                                    \
                    stderr,                                                 \
                    "%s:%d failed to push data to the dynamic array: %s\n", \
                    __FILE__, __LINE__, strerror(errno));                   \
                exit(1);                                                    \
            }                                                               \
            else {                                                          \
                (array).cap  = ((array).cap + !(array).cap) * 2;            \
                (array).data = data;                                        \
            }                                                               \
        }                                                                   \
        (array).data[(array).len] = (item);                                 \
        (array).len++;                                                      \
    } while (0)

/**
 * @brief Reserve space in the dynamic array.
 *
 * @param array The dynamic array.
 * @param new_size The new size to reserve.
 *
 * @todo return error
 */
// TODO: check if its working
#define da_reserve(array, new_size)                                          \
    do {                                                                     \
        if ((array).cap < (new_size)) {                                      \
            typeof((array).data) data =                                      \
                realloc((array).data,                                        \
                        sizeof(*(array).data) * ((array).cap + (new_size))); \
            if (data == NULL) {                                              \
                fprintf(stderr,                                              \
                        "%s:%d failed to reserve the dynamic array: %s\n",   \
                        __FILE__, __LINE__, strerror(errno));                \
                exit(1);                                                     \
            }                                                                \
            else {                                                           \
                (array).cap += (new_size);                                   \
                (array).data = data;                                         \
            }                                                                \
        }                                                                    \
    } while (0)

// TODO: work in progress
#define da_shrink(array, new_size)                                         \
    do {                                                                   \
        if ((new_size) <= (array).cap) {                                   \
            typeof((array).data) data =                                    \
                realloc((array).data, sizeof(*(array).data) * (new_size)); \
            if (data == NULL) {                                            \
                fprintf(stderr,                                            \
                        "%s:%d failed to shrink the dynamic array: %s\n",  \
                        __FILE__, __LINE__, strerror(errno));              \
                exit(1);                                                   \
            }                                                              \
            else {                                                         \
                (array).cap  = (new_size);                                 \
                (array).data = data;                                       \
            }                                                              \
        }                                                                  \
    } while (0)

#endif  // DA_H

/* TODO: check if this is better, and the user will make his own struct
struct dynamic_array {
    size_t len;
    size_t cap;
    void* data;
    size_t item_size;
}; */
