/**
 * @file error.c
 * @brief Errors defined
 *
 * @author Atri Bhattacharyya
 */
#include "error.h"

/* Global declaration of error variable */
l2_error l2_errno = SUCCESS;

const char *error_strings[] = {
    "SUCCESS",
    "Out of memory",
    "Invalid argument",
    "Example error",
    "Error code out of bounds"
};

const char *l2_strerror(l2_error errno) {
  /* Return error string if in bounds */
  if((errno < MAX_ERROR) && (errno >= 0))
    return error_strings[errno];
  else 
    return error_strings[MAX_ERROR];
}