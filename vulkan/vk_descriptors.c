#include <stddef.h>
#include <stdlib.h>

#include "vk_descriptors.h"
#include "game_objects.h"

VkVertexInputBindingDescription
get_vertex_binding_description(uint32_t binding)
{
	VkVertexInputBindingDescription binding_description = {
		.binding = binding,
		.stride = sizeof(struct vertex),
		.inputRate = VK_VERTEX_INPUT_RATE_VERTEX
	};

	return binding_description;
}

VkVertexInputAttributeDescription *
get_vertex_attribute_descriptions(uint32_t binding, uint32_t first_location)
{
	VkVertexInputAttributeDescription *attribute_descriptions;

	attribute_descriptions = malloc(sizeof(VkVertexInputAttributeDescription) * 2);
	if (!attribute_descriptions)
		return NULL;

	attribute_descriptions[0].binding = binding;
	attribute_descriptions[0].location = first_location;
	attribute_descriptions[0].format = VK_FORMAT_R32G32_SFLOAT;
	attribute_descriptions[0].offset = offsetof(struct vertex, pos);

	attribute_descriptions[1].binding = binding;
	attribute_descriptions[1].location = first_location + 1;
	attribute_descriptions[1].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descriptions[1].offset = offsetof(struct vertex, color);

	return attribute_descriptions;
}

