#ifndef VK_COMMAND_BUFFER_H
#define VK_COMMAND_BUFFER_H

#include <vulkan/vulkan.h>

#include "vk_types.h"

VkCommandPool
create_command_pool(VkDevice logical_device, uint32_t family_index, VkCommandPoolCreateFlags flags);

VkCommandBuffer *
create_command_buffers(VkDevice logical_device, VkCommandPool pool, VkCommandBufferLevel level, uint32_t count);

void
cleanup_command_pools(VkDevice logical_device, VkCommandPool command_pools[]);

void
free_command_buffer_vector(VkCommandBuffer *cmd_buffers[]);

int
create_cmd_submission_infra(struct vk_device *device);

int
record_draw_cmd(struct vk_cmd_submission *cmd_sub, struct vk_swapchain *swapchain,
				struct vk_render *render, struct vk_game_objects *game_objects);

VkResult
begin_single_time_commands(VkCommandBuffer cmd_buffer);

VkResult
end_single_time_commands(VkCommandBuffer cmd_buffer, VkQueue queue);

#endif //VK_COMMAND_BUFFER_H
