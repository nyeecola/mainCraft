#include <string.h>

#include "vk_command_buffer.h"
#include "game_objects.h"
#include "vk_buffer.h"
#include "utils.h"


int64_t
find_memory_type(VkPhysicalDevice physical_device, uint32_t type_filter, VkMemoryPropertyFlags prop)
{
	VkPhysicalDeviceMemoryProperties mem_prop;
	uint32_t i;

	vkGetPhysicalDeviceMemoryProperties(physical_device, &mem_prop);

	for (i = 0; i < mem_prop.memoryTypeCount; i++)
		if ((type_filter & (1 << i)) && (mem_prop.memoryTypes[i].propertyFlags & prop) == prop)
			return i;

	return -1;
}

int
create_buffer(struct vk_device *dev, VkDeviceSize size, VkBufferUsageFlags usage,
			 VkMemoryPropertyFlags properties, VkBuffer *buffer, VkDeviceMemory *buffer_memory)
{
	VkMemoryRequirements mem_requirements;
	int64_t mem_type;
	VkResult result;

	VkBufferCreateInfo buffer_info = {
		.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO,
		.size = size,
		.usage = usage,
		.sharingMode = VK_SHARING_MODE_EXCLUSIVE
	};

	result = vkCreateBuffer(dev->logical_device, &buffer_info, NULL, buffer);
	if (result != VK_SUCCESS) {
		print_error("Failed to create buffer!");
		goto return_error;
	}

	vkGetBufferMemoryRequirements(dev->logical_device, *buffer, &mem_requirements);
	mem_type = find_memory_type(dev->physical_device, mem_requirements.memoryTypeBits, properties);
	if (mem_type == -1) {
		print_error("Failed to find suitable memory type!");
		goto destroy_buffer;
	}

	VkMemoryAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = mem_requirements.size,
		.memoryTypeIndex = (uint32_t) mem_type
	};

	result = vkAllocateMemory(dev->logical_device, &alloc_info, NULL, buffer_memory);
	if (result != VK_SUCCESS) {
		print_error("Failed to allocate buffer memory!");
		goto destroy_buffer;
	}

	result = vkBindBufferMemory(dev->logical_device, *buffer, *buffer_memory, 0);
	if (result != VK_SUCCESS) {
		print_error("Failed to bind buffer with buffer memory!");
		goto free_memory_buffer;
	}

	return 0;

free_memory_buffer:
	vkFreeMemory(dev->logical_device, *buffer_memory, NULL);
destroy_buffer:
	vkDestroyBuffer(dev->logical_device, *buffer, NULL);
return_error:
	return -1;
}

int
copy_buffer(struct vk_device *dev, VkBuffer src_buffer, VkBuffer dst_buffer, VkDeviceSize size)
{
	VkCommandBuffer cmd_buffer;
	VkResult result;
	VkQueue queue;

	if (dev->cmd_buffers_count[transfer] > 0) {
		cmd_buffer = dev->cmd_buffers[transfer][0];
		queue = dev->queues.handles[transfer];
	} else {
		cmd_buffer = dev->cmd_buffers[graphics][0];
		queue = dev->queues.handles[graphics];
	}

	result = begin_single_time_commands(cmd_buffer);
	if (result != VK_SUCCESS)
		return -1;

	VkBufferCopy copy_region = {
		.size = size
	};

	vkCmdCopyBuffer(cmd_buffer, src_buffer, dst_buffer, 1, &copy_region);

	result = end_single_time_commands(cmd_buffer, queue);
	if (result != VK_SUCCESS)
		return -1;

	return 0;
}

int
create_gpu_buffer(struct vk_device *dev, VkDeviceMemory *buffer_memory, VkBuffer *buffer,
				  void *buffer_data, VkDeviceSize buffer_size, VkBufferUsageFlags usage)
{
	VkDeviceMemory staging_buffer_memory, local_buffer_memory;
	VkBuffer staging_buffer, local_buffer;
	VkResult result;
	void *data;
	int ret;

	ret = create_buffer(dev, buffer_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
					   VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
					   &staging_buffer, &staging_buffer_memory);
	if (ret)
		goto return_error;

	result = vkMapMemory(dev->logical_device, staging_buffer_memory, 0, buffer_size, 0, &data);
	if (result != VK_SUCCESS) {
		print_error("Failed to map buffer to system memory!");
		goto destoy_staging_buffer;
	}

	memcpy(data, buffer_data, (size_t) buffer_size);
	vkUnmapMemory(dev->logical_device, staging_buffer_memory);

	ret = create_buffer(dev, buffer_size, VK_BUFFER_USAGE_TRANSFER_DST_BIT | usage,
					   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &local_buffer, &local_buffer_memory);
	if (ret)
		goto destoy_staging_buffer;

	if (copy_buffer(dev, staging_buffer, local_buffer, buffer_size)) {
		vkDestroyBuffer(dev->logical_device, local_buffer, NULL);
		vkFreeMemory(dev->logical_device, local_buffer_memory, NULL);
		goto destoy_staging_buffer;
	}

	*buffer = local_buffer;
	*buffer_memory = local_buffer_memory;

	ret = 0;

destoy_staging_buffer:
	vkDestroyBuffer(dev->logical_device, staging_buffer, NULL);
	vkFreeMemory(dev->logical_device, staging_buffer_memory, NULL);
return_error:
	return ret;
}

int
create_vertex_buffer(struct vk_device *dev, struct vk_vertex_object *vertex_object)
{
	VkDeviceSize size = sizeof(struct vertex) * vertex_object->vertices_count;

	return create_gpu_buffer(dev, &vertex_object->vertex_buffer_memory, &vertex_object->vertex_buffer,
							 vertex_object->vertices, size, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
}

int
create_index_buffer(struct vk_device *dev, struct vk_vertex_object *vertex_object)
{
	VkDeviceSize size = sizeof(uint16_t) * vertex_object->indices_count;

	return create_gpu_buffer(dev, &vertex_object->index_buffer_memory, &vertex_object->index_buffer,
							 vertex_object->indices, size, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
}

