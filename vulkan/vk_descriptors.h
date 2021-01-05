#ifndef VK_VERTEX_BUFFER_H
#define VK_VERTEX_BUFFER_H

#include <vulkan/vulkan.h>

#include "vk_types.h"

VkVertexInputBindingDescription
get_vertex_binding_description(uint32_t binding);

VkVertexInputAttributeDescription *
get_vertex_attribute_descriptions(uint32_t binding, uint32_t first_location);

#endif //VK_VERTEX_BUFFER_H
