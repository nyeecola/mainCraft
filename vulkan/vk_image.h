#ifndef VK_IMAGE_VIEW_H
#define VK_IMAGE_VIEW_H

#include "vk_types.h"

VkImageView
create_image_view(VkDevice logical_device, VkImage image, VkFormat format, VkImageAspectFlags aspect_flags);

void
image_views_cleanup(VkDevice logical_device, VkImageView *image_views, uint32_t images_count);

#endif //VK_IMAGE_VIEW_H
