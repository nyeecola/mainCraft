#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <vulkan/vulkan.h>

#include "vk_types.h"


int64_t
find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags prop);

int
create_gpu_buffer(struct vk_device *dev, VkDeviceMemory *buffer_memory, VkBuffer *buffer,
				  void *buffer_data, VkDeviceSize buffer_size, VkBufferUsageFlags usage);

int
create_buffer(struct vk_device *dev, VkDeviceSize size, VkBufferUsageFlags usage,
			  VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffer_memory);

int
copy_buffer(struct vk_cmd_submission *cmd_sub, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

#endif //VK_BUFFER_H
