#include <stdlib.h>

#include "vk_draw.h"
#include "utils.h"

int
create_sync_objects(VkDevice logical_device, struct vk_draw_sync *sync, uint32_t images_count)
{
	int i;
	sync->images_in_flight = calloc(sizeof(VkFence), images_count);
	if (!sync->images_in_flight) {
		print_error("Failed to allocate images_in_fligth vector");
		return -1;
	}

	VkSemaphoreCreateInfo semaphore_info = {
		.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO
	};

	VkFenceCreateInfo fence_info = {
		.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO,
		/* Create a fence that is already 'unlock', in other words
		 * this fence start will not block anything at beggining */
		.flags = VK_FENCE_CREATE_SIGNALED_BIT
	};

	for (i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (vkCreateSemaphore(logical_device, &semaphore_info, NULL, &sync->image_available_semaphore[i])) {
			print_error("Failed to create semaphores!");
			break;
		}

		if (vkCreateSemaphore(logical_device, &semaphore_info, NULL, &sync->render_finished_semaphore[i])) {
			print_error("Failed to create semaphores!");
			break;
		}

		if (vkCreateFence(logical_device, &fence_info, NULL, &sync->in_flight_fences[i])){
			print_error("Failed to create fences!");
			break;
		}
	}

	if (i != MAX_FRAMES_IN_FLIGHT) {
		sync_objects_cleanup(logical_device, sync);
		return -1;
	}

	return 0;
}

void
sync_objects_cleanup(VkDevice logical_device, struct vk_draw_sync *sync)
{
	int i;

	for (i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
		if (sync->render_finished_semaphore[i])
			vkDestroySemaphore(logical_device, sync->render_finished_semaphore[i], NULL);
		if (sync->image_available_semaphore[i])
			vkDestroySemaphore(logical_device, sync->image_available_semaphore[i], NULL);
		if (sync->in_flight_fences[i])
			vkDestroyFence(logical_device, sync->in_flight_fences[i], NULL);
	}

	free(sync->images_in_flight);
}


