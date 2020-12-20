#ifndef VK_CONSTANTS_H
#define VK_CONSTANTS_H

#include <vulkan/vulkan.h>


#ifdef ENABLE_VALIDATION_LAYERS
#define enable_validation_layers true
#else
#define enable_validation_layers false
#endif

extern const char *validation_layers[1];
extern const char *device_extensions[1];

#endif //VK_CONSTANTS_H
