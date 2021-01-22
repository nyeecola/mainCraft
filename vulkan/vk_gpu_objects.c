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
	size_t i, ret;

	mvp->buffers = malloc(sizeof(VkBuffer) * swapchain->images_count);
	if (!mvp->buffers) {
		print_error("Failed to allocate mvp uniform buffer");
		goto return_error;
	}

	mvp->buffers_memory = malloc(sizeof(VkDeviceMemory) * swapchain->images_count);
	if (!mvp->buffers_memory) {
		print_error("Failed to allocate mvp uniform buffer memory");
		goto free_buffers;
	}

	/* We need create several(equal to the swap chains) because we have
	 * Several swap chain buffer on the fly at same time and we need
	 * a uniform buffer with MVP to each frame.
	 * */
	for (i = 0; i < swapchain->images_count; i++) {
		ret = create_buffer(dev, buffer_size, VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
							VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
							&mvp->buffers[i], &mvp->buffers_memory[i]);
		if (ret)
			break;
	}

	if (i == swapchain->images_count) {
		mvp->buffer_count = swapchain->images_count;
		return 0;
	}

	destroy_uniform_buffers(dev, mvp->buffers, mvp->buffers_memory, i);
	free(mvp->buffers_memory);
free_buffers:
	free(mvp->buffers);
return_error:
	return -1;
}

void
destroy_uniform_buffers(struct vk_device *dev, VkBuffer *buffers, VkDeviceMemory *buffers_memory, uint32_t buffer_count)
{
	int i;

	for (i = 0; i < buffer_count; i++) {
		vkDestroyBuffer(dev->logical_device, buffers[i], NULL);
		vkFreeMemory(dev->logical_device, buffers_memory[i], NULL);
	}
}
