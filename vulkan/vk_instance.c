#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "vk_instance.h"
#include "vk_constants.h"
#include "utils.h"

/* This function check if all validation layers are available and return a boolean */
bool checkValidationLayerSupport() {
	VkLayerProperties *available_layers;
	uint32_t layer_count, i, j;
	bool layer_found, ret = false;
	VkResult result;

	result = vkEnumerateInstanceLayerProperties(&layer_count, NULL);
	if (result != VK_SUCCESS) {
		print_error("Failed to enumerate instace layer properties!");
		goto return_false;
	}

	available_layers = malloc(sizeof(VkLayerProperties) * layer_count);
	if (!available_layers) {
		print_error("Failed while quering Vulkan layers!");
		goto return_false;
	}

	result = vkEnumerateInstanceLayerProperties(&layer_count, available_layers);
	if (result != VK_SUCCESS) {
		print_error("Failed to enumerate instace layer properties!");
		goto free_available_layers_vector;
	}

	ret = true;
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

free_available_layers_vector:
	free(available_layers);
return_false:
	return ret;
}

VkInstance
create_instance(VkApplicationInfo *app_info)
{
	uint32_t glfw_extension_count;
	const char **glfw_extensions;
	VkInstance instance;
	VkResult result;

	if (enable_validation_layers && !checkValidationLayerSupport()) {
		print_error("Not all validation layers requested are available!");
		return VK_NULL_HANDLE;
	}

	VkInstanceCreateInfo create_info = {
		.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
		.pApplicationInfo = app_info,
	};

	glfw_extensions = glfwGetRequiredInstanceExtensions(&glfw_extension_count);
	if (!glfw_extensions) {
		print_error("Failed to get glfw required extentions!");
		return VK_NULL_HANDLE;
	}

	create_info.enabledExtensionCount = glfw_extension_count;
	create_info.ppEnabledExtensionNames = glfw_extensions;

	if (enable_validation_layers) {
		create_info.enabledLayerCount = array_size(validation_layers);
		create_info.ppEnabledLayerNames = validation_layers;
	} else
		create_info.enabledLayerCount = 0;

	result = vkCreateInstance(&create_info, NULL, &instance);
	if (result != VK_SUCCESS) {
		print_error("Failed to create a Vulkan instance!");
		return VK_NULL_HANDLE;
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

