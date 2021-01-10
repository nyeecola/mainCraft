#include <string.h>

#include "vk_texture.h"
#include "vk_buffer.h"
#include "vk_image.h"
#include "utils.h"

int
create_texture_image(struct vk_device *dev, char *image_name,
					 VkImage *texture_image, VkDeviceMemory *texture_image_memory)
{
	int tex_width, tex_height, tex_channels, ret = -1;
	VkDeviceMemory local_texture_image_memory;
	VkDeviceMemory staging_buffer_memory;
	VkImage local_texture_image;
	VkBuffer staging_buffer;
	VkDeviceSize image_size;
	VkResult result;
	stbi_uc* pixels;
	void* data;

	pixels = stbi_load(image_name, &tex_width, &tex_height, &tex_channels, STBI_rgb_alpha);
	if (!pixels) {
		print_error("failed to load texture image!");
		goto return_error;
	}

	image_size = tex_width * tex_height * 4;

	ret = create_buffer(dev, image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
						VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, &staging_buffer, &staging_buffer_memory);
	if (ret)
		goto free_image;

	result = vkMapMemory(dev->logical_device, staging_buffer_memory, 0, image_size, 0, &data);
	if (result != VK_SUCCESS) {
		print_error("Failed to map buffer to system memory!");
		goto destoy_staging_buffer;
	}

	memcpy(data, pixels, image_size);
	vkUnmapMemory(dev->logical_device, staging_buffer_memory);

	ret = create_image(dev, tex_width, tex_height, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
					   VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
					   VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, &local_texture_image, &local_texture_image_memory);
	if (ret)
		goto destoy_staging_buffer;

	/* The following steps do:
	 * 1) The transition from a "stbi layout"(vkCmdCopyBufferToImage) to a
	 *    transfer optimized layout.
	 * 2) Copy the the image with this new layout to the a image buffer.
	 * 3) Transition to a layout that is readable to the shader.
	 * The step (1) is necessary because in the step (2) we are using the
	 * vkCmdCopyBufferToImage function. And the step (3) is necessary
	 * to shader access the texture.
	 * */
	ret = transition_image_layout(&dev->cmd_submission, local_texture_image, VK_FORMAT_R8G8B8A8_SRGB,
								  VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	if (ret) {
		vkDestroyImage(dev->logical_device, local_texture_image, NULL);
		vkFreeMemory(dev->logical_device, local_texture_image_memory, NULL);
		goto destoy_staging_buffer;
	}

	ret = copy_buffer_to_image(&dev->cmd_submission, staging_buffer, local_texture_image, tex_width, tex_height);
	if (ret) {
		vkDestroyImage(dev->logical_device, local_texture_image, NULL);
		vkFreeMemory(dev->logical_device, local_texture_image_memory, NULL);
		goto destoy_staging_buffer;
	}

	ret = transition_image_layout(&dev->cmd_submission, local_texture_image, VK_FORMAT_R8G8B8A8_SRGB,
								VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
	if (ret) {
		vkDestroyImage(dev->logical_device, local_texture_image, NULL);
		vkFreeMemory(dev->logical_device, local_texture_image_memory, NULL);
		goto destoy_staging_buffer;
	}

	*texture_image = local_texture_image;
	*texture_image_memory = local_texture_image_memory;

	ret = 0;

destoy_staging_buffer:
	vkDestroyBuffer(dev->logical_device, staging_buffer, NULL);
	vkFreeMemory(dev->logical_device, staging_buffer_memory, NULL);
free_image:
	stbi_image_free(pixels);
return_error:
	return ret;
}

/* Create a ImageView to our texture(from createTextureImage).
 * As the swapchain imageView we cannot access the content of texture
 * directly, we need a imageView to access it.
 * */
VkImageView
create_texture_image_view(VkDevice logical_device, VkImage texture_image)
{
	return create_image_view(logical_device, texture_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT);
}

VkSampler
create_texture_sampler(VkDevice logical_device, VkPhysicalDeviceProperties *device_properties)
{
	VkSampler texture_sampler;
	VkResult result;

	VkSamplerCreateInfo sampler_info = {
		.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO,
		.magFilter = VK_FILTER_LINEAR,
		.minFilter = VK_FILTER_LINEAR,
		.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT,
		.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK,
		.maxAnisotropy = device_properties->limits.maxSamplerAnisotropy,
		.anisotropyEnable = VK_TRUE,
		.unnormalizedCoordinates = VK_FALSE,
		.compareEnable = VK_FALSE,
		.compareOp = VK_COMPARE_OP_ALWAYS,
		.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR,
		.mipLodBias = 0.0f,
		.minLod = 0.0f,
		.maxLod = 0.0f
	};

	result = vkCreateSampler(logical_device, &sampler_info, NULL, &texture_sampler);
	if (result != VK_SUCCESS) {
		print_error("Failed to create texture sampler!");
		return VK_NULL_HANDLE;
	}

	return texture_sampler;
}


