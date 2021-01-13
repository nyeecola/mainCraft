#ifndef WINDOW_H
#define WINDOW_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include "constants.h"
#include "types.h"
#include "input.h"

int
create_window(GLFWwindow **window, struct glfw_callback_data *data_pointer, char *window_title);

#endif //WINDOW_H
