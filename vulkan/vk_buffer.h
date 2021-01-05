#ifndef VK_BUFFER_H
#define VK_BUFFER_H

#include <vulkan/vulkan.h>

#include "vk_types.h"

int
create_gpu_buffer(struct vk_device *dev, VkDeviceMemory *buffer_memory, VkBuffer *buffer,
				  void *buffer_data, VkDeviceSize buffer_size, VkBufferUsageFlags usage);

int
create_buffer(struct vk_device *dev, VkDeviceSize size, VkBufferUsageFlags usage,
			 VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffer_memory);

int
copy_buffer(struct vk_device *dev, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size);

int
create_vertex_buffer(struct vk_device *dev, struct vk_vertex_object *vertex_object);

int
create_index_buffer(struct vk_device *dev, struct vk_vertex_object *vertex_object);

#endif //VK_BUFFER_H
