#include <GLFW/glfw3.h>
#include <stdlib.h>
#include "gl_backend.h"

/* main program */
int
main(void)
{
	int exit_status = EXIT_FAILURE;

	GLFWwindow *window = init_window();
	if (!window)
		goto destroy_window;

	// Setup graphics infrastructure
	setup();

	if (!main_loop(window))
		exit_status = EXIT_SUCCESS;

destroy_window:
	destroy_window(window);
exit_program:
	exit(exit_status);
}
