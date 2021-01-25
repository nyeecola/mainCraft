#include "vk_constants.h"

const char *validation_layers[1] = {
	"VK_LAYER_KHRONOS_validation"
};

// Extentions necessary by this program
const char *device_extensions[1] = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const VkFormat depth_buffer_formats[3] = {
	VK_FORMAT_D32_SFLOAT,
	VK_FORMAT_D32_SFLOAT_S8_UINT,
	VK_FORMAT_D24_UNORM_S8_UINT
};

