#ifndef VK_SWAP_CHAIN_H
#define VK_SWAP_CHAIN_H

#include "vk_types.h"

int
create_swapchain(struct vk_device *device, VkSurfaceKHR surface, GLFWwindow *window);

#endif //VK_SWAP_CHAIN_H
