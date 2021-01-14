#ifndef PLAYER_VIEW_H
#define PLAYER_VIEW_H

#include <GLFW/glfw3.h>
#include <cglm/cglm.h>

#include "types.h"

/* The Y variable exist because Vulkan has a inverted Y axis compared to OpenGL */
void
update_position_and_view(GLFWwindow *window, struct input *input, struct game_data *game, mat4 view, float y);

void
update_projection(mat4 projection, float FoV, uint32_t width, uint32_t height, float y);

#endif //PLAYER_VIEW_H
