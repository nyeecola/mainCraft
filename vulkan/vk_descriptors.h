#ifndef VK_VERTEX_BUFFER_H
#define VK_VERTEX_BUFFER_H

#include <vulkan/vulkan.h>

#include "vk_types.h"

void
get_vertex_binding_description(uint32_t binding, VkVertexInputBindingDescription *binding_description);

void
get_vertex_attribute_descriptions(uint32_t binding, uint32_t first_location,
								  VkVertexInputAttributeDescription *attribute_descriptions);

VkDescriptorSetLayout
create_descriptor_set_layout_binding(VkDevice logical_device);

VkDescriptorPool
create_descriptor_pool(VkDevice logical_device, struct vk_swapchain *swapchain);

int
create_descriptor_sets(struct vk_device *dev, struct vk_cmd_submission *cmd_sub,
					   VkDescriptorSetLayout descriptor_set_layout);

#endif //VK_VERTEX_BUFFER_H
