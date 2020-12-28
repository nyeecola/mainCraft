#ifndef VK_RENDER_H
#define VK_RENDER_H

#include <vulkan/vulkan.h>
#include "vk_types.h"

VkRenderPass
create_render_pass(VkDevice logical_device, struct swapchain_info state);

int
create_graphics_pipeline(const VkDevice logical_device, struct swapchain_info *swapchain_info, struct vk_render *render);

#endif //VK_RENDER_H
