#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#include "vk_logical_device.h"

// Extentions necessary by this program
const char *device_extensions[1] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

int
find_queue_families(VkPhysicalDevice physical_device, struct vk_queues *queues, VkSurfaceKHR surface)
{
	uint32_t i, queue_queue_count = 0;
	VkQueueFamilyProperties *queue_family;
	VkBool32 present_support = false;

	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_queue_count, NULL);

	queue_family = malloc(sizeof(VkQueueFamilyProperties) * queues_count);
	if (!queue_family) {
		fprintf(stderr, "Error: Failed to allocate queue_family vector!\n");
		return -1;
	}

	vkGetPhysicalDeviceQueueFamilyProperties(physical_device, &queue_queue_count, queue_family);

	for (i = 0; i < queue_queue_count; i++) {
		vkGetPhysicalDeviceSurfaceSupportKHR(physical_device, i, surface, &present_support);

		/* Verify if our gpu has a presentation family_indices queue
		 * Notice this queue is a extention, so in the vulkan core
		 * this isn't necessary
		 * */
		if (present_support && queue_family[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queues->family_indices[present]= i;
			queues->queue_count[present] = queue_family[i].queueCount;
		} if (queue_family[i].queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			queues->family_indices[graphics] = i;
			queues->queue_count[graphics] = queue_family[i].queueCount;
		} else if (queue_family[i].queueFlags & VK_QUEUE_TRANSFER_BIT) {
			queues->family_indices[transfer] = i;
			queues->queue_count[transfer] = queue_family[i].queueCount;
		} else if (queue_family[i].queueFlags & VK_QUEUE_COMPUTE_BIT) {
			queues->family_indices[compute] = i;
			queues->queue_count[compute] = queue_family[i].queueCount;
		} else if (queue_family[i].queueFlags & VK_QUEUE_PROTECTED_BIT) {
			queues->family_indices[protectedBit] = i;
			queues->queue_count[protectedBit] = queue_family[i].queueCount;
		} else if (queue_family[i].queueFlags & VK_QUEUE_SPARSE_BINDING_BIT) {
			queues->family_indices[sparseBindingBit] = i;
			queues->queue_count[sparseBindingBit] = queue_family[i].queueCount;
		}
	}

	free(queue_family);

	return 0;
}

bool
check_device_extension_support(VkPhysicalDevice physical_device)
{
	VkExtensionProperties *available_extensions;
	uint32_t extension_count, i, j, available_in_device = 0;
	bool ret = true;

	vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, NULL);
	available_extensions = malloc(sizeof(VkExtensionProperties) * extension_count);
	if (!available_extensions) {
		fprintf(stderr, "Error: failed while probing device extentions\n");
		ret = false;
		goto return_value;
	}

	vkEnumerateDeviceExtensionProperties(physical_device, NULL, &extension_count, available_extensions);

	for (i = 0; i < extension_count; i++)
		for (j = 0; j < array_size(device_extensions); j++)
			if (!strcmp(available_extensions[i].extensionName, device_extensions[j]))
					available_in_device++;

	/* Check if all the necessary extentions are available in the device */
	if (available_in_device != array_size(device_extensions))
		ret = false;

	free(available_extensions);
return_value:
	return ret;
}

bool
is_device_suitable(VkPhysicalDevice physical_device, struct vk_device *picked_device, VkSurfaceKHR surface)
{
	bool extensions_supported = check_device_extension_support(physical_device);
	struct vk_queues queues = { };

	if (!extensions_supported)
		goto return_false;

	if (find_queue_families(physical_device, &queues, surface))
		goto return_false;

	if (!queues.queue_count[graphics] || !queues.queue_count[present])
		goto return_false;

	picked_device->physical_device = physical_device;
	picked_device->queues = queues;

	return true;

return_false:
	return false;
}

int
pick_physical_device(VkInstance instance, struct vk_device *device, VkSurfaceKHR surface)
{
	uint32_t device_count = 0, i;
	VkPhysicalDevice *devices;
	int ret = -1;

	vkEnumeratePhysicalDevices(instance, &device_count, NULL);
	if (device_count == 0) {
		fprintf(stderr, "Error: failed to find GPUs with Vulkan support!\n");
		goto return_error;
	}

	devices = malloc(sizeof(VkPhysicalDevice) * device_count);
	if (!devices) {
		fprintf(stderr, "Error: failed while allocating physical device vector!\n");
		goto return_error;
	}

	vkEnumeratePhysicalDevices(instance, &device_count, devices);

	for (i = 0; i < device_count; i++)
		if (is_device_suitable(devices[i], device, surface)) {
			ret = 0;
			break;
		}

	if (ret)
		fprintf(stderr, "Error: Failed to find a suitable GPU!\n");

	free(devices);
return_error:
	return ret;
}
