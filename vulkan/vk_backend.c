#include <stdlib.h>

#include "vk_backend.h"
#include "vk_window.h"

int
run_vk(const int argc, char *const *argv) {
	int exit_status = EXIT_FAILURE;
	GLFWwindow *window = vk_init_window();

	if (!window)
		goto exit_program;

	exit_status = EXIT_SUCCESS;

	vk_destroy_window(window);
exit_program:
	return exit_status;
}

