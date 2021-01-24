#include "vk_gpu_objects.h"
#include "vk_buffer.h"
#include "utils.h"

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

int
create_mvp_buffers(struct vk_device *dev, struct view_projection *mvp)
{
	VkDeviceSize buffer_size = sizeof(struct MVP);
	struct vk_swapchain *swapchain = &dev->swapchain;
	VkDeviceMemory *local_buffer_memory;
	VkBuffer *local_buffer;
	size_t i, ret;

	local_buffer = malloc(sizeof(VkBuffer) * swapchain->images_count);
	if (!local_buffer) {
		print_error("Failed to allocate mvp uniform buffer");
		return -1;
	}

	local_buffer_memory = malloc(sizeof(VkDeviceMemory) * swapchain->images_count);
	if (!local_buffer_memory) {
		print_error("Failed to allocate mvp uniform buffer memory");
		free(mvp->buffers);
		return -1;
	}

	/* We need create several(equal to the swap chains) because we have
	 * Several swap chain buffer on the fly at same time and we need
	 * a uniform buffer with MVP to each frame.
	 * */
	for (i = 0; i < swapchain->images_count; i++) {
		ret = create_buffer(dev, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							&local_buffer[i], &local_buffer_memory[i]);
		if (ret)
			break;
	}

	if (i != swapchain->images_count) {
		destroy_buffer_vector(dev, mvp->buffers, mvp->buffers_memory, i);
		return -1;
	}

	mvp->buffers = local_buffer;
	mvp->buffers_memory = local_buffer_memory;
	mvp->buffer_count = swapchain->images_count;
	return 0;
}

void
destroy_buffer_vector(struct vk_device *dev, VkBuffer *buffers, VkDeviceMemory *buffers_memory, uint32_t buffer_count)
{
	int i;

	for (i = 0; i < buffer_count; i++) {
		vkDestroyBuffer(dev->logical_device, buffers[i], NULL);
		vkFreeMemory(dev->logical_device, buffers_memory[i], NULL);
	}

	free(buffers);
	free(buffers_memory);
}
