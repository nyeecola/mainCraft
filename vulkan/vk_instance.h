#ifndef VK_INSTANCE_H
#define VK_INSTANCE_H

#include <vulkan/vulkan.h>

#include "vk_types.h"


VkInstance
create_instance(VkApplicationInfo *app_info);

VkApplicationInfo
create_app_info();

#endif //VK_INSTANCE_H
