#ifndef VK_GPU_OBJECTS_H
#define VK_GPU_OBJECTS_H

#include "game_objects.h"
#include "vk_types.h"

int
create_vertex_buffer(struct vk_device *dev, struct vk_vertex_object *vertex_object);

int
create_index_buffer(struct vk_device *dev, struct vk_vertex_object *vertex_object);

int
create_mvp_buffers(struct vk_device *dev, struct view_projection *mvp);

void
destroy_uniform_buffers(struct vk_device *dev, VkBuffer *buffers, VkDeviceMemory *buffers_memory, uint32_t buffer_count);

#endif //VK_GPU_OBJECTS_H
