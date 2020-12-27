#ifndef VK_TYPES_H
#define VK_TYPES_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>


enum family_indices { graphics = 0, transfer, compute, protectedBit, sparseBindingBit, present, queues_count };

struct vk_render {
	VkRenderPass render_pass;
};

struct surface_support {
	VkSurfaceCapabilitiesKHR capabilities;
	VkSurfaceFormatKHR *formats;
	uint32_t formats_count;
	VkPresentModeKHR *present_modes;
	uint32_t present_modes_count;
};

struct swapchain_info {
	VkSurfaceFormatKHR surface_format;
	VkPresentModeKHR present_mode;
	VkExtent2D extent;
};

struct vk_swapchain {
	VkSwapchainKHR handle;
	VkImageView *image_views;
	VkImage* images;
	uint32_t images_count;
	struct swapchain_info state;
	struct surface_support support;
};

struct vk_queues {
	uint32_t family_indices[queues_count];
	uint32_t queue_count[queues_count];
	VkQueue handles[queues_count];
	/* Per family in use queue count*/
	uint32_t handles_count[queues_count];
};

struct vk_device {
	VkPhysicalDevice physical_device;
	VkDevice logical_device;
	struct vk_queues queues;
	VkCommandPool command_pools[queues_count];
	uint64_t pools_allocated;
	VkCommandBuffer *cmd_buffers[queues_count];
	uint32_t cmd_buffers_count[queues_count];
	struct vk_swapchain swapchain;
	struct vk_render render;
};

struct vk_program {
	GLFWwindow *window;
	VkSurfaceKHR surface;
	VkApplicationInfo app_info;
	VkInstance instance;
	struct vk_device device;
};

#endif //VK_TYPES_H
