#ifndef VK_RESOURCE_MANAGER_H
#define VK_RESOURCE_MANAGER_H

#include <vulkan/vulkan.h>
#include "vk_types.h"

int
create_render_and_presentation_infra(struct vk_program *program);

void
destroy_render_and_presentation_infra(struct vk_device *dev);

int
recreate_render_and_presentation_infra(struct vk_program *program);

int
init_vk(struct vk_program *program);

void
vk_cleanup(struct vk_program *program);

#endif //VK_RESOURCE_MANAGER_H
