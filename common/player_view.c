#include <math.h>

#include "player_view.h"
#include "constants.h"

void
update_position_and_view(GLFWwindow *window, struct input *input, struct game_data *game, mat4 view, float y)
{
	vec3 normalized_direction, normalized_right, up;
	struct game_configs *configs = &game->configs;
	struct player_info *player = &game->player;
	double current_frame_time;
	float delta_time;

	current_frame_time = glfwGetTime();
	delta_time = current_frame_time - game->last_frame_time;

	glfwGetCursorPos(window, &input->mouse_position[X], &input->mouse_position[Y]);

	player->horizontal_angle = configs->mouse_speed * input->mouse_position[X] * -1.0f;
	player->vertical_angle   = configs->mouse_speed * input->mouse_position[Y];

	// Direction : Spherical coordinates to Cartesian coordinates conversion
	vec3 direction = {
		cos(player->vertical_angle) * sin(player->horizontal_angle),
		sin(player->vertical_angle) * y,
		cos(player->vertical_angle) * cos(player->horizontal_angle)
	};

	// Right vector
	vec3 right = {
		sin(player->horizontal_angle - M_PI_2),
		0,
		cos(player->horizontal_angle - M_PI_2)
	};

	// Up vector
	glm_vec3_cross(right, direction, up);

	glm_vec3_scale(direction, delta_time * PLAYER_MOVEMENT_SPEED, normalized_direction);
	if (input->key_pressed[W])
		glm_vec3_add(player->position, normalized_direction, player->position);
	if (input->key_pressed[S])
		glm_vec3_sub(player->position, normalized_direction, player->position);

	glm_vec3_scale(right, delta_time * PLAYER_MOVEMENT_SPEED, normalized_right);
	if (input->key_pressed[D])
		glm_vec3_add(player->position, normalized_right, player->position);
	if (input->key_pressed[A])
		glm_vec3_sub(player->position, normalized_right, player->position);

	glm_vec3_add(player->position, direction, player->looking_at);
	glm_lookat(player->position, player->looking_at, up, view);

	game->last_frame_time = current_frame_time;
}

void
update_projection(mat4 projection, float FoV, uint32_t width, uint32_t height, float y)
{
	glm_perspective(glm_rad(FoV), width / (float) height, 0.1f, 100.0f, projection);
	projection[1][1] *= y;
}
