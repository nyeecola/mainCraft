#ifndef VK_LOGICAL_DEVICE_H
#define VK_LOGICAL_DEVICE_H

#include <vulkan/vulkan.h>
#include <stdbool.h>

#include "vk_types.h"
#include "utils.h"

int
find_queue_families(VkPhysicalDevice physical_device, struct vk_queues *queues, VkSurfaceKHR surface);

bool
check_device_extension_support(VkPhysicalDevice physical_device);

bool
is_device_suitable(VkPhysicalDevice physical_device, struct vk_device *picked_device, VkSurfaceKHR surface);

int
pick_physical_device(VkInstance instance, struct vk_device *device, VkSurfaceKHR surface);

#endif //VK_LOGICAL_DEVICE_H
