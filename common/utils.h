#ifndef UTILS_H
#define UTILS_H

#define array_size(array) (sizeof(array)/sizeof(*array))

#define print_error(msg) fprintf(stderr, "%s:%d - Error: "  msg "\n", __FILE__, __LINE__)

#define min(var, var2) ((var) < (var2) ? (var) : (var2))
#define max(var, var2) ((var) > (var2) ? (var) : (var2))

#endif //UTILS_H
