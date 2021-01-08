#ifndef VK_TYPES_H
#define VK_TYPES_H

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <vulkan/vulkan.h>
#include <cglm/cglm.h>
#include <stdbool.h>

#include "vk_constants.h"


enum family_indices { graphics = 0, transfer, compute, protectedBit, sparseBindingBit, present, queues_count };

struct view_projection {
	VkBuffer *buffers;
	uint32_t buffer_count;
	VkDeviceMemory *buffers_memory;
	uint32_t buffer_memory_count;
	mat4 proj;
};

struct vk_vertex_object {
	VkBuffer vertex_buffer;
	VkDeviceMemory vertex_buffer_memory;
	struct vertex *vertices;
	uint64_t vertices_count;
	uint16_t *indices;
	uint64_t indices_count;
	VkBuffer index_buffer;
	VkDeviceMemory index_buffer_memory;
};

struct vk_game_objects {
	struct vk_vertex_object dummy_triangle;
	struct view_projection camera;
};

struct vk_device_properties {
	VkPhysicalDeviceFeatures supported_features;
	VkPhysicalDeviceProperties device_properties;
};

struct vk_draw_sync {
	VkSemaphore image_available_semaphore[MAX_FRAMES_IN_FLIGHT];
	VkSemaphore render_finished_semaphore[MAX_FRAMES_IN_FLIGHT];
	VkFence in_flight_fences[MAX_FRAMES_IN_FLIGHT];
	VkFence *images_in_flight;
};

struct vk_cmd_submission {
	VkCommandPool command_pools[queues_count];
	uint64_t pools_allocated;
	VkCommandBuffer *cmd_buffers[queues_count];
	uint32_t cmd_buffers_count[queues_count];
	uint32_t family_indices[queues_count];
	uint32_t queue_count[queues_count];
	VkQueue queue_handles[queues_count];
	/* Per family in use queue count*/
	uint32_t handles_count[queues_count];
	VkDescriptorPool descriptor_pool;
	VkDescriptorSet *descriptor_sets;
	uint32_t descriptors_count;
};

struct vk_render {
	VkRenderPass render_pass;
	VkPipeline graphics_pipeline;
	VkPipelineLayout pipeline_layout;;
	VkFramebuffer *swapChain_framebuffers;
	uint32_t framebuffer_count;
	VkDescriptorSetLayout descriptor_set_layout;
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
	bool framebuffer_resized;
};

struct vk_device {
	VkPhysicalDevice physical_device;
	VkDevice logical_device;
	struct vk_cmd_submission cmd_submission;
	struct vk_swapchain swapchain;
	struct vk_render render;
	struct vk_draw_sync draw_sync;
	struct vk_game_objects game_objs;
	struct vk_device_properties device_properties;
};

struct vk_program {
	GLFWwindow *window;
	VkSurfaceKHR surface;
	VkApplicationInfo app_info;
	VkInstance instance;
	struct vk_device device;
};

#endif //VK_TYPES_H
