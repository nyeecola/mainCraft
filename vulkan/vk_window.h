#ifndef VK_INIT_H
#define VK_INIT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>


GLFWwindow *
vk_init_window();

void
vk_destroy_window(GLFWwindow *window);

#endif //VK_INIT_H
