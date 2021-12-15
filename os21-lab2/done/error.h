/**
 * @file error.h
 * @brief Errors headers
 *
 * @author Atri Bhattacharyya
 */
#pragma once

typedef enum {
    SUCCESS = 0,
    ERRNOMEM,
    ERRINVAL,
    EXAMPLE_ERROR,
    MAX_ERROR,
} l2_error;

/* Global variable for holding error number.
 * Similar to errno available by using `errno.h`
 * Holds the condition that caused error for certain
 * functions */
extern l2_error l2_errno;

/**
 * @brief Returns a human-readable description of error condition
 *
 * This function implements functionality similar to, but not equalling,
 * `strerror`. This string must not be modified by the application
 *
 * @param  errno           The error code.
 * @return  A pointer to a string describing the error
 */
const char *l2_strerror(l2_error errno);