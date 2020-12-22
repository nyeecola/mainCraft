#ifndef UTILS_H
#define UTILS_H

#define array_size(array) (sizeof(array)/sizeof(*array))

#define pprint_error(msg, ...) fprintf(stderr, "%s:%d - Error: "  msg "\n", __FILE__, __LINE__, __VA_ARGS__)

#define print_error(msg) fprintf(stderr, "%s:%d - Error: "  msg "\n", __FILE__, __LINE__)

#define min(var, var2) ((var) < (var2) ? (var) : (var2))
#define max(var, var2) ((var) > (var2) ? (var) : (var2))

#endif //UTILS_H
