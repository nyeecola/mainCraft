#ifndef VK_RENDER_H
#define VK_RENDER_H

#include <vulkan/vulkan.h>
#include "vk_types.h"

VkRenderPass
create_render_pass(VkDevice logical_device, VkFormat depth_format, struct swapchain_info state);

int
create_graphics_pipeline(const VkDevice logical_device, struct swapchain_info *swapchain_info, struct vk_render *render);

int
create_framebuffers(const VkDevice logical_device, struct vk_swapchain *swapchain, struct vk_render *render);

void
framebuffers_cleanup(const VkDevice logical_device, VkFramebuffer *framebuffers, uint32_t size);

int
create_depth_resources(struct vk_device *dev, struct vk_render *render, VkExtent2D swapchain_extent);

#endif //VK_RENDER_H
