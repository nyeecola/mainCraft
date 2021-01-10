#ifndef VK_IMAGE_VIEW_H
#define VK_IMAGE_VIEW_H

#include "vk_types.h"

VkImageView
create_image_view(VkDevice logical_device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);

void
image_views_cleanup(VkDevice logical_device, VkImageView *image_views, uint32_t images_count);

int
create_image(struct vk_device *dev, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling,
			VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage* image, VkDeviceMemory* image_memory);

int
transition_image_layout(struct vk_cmd_submission *cmd_sub, VkImage image, VkFormat format,
						VkImageLayout old_layout, VkImageLayout new_layout);

int
copy_buffer_to_image(struct vk_cmd_submission *cmd_sub, VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

#endif //VK_IMAGE_VIEW_H
