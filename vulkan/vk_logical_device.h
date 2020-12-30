#ifndef VK_LOGICAL_DEVICE_H
#define VK_LOGICAL_DEVICE_H

#include <vulkan/vulkan.h>
#include <stdbool.h>

#include "vk_types.h"


int
find_queue_families(VkPhysicalDevice physical_device, struct vk_queues *queues, VkSurfaceKHR surface);

bool
check_device_extension_support(VkPhysicalDevice physical_device);

int
query_surface_support(VkPhysicalDevice physical_device, VkSurfaceKHR surface, struct surface_support *surface_support);

bool
is_device_suitable(VkPhysicalDevice physical_device, struct vk_device *picked_device, VkSurfaceKHR surface);

int
pick_physical_device(VkInstance instance, struct vk_device *device, VkSurfaceKHR surface);

int
create_logical_device(struct vk_device *device);

void
surface_support_cleanup(struct surface_support *surface_support);

#endif //VK_LOGICAL_DEVICE_H
