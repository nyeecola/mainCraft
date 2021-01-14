#include <GLFW/glfw3.h>

#include "constants.h"
#include "game_data.h"

void
init_game_state(struct game_data *game)
{
	struct player_info *player = &game->player;
	struct game_configs *configs = &game->configs;

	configs->mouse_speed = DEFAULT_MOUSE_SPEED;
	configs->FoV = DEFAULT_FOV;

	player->horizontal_angle = INITIAL_HORIZONTAL_ANGLE;
	player->vertical_angle = INITIAL_VERTICAL_ANGLE;
	player->position[0] = PLAYER_INITIAL_POSITION_X;
	player->position[1] = PLAYER_INITIAL_POSITION_Y;
	player->position[2] = PLAYER_INITIAL_POSITION_Z;

	game->last_frame_time = glfwGetTime();
}
