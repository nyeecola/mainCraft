#ifndef VK_TEXTURE_H
#define VK_TEXTURE_H

#include <stb/stb_image.h>

#include "vk_types.h"


int
create_texture_image(struct vk_device *dev, char *image_name,
					 VkImage *texture_image, VkDeviceMemory *texture_image_memory);

VkImageView
create_texture_image_view(VkDevice logical_device, VkImage texture_image);

VkSampler
create_texture_sampler(VkDevice logical_device, VkPhysicalDeviceProperties *device_properties);

#endif //VK_TEXTURE_H
