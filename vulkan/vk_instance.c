#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vk_instance.h"
#include "utils.h"

const char *validation_layers[1] = {
	"VK_LAYER_KHRONOS_validation"
};

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

