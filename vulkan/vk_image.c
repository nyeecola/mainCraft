#include "vk_command_buffer.h"
#include "vk_buffer.h"
#include "vk_image.h"
#include "utils.h"

VkImageView
create_image_view(VkDevice logical_device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags)
{
	VkImageView image_view;
	VkResult result;

	VkImageViewCreateInfo view_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO,
		.image = image,
		.viewType = VK_IMAGE_VIEW_TYPE_2D,
		.format = format,
		.components.r = VK_COMPONENT_SWIZZLE_IDENTITY,
		.components.g = VK_COMPONENT_SWIZZLE_IDENTITY,
		.components.b = VK_COMPONENT_SWIZZLE_IDENTITY,
		.components.a = VK_COMPONENT_SWIZZLE_IDENTITY,
		.subresourceRange.aspectMask = aspect_flags,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1
	};

	result = vkCreateImageView(logical_device, &view_info, NULL, &image_view);
	if (result != VK_SUCCESS) {
		print_error("Failed to create texture image view!");
		return VK_NULL_HANDLE;
	}

	return image_view;
}

void
image_views_cleanup(VkDevice logical_device, VkImageView *image_views, uint32_t images_count)
{
	int i;

	for (i = 0; i < images_count && image_views[i]; i++)
		vkDestroyImageView(logical_device, image_views[i], NULL);

	free(image_views);
}

int
create_image(struct vk_device *dev, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* image_memory)
{
	VkMemoryRequirements mem_requirements;
	int64_t mem_type;
	VkResult result;

	VkImageCreateInfo image_info = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO,
		.imageType = VK_IMAGE_TYPE_2D,
		.extent.width = width,
		.extent.height = height,
		.extent.depth = 1,
		.mipLevels = 1,
		.arrayLayers = 1,
		.format = format,
		.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED,
		.usage = usage,
		.samples = VK_SAMPLE_COUNT_1_BIT,
	};

	if (dev->cmd_submission.cmd_buffers_count[transfer] > 0)
		/* It will be used by graphics and transfer command buffers */
		image_info.sharingMode = VK_SHARING_MODE_CONCURRENT;
	else
		image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

	result = vkCreateImage(dev->logical_device, &image_info, NULL, image);
	if (result != VK_SUCCESS) {
		print_error("failed to create image!");
		goto return_error;
	}

	vkGetImageMemoryRequirements(dev->logical_device, *image, &mem_requirements);

	mem_type = find_memory_type(dev->physical_device, mem_requirements.memoryTypeBits, properties);
	if (mem_type == -1) {
		print_error("Failed to find suitable memory type!");
		goto destroy_image;
	}

	VkMemoryAllocateInfo alloc_info = {
		.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO,
		.allocationSize = mem_requirements.size,
		.memoryTypeIndex = (uint32_t) mem_type
	};

	result = vkAllocateMemory(dev->logical_device, &alloc_info, NULL, image_memory);
	if (result != VK_SUCCESS) {
		print_error("failed to allocate image memory!");
		goto destroy_image;
	}

	result = vkBindImageMemory(dev->logical_device, *image, *image_memory, 0);
	if (result != VK_SUCCESS) {
		print_error("failed to allocate image memory!");
		goto destroy_image_memory;
	}

	return 0;

destroy_image_memory:
	vkFreeMemory(dev->logical_device, *image_memory, NULL);
destroy_image:
	vkDestroyImage(dev->logical_device, *image, NULL);
return_error:
	return -1;
}

int
transition_image_layout(struct vk_cmd_submission *cmd_sub, VkImage image, VkFormat format,
						VkImageLayout old_layout, VkImageLayout new_layout)
{
	VkPipelineStageFlags src_stage;
	VkPipelineStageFlags dst_stage;
	VkCommandBuffer cmd_buffer;
	VkResult result;
	VkQueue queue;

	VkImageMemoryBarrier barrier = {
		.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER,
		.oldLayout = old_layout,
		.newLayout = new_layout,
		.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED,
		.image = image,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.subresourceRange.baseMipLevel = 0,
		.subresourceRange.levelCount = 1,
		.subresourceRange.baseArrayLayer = 0,
		.subresourceRange.layerCount = 1,
		.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT
	};

	if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

		src_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		dst_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	} else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
			   new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		src_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		dst_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	} else {
		print_error("Unsupported layout transition!");
		return -1;
	}

	if (cmd_sub->cmd_buffers_count[transfer] > 0) {
		cmd_buffer = cmd_sub->cmd_buffers[transfer][0];
		queue = cmd_sub->queue_handles[transfer];
	} else {
		cmd_buffer = cmd_sub->cmd_buffers[graphics][0];
		queue = cmd_sub->queue_handles[graphics];
	}

	result = begin_single_time_commands(cmd_buffer);
	if (result != VK_SUCCESS)
		return -1;

	vkCmdPipelineBarrier(cmd_buffer, src_stage, dst_stage, 0, 0, NULL, 0, NULL, 1, &barrier);

	result = end_single_time_commands(cmd_buffer, queue);
	if (result != VK_SUCCESS)
		return -1;

	return 0;
}

int
copy_buffer_to_image(struct vk_cmd_submission *cmd_sub, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	VkCommandBuffer cmd_buffer;
	VkResult result;
	VkQueue queue;

	VkBufferImageCopy region = {
		.bufferOffset = 0,
		.bufferRowLength = 0,
		.bufferImageHeight = 0,
		.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT,
		.imageSubresource.mipLevel = 0,
		.imageSubresource.baseArrayLayer = 0,
		.imageSubresource.layerCount = 1,
		.imageOffset = { 0, 0, 0 },
		.imageExtent = { width, height, 1 }
	};

	if (cmd_sub->cmd_buffers_count[transfer] > 0) {
		cmd_buffer = cmd_sub->cmd_buffers[transfer][0];
		queue = cmd_sub->queue_handles[transfer];
	} else {
		cmd_buffer = cmd_sub->cmd_buffers[graphics][0];
		queue = cmd_sub->queue_handles[graphics];
	}

	result = begin_single_time_commands(cmd_buffer);
	if (result != VK_SUCCESS)
		return -1;

	vkCmdCopyBufferToImage(cmd_buffer, buffer, image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, 1, &region);

	result = end_single_time_commands(cmd_buffer, queue);
	if (result != VK_SUCCESS)
		return -1;

	return 0;
}
