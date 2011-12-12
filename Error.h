/* vim: set filetype=cpp.doxygen : */
#ifndef ERROR_H
#define ERROR_H 

#include <stdio.h>
#include <string.h>
#include <errno.h>

#define ERROR(s, ...) \
  fprintf(stderr, "----------------------------------------------------\n");\
  fprintf(stderr, "%s:%d ERROR: " s, __FILE__, __LINE__, ##__VA_ARGS__); \
  fprintf(stderr, "----------------------------------------------------\n");\
  fflush(stderr);


#endif /* ERROR_H */
