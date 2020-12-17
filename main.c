#include <GLFW/glfw3.h>
#include <getopt.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "gl_backend.h"

#define HELP_MESSAGE \
    "Usage:\tmainCraft.run --backend vulkan\n" \
    "Options:\n" \
    "\t-b,\t--backend\t Selct backend (vulkan or opengl).\n" \
    "\t-h,\t--help\t Show This Message.\n\n" \


enum backend_type { vulkan, opengl };

// Inicialization of long options of opt
#define LONG_OPTIONS \
	{ \
		{"backend", required_argument, NULL, 'b'}, \
		{"help", no_argument, NULL, 'h'}, \
		{0, 0, 0, 0} \
	}

/* main program */
int
main(const int argc, char *const *argv)
{
	int exit_status = EXIT_FAILURE;
	enum backend_type backend = opengl;
	struct option longOptions[] = LONG_OPTIONS;
	int option = 0;

	while ((option = getopt_long(argc, argv, "b:h", longOptions, NULL)) != -1) {
		switch (option){
		case 'b':
			if (!strcmp(optarg, "vulkan"))
				backend = vulkan;
			else if (!strcmp(optarg, "opengl"))
				backend = opengl;
			else {
				fprintf(stderr, "'%s' is no a valid backend\n", optarg);
				goto exit_program;
			}
			break;
		case 'h':
			printf(HELP_MESSAGE);
			exit_status = EXIT_SUCCESS;
			goto exit_program;
		case '?':
			// flush opt message
			fflush(stderr);
			printf("Try --help\n");
			goto exit_program;
		}
	}

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
