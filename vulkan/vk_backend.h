#ifndef VK_BACKEND_H
#define VK_BACKEND_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


enum family_indices { graphics = 0, transfer, compute, protectedBit, sparseBindingBit, present, queues_count };

struct vk_queues {
	uint32_t family_indices[queues_count];
	uint32_t queue_count[queues_count];
};

struct vk_device {
	VkPhysicalDevice physical_device;
	struct vk_queues queues;
};


struct vk_program {
	GLFWwindow *window;
	VkSurfaceKHR surface;
	VkApplicationInfo app_info;
	VkInstance instance;
	struct vk_device device;
};


int
run_vk(const int argc, char *const *argv);

#endif //VK_BACKEND_H
