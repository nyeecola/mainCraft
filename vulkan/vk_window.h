#ifndef VK_INIT_H
#define VK_INIT_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include "vk_types.h"

int
vk_init_window(struct vk_program *program);

void
vk_destroy_window(struct window *game_window);

#endif //VK_INIT_H
