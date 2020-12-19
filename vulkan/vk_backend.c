#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

#include "vk_backend.h"
#include "vk_window.h"
#include "utils.h"

const char *validation_layers[1] = {
	"VK_LAYER_KHRONOS_validation"
};

// Extentions necessary by this program
const char *device_extensions[1] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};


#ifdef ENABLE_VALIDATION_LAYERS
const bool enable_validation_layers = true;
#else
const bool enable_validation_layers = false;
#endif

VkApplicationInfo
create_app_info()
{
	VkApplicationInfo app_info = {
		.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
		.pApplicationName = "Main Craft",
		.applicationVersion = VK_MAKE_VERSION(1, 0, 0),
		.pEngineName = "N/A",
		.engineVersion = VK_MAKE_VERSION(1, 0, 0),
		.apiVersion = VK_API_VERSION_1_0,
	};

	return app_info;
}

/* This function check if all validation layers are available and return a boolean */
bool checkValidationLayerSupport() {
	VkLayerProperties *available_layers;
	uint32_t layer_count, i, j;
	bool layer_found, ret = true;

	vkEnumerateInstanceLayerProperties(&layer_count, NULL);

	available_layers = malloc(sizeof(VkLayerProperties) * layer_count);
	if (!available_layers) {
		fprintf(stderr, "Error: Failed while quering Vulkan layers!\n");
		return false;
	}

	vkEnumerateInstanceLayerProperties(&layer_count, available_layers);

	// Verify if the necessary layers are available by comparing the names
	for (i = 0; i < array_size(validation_layers); i++) {
		layer_found = false;

		for (j = 0; j < layer_count; j++)
			if (!strcmp(validation_layers[i], available_layers[j].layerName)) {
				layer_found = true;
				break;
			}

		if(!layer_found) {
			ret = false;
			break;
		}
	}

	free(available_layers);
	return ret;
}

VkInstance
create_instance(VkApplicationInfo *app_info)
{
	VkInstance instance;
	uint32_t glfw_extension_count;
	const char **glfw_extensions;

	if (enable_validation_layers && !checkValidationLayerSupport())
		fprintf(stderr, "Error: Not all validation layers requested are available!\n");

	VkInstanceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = app_info,
	};

	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	create_info.enabledExtensionCount = glfw_extension_count;
	create_info.ppEnabledExtensionNames = glfw_extensions;

	if (enable_validation_layers) {
		create_info.enabledLayerCount = array_size(validation_layers);
		create_info.ppEnabledLayerNames = validation_layers;
	} else
		create_info.enabledLayerCount = 0;

	if (vkCreateInstance(&create_info, NULL, &instance) != VK_SUCCESS) {
		fprintf(stderr, "Error: Failed to create a Vulkan instance!\n");
		return NULL;
	}

	return instance;
}

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
		goto exit_false;

	if (find_queue_families(physical_device, &queues, surface))
		goto exit_false;

	if (!queues.queue_count[graphics] || !queues.queue_count[present])
		goto exit_false;

	picked_device->physical_device = physical_device;
	picked_device->queues = queues;

	return true;

exit_false:
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

int
init_vk(struct vk_program *program)
{
	program->app_info = create_app_info();
	if ((program->instance = create_instance(&program->app_info)) == NULL)
		goto exit_error;

	if (glfwCreateWindowSurface(program->instance, program->window, NULL, &program->surface) != VK_SUCCESS) {
		fprintf(stderr, "Error: Failed to create a Window surface!\n");
		goto destroy_instance;
	}

	if (pick_physical_device(program->instance, &program->device, program->surface))
		goto destroy_surface;

	return 0;

destroy_surface:
	vkDestroySurfaceKHR(program->instance, program->surface, NULL);
destroy_instance:
	vkDestroyInstance(program->instance, NULL);
exit_error:
	return 1;
}

void
vk_cleanup(struct vk_program program)
{
	vkDestroySurfaceKHR(program.instance, program.surface, NULL);
	vkDestroyInstance(program.instance, NULL);
}

int
run_vk(const int argc, char *const *argv) {
	int exit_status = EXIT_FAILURE;
	struct vk_program program = { };

	program.window = vk_init_window();
	if (!program.window)
		goto exit_program;

	if (init_vk(&program))
		goto destroy_window;

	exit_status = EXIT_SUCCESS;

	vk_cleanup(program);
destroy_window:
	vk_destroy_window(program.window);
exit_program:
	return exit_status;
}

