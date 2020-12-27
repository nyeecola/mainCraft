#ifndef VK_RENDER_H
#define VK_RENDER_H

#include <vulkan/vulkan.h>
#include "vk_types.h"

VkRenderPass
create_render_pass(VkDevice logical_device, struct swapchain_info state);

#endif //VK_RENDER_H
