#include <GLFW/glfw3.h>
#include <GL/glx.h>

#include "gl_backend.h"
#include "input.h"
#include "extras.h"

/* constants */
const float SKY_COLOR_RED = 0.5f;
const float SKY_COLOR_GREEN = 0.69f;
const float SKY_COLOR_BLUE = 1;
const float SKY_COLOR_ALPHA = 1;
const float SCREEN_WIDTH = 640;
const float SCREEN_HEIGHT = 480;
const float FPS_COOLDOWN = 0.7;

// TODO: Find a better place to these variables
// create and initialize FPS counter data
struct FPS fps = {};
FTGLfont *FPSfont;

/* draws on screen */
void
draw()
{
	// set background color (sky)
	glClearColor(SKY_COLOR_RED, SKY_COLOR_GREEN, SKY_COLOR_BLUE, SKY_COLOR_ALPHA);

	// fill sky with color
	glClear(GL_COLOR_BUFFER_BIT);
}

int
main_loop(GLFWwindow *window)
{
	// main program loop
	while (!glfwWindowShouldClose(window)) {
		int width, height;

		// calculate fps
		calculate_FPS(&fps, FPS_COOLDOWN);

		// TODO: should this really run every frame?
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		glOrtho(-1, 1, -1, 1, -1 , 1);

		// handle screen resize
		glfwGetFramebufferSize(window, &width, &height);
		glViewport(0, 0, width, height);

		// set modelview -- TODO: learn more about this
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glOrtho(-width / 2, width / 2, -height / 2, height / 2, -1 , 1);

		// draw
		draw();

		// display fps at the top left
		glRasterPos2f(-width / 2, height / 2 - 40);
		ftglRenderFont(FPSfont, fps.value, FTGL_RENDER_ALL);

		// swap buffers
		glfwSwapBuffers(window);

		// handle events
		glfwPollEvents();
	}
	return 0;
}

/* configures opengl */
int
setup()
{
	GLfloat fog_color[4] = {SKY_COLOR_RED, SKY_COLOR_GREEN, SKY_COLOR_BLUE, SKY_COLOR_ALPHA};

	// enable culling (meaning that the program won't render unseen polygons)
	glEnable(GL_CULL_FACE);

	// set texture min/magnification filters to nearest
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	// enable fog
	// glEnable(GL_FOG); // TODO: find out why this causes a HUUUUGE fps drop when activated

	// set fog color
	glFogfv(GL_FOG_COLOR, fog_color);

	// set fog hint preference as ``don't care``
	glHint(GL_FOG_HINT, GL_DONT_CARE);

	// specify the equation used to compute the blending factor
	glFogi(GL_FOG_MODE, GL_LINEAR);

	// set fog start and end distance
	glFogf(GL_FOG_START, 20.0f);
	glFogf(GL_FOG_END, 60.0f);

	//Create a pixmap font from a TrueType file
	FPSfont = ftglCreatePixmapFont("./assets/fonts/Growly_Grin.ttf");
	if(!FPSfont)
		goto return_error;

	//Set size of font
	if (!ftglSetFontFaceSize(FPSfont, 50, 0))
		goto destroy_font;

	return 0;

destroy_font:
	ftglDestroyFont(FPSfont);
return_error:
	return -1;
}

GLFWwindow *
init_window()
{
	// define window
	GLFWwindow *window;

	// set error callback
	glfwSetErrorCallback(error_callback);

	// try to initialize glfw, abort on failure
	if (!glfwInit())
		goto return_null;

	// create window
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "Main Craft", NULL, NULL);
	if (!window)
		goto terminate_glfw;

	// set window as context
	glfwMakeContextCurrent(window);

	// define vsync (1 = on, 0 = off)
	glfwSwapInterval(0);

	// set key callback
	if(!glfwSetKeyCallback(window, key_callback))
		return window;

destroy_window:
	glfwDestroyWindow(window);
terminate_glfw:
	glfwTerminate();
return_null:
	return NULL;
}

void
destroy_window(GLFWwindow *window)
{
	glfwDestroyWindow(window);
	glfwTerminate();
}

int
run_gl(const int argc, char *const *argv)
{
	int exit_status = EXIT_FAILURE;
	GLFWwindow *window = init_window();

	if (!window)
		goto exit_program;

	// Setup graphics infrastructure
	setup();

	if (!main_loop(window))
		exit_status = EXIT_SUCCESS;

destroy_window:
	destroy_window(window);
exit_program:
	return exit_status;
}

