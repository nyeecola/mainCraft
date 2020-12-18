#ifndef VK_BACKEND_H
#define VK_BACKEND_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>

struct vk_program {
	VkApplicationInfo app_info;
	VkInstance instance;
};


int
run_vk(const int argc, char *const *argv);

#endif //VK_BACKEND_H
