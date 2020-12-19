#include "extras.h"

void
calculate_FPS(struct FPS *fps, double cooldown)
{
	fps->current_time = glfwGetTime();
	double time_elapsed = fps->current_time - fps->last_time;
	fps->last_time = fps->current_time;

	fps->timer -= time_elapsed;
	fps->total_elapsed_time += time_elapsed;
	fps->num_frames++;

	if (fps->timer <= 0) {
		sprintf(fps->value, "%.02f fps", (float) (1 / (fps->total_elapsed_time / fps->num_frames)));
		fps->timer = cooldown;
		fps->total_elapsed_time = 0;
		fps->num_frames = 0;
	}
}
