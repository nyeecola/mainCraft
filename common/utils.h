#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <math.h>

#define array_size(array) (sizeof(array)/sizeof(*array))

#define pprint_error(msg, ...) fprintf(stderr, "%s:%d - Error: "  msg "\n", __FILE__, __LINE__, __VA_ARGS__)

#define print_error(msg) fprintf(stderr, "%s:%d - Error: "  msg "\n", __FILE__, __LINE__)

#define min(var, var2) ((var) < (var2) ? (var) : (var2))
#define max(var, var2) ((var) > (var2) ? (var) : (var2))

#define get_alignment(value, stride) (ceil((double)(value) / (stride)) * (stride))

char *
read_file(const char *file_name, int64_t *out_size);

#endif //UTILS_H
