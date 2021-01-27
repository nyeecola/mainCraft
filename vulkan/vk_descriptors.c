#include <stddef.h>
#include <stdlib.h>

#include "vk_descriptors.h"
#include "game_objects.h"
#include "utils.h"

void
get_vertex_binding_description(uint32_t binding, VkVertexInputBindingDescription *binding_description)
{
	binding_description[0].binding = binding;
	binding_description[0].stride = sizeof(struct vertex);
	binding_description[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
}

void
get_vertex_attribute_descriptions(uint32_t binding, uint32_t first_location,
								  VkVertexInputAttributeDescription *attribute_descriptions)
{
	attribute_descriptions[0].binding = binding;
	attribute_descriptions[0].location = first_location;
	attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descriptions[0].offset = offsetof(struct vertex, pos);

	attribute_descriptions[1].binding = binding;
	attribute_descriptions[1].location = first_location + 1;
	attribute_descriptions[1].format = VK_FORMAT_R32G32_SFLOAT;
	attribute_descriptions[1].offset = offsetof(struct vertex, texCoord);
}

void
get_vec3_binding_description(uint32_t binding, VkVertexInputRate input_rate,
							 VkVertexInputBindingDescription *binding_description)
{
	binding_description[0].binding = binding;
	binding_description[0].stride = sizeof(vec3);
	binding_description[0].inputRate = input_rate;
}

void
get_vec3_attribute_descriptions(uint32_t binding, uint32_t first_location,
								VkVertexInputAttributeDescription *attribute_descriptions)
{
	attribute_descriptions[0].binding = binding;
	attribute_descriptions[0].location = first_location;
	attribute_descriptions[0].format = VK_FORMAT_R32G32B32_SFLOAT;
	attribute_descriptions[0].offset = 0;
}

VkDescriptorSetLayout
create_descriptor_set_layout_binding(VkDevice logical_device, uint32_t cube_texture_count)
{
	VkDescriptorSetLayout descriptor_set_layout;
	VkResult result;

	VkDescriptorSetLayoutBinding bindings[] = {
		(VkDescriptorSetLayoutBinding) {
			.binding = 0,
			.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = 1,
			.stageFlags = VK_SHADER_STAGE_VERTEX_BIT
		},
		(VkDescriptorSetLayoutBinding) {
			.binding = 1,
			.descriptorCount = 1,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
			.pImmutableSamplers = NULL,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
		},
		(VkDescriptorSetLayoutBinding) {
			.binding = 2,
			.descriptorCount = cube_texture_count,
			.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.pImmutableSamplers = NULL,
			.stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
		}
	};

	VkDescriptorSetLayoutCreateInfo layout_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO,
		.bindingCount = array_size(bindings),
		.pBindings = bindings
	};

	result = vkCreateDescriptorSetLayout(logical_device, &layout_info, NULL, &descriptor_set_layout);
	if (result != VK_SUCCESS) {
		print_error("Failed to create descriptor set layout!");
		return VK_NULL_HANDLE;
	}

	return descriptor_set_layout;
}

VkDescriptorPool
create_descriptor_pool(VkDevice logical_device, struct vk_swapchain *swapchain, uint32_t texture_count)
{
	VkDescriptorPool descriptor_pool;
	VkResult result;

	VkDescriptorPoolSize pool_sizes[] = {
		(VkDescriptorPoolSize) {
			.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
			.descriptorCount = swapchain->images_count
		},
		(VkDescriptorPoolSize) {
			.type = VK_DESCRIPTOR_TYPE_SAMPLER,
			.descriptorCount = swapchain->images_count
		},
		(VkDescriptorPoolSize) {
			.type = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
			.descriptorCount = swapchain->images_count * texture_count
		}
	};

	VkDescriptorPoolCreateInfo pool_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO,
		.poolSizeCount = array_size(pool_sizes),
		.pPoolSizes = pool_sizes,
		/* "Maximum number of descriptor sets that can be allocated from the pool" */
		.maxSets = swapchain->images_count
	};

	result = vkCreateDescriptorPool(logical_device, &pool_info, NULL, &descriptor_pool);
	if (result != VK_SUCCESS) {
		print_error("Failed to create descriptor pool!");
		return VK_NULL_HANDLE;
	}

	return descriptor_pool;
}

int
create_descriptor_sets(struct vk_device *dev, struct vk_cmd_submission *cmd_sub,
					   VkDescriptorSetLayout descriptor_set_layout)
{
	uint32_t swapchain_images_size = dev->swapchain.images_count;
	VkDescriptorSetLayout layouts[swapchain_images_size];
	struct view_projection *camera = &dev->game_objs.camera;
	struct vk_vertex_object *cube = &dev->game_objs.cube;
	VkDescriptorImageInfo image_info[cube->texture_count];
	VkDescriptorSet *descriptor_sets;
	VkResult result;
	size_t i;

	for (i = 0; i < swapchain_images_size; i++)
		layouts[i] = descriptor_set_layout;

	VkDescriptorSetAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO,
		.descriptorPool = cmd_sub->descriptor_pool,
		.descriptorSetCount = swapchain_images_size,
		.pSetLayouts = layouts
	};

	descriptor_sets = malloc(sizeof(descriptor_sets) * swapchain_images_size);
	if (!descriptor_sets) {
		print_error("Failed to allocate descriptor sets vector!");
		goto return_error;
	}

	/* We don't need to explicitly clean up descriptor sets, because they
	 * will be automatically freed when the descriptor pool is destroyed
	 * */
	result = vkAllocateDescriptorSets(dev->logical_device, &alloc_info, descriptor_sets);
	if (result != VK_SUCCESS) {
		print_error("Failed to allocate descriptor sets!");
		goto descriptor_sets_vector;
	}

	for (i = 0; i < cube->texture_count; i++) {
		image_info[i].imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
		image_info[i].imageView = cube->texture_images_view[i];
		image_info[i].sampler = VK_NULL_HANDLE;
	}

	VkDescriptorImageInfo sampler_info = {
		.sampler = dev->render.texture_sampler
	};

	for (i = 0; i < swapchain_images_size; i++) {
		VkDescriptorBufferInfo buffer_info = {
			.buffer = camera->buffers[i],
			.offset = 0,
			.range = sizeof(mat4),
		};

		VkWriteDescriptorSet descriptor_writes[] = {
			(VkWriteDescriptorSet) {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				// The descriptor set that we will write into
				.dstSet = descriptor_sets[i],
				.dstBinding = 0,
				.dstArrayElement = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER,
				.descriptorCount = 1,
				.pBufferInfo = &buffer_info
			},
			(VkWriteDescriptorSet) {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = descriptor_sets[i],
				.dstBinding = 1,
				.dstArrayElement = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLER,
				.descriptorCount = 1,
				.pImageInfo = &sampler_info
			},
			(VkWriteDescriptorSet) {
				.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET,
				.dstSet = descriptor_sets[i],
				.dstBinding = 2,
				.dstArrayElement = 0,
				.descriptorType = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE,
				.descriptorCount = array_size(image_info),
				.pImageInfo = image_info
			}
		};

		vkUpdateDescriptorSets(dev->logical_device, array_size(descriptor_writes), descriptor_writes, 0, NULL);
	}

	cmd_sub->descriptor_sets = descriptor_sets;
	cmd_sub->descriptors_count = swapchain_images_size;

	return 0;

descriptor_sets_vector:
	free(descriptor_sets);
return_error:
	return -1;
}

