#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H

#include <vulkan/vulkan.h>

#include "vk_types.h"

#ifdef ENABLE_VALIDATION_LAYERS
#define enable_validation_layers true
#else
#define enable_validation_layers false
#endif

VkInstance
create_instance(VkApplicationInfo *app_info);

VkApplicationInfo
create_app_info();

#endif //VK_INSTANCE_H
