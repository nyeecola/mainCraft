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
