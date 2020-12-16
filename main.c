#include <GLFW/glfw3.h>
#include <FTGL/ftgl.h>
#include <stdlib.h>
#include <stdio.h>


/* constants */
const float SKY_COLOR_RED = 0.5f;
const float SKY_COLOR_GREEN = 0.69f;
const float SKY_COLOR_BLUE = 1;
const float SKY_COLOR_ALPHA = 1;
const float SCREEN_WIDTH = 640;
const float SCREEN_HEIGHT = 480;
const float FPS_COOLDOWN = 0.7;


/* FPS counter */
typedef struct FPS_ {
	double timer;
	double last_time;
	double current_time;
	double total_elapsed_time;
	double num_frames;
	char value[40];
} FPS;


/* get FPS */
double
calculateFPS(FPS *fps, double cooldown)
{
	fps->current_time = glfwGetTime();
	double time_elapsed = fps->current_time - fps->last_time;
	fps->last_time = fps->current_time;

	fps->timer -= time_elapsed;
	fps->total_elapsed_time += time_elapsed;
	fps->num_frames++;

	if (fps->timer <= 0) {
		sprintf(fps->value, "%.02f", (float) (1 / (fps->total_elapsed_time / fps->num_frames)));
		fps->timer = cooldown;
		fps->total_elapsed_time = 0;
		fps->num_frames = 0;
	}
}


/* draws on screen */
static
void draw()
{
	// set background color (sky)
	glClearColor(SKY_COLOR_RED, SKY_COLOR_GREEN, SKY_COLOR_BLUE, SKY_COLOR_ALPHA);

	// fill sky with color
	glClear(GL_COLOR_BUFFER_BIT);
}


/* callback for error events */
static void
error_callback(int error, const char* description)
{
	// log error
	fputs(description, stderr);
}


/* callback for key events */
static void
key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	// if key is escape: close window
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
}


/* configures opengl */
static
void setup()
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
}


/* main program */
int
main(void)
{
	int status = EXIT_FAILURE;

	// create and initialize FPS counter data
	FPS fps = {};

	// define window
	GLFWwindow* window;

	// set error callback
	glfwSetErrorCallback(error_callback);

	// try to initialize glfw, abort on failure
	if (!glfwInit())
		goto exit_program;

	// create window
	window = glfwCreateWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "mainCraft", NULL, NULL);
	if (!window)
		goto terminate_glfw;

	// set window as context
	glfwMakeContextCurrent(window);

	// define vsync (1 = on, 0 = off)
	glfwSwapInterval(0);

	// set key callback
	if(glfwSetKeyCallback(window, key_callback))
		goto destroy_window;

	// configure opengl preferences
	setup();

	//Create a pixmap font from a TrueType file
	FTGLfont* FPSfont = ftglCreatePixmapFont("./assets/fonts/Growly_Grin.ttf");
	if (!FPSfont)
		goto destroy_window;

	//Set size of font
	if (!ftglSetFontFaceSize(FPSfont, 50, 0))
		goto destroy_font;


	// main program loop
	while (!glfwWindowShouldClose(window)) {
		int width, height;

		// calculate fps
		calculateFPS(&fps, FPS_COOLDOWN);

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
		printf("%d\n", height);
		glRasterPos2f(-width / 2, height / 2 - 40);
		ftglRenderFont(FPSfont, fps.value, FTGL_RENDER_ALL);

		// swap buffers
		glfwSwapBuffers(window);

		// handle events
		glfwPollEvents();
	}

	// exit with success
	status = EXIT_SUCCESS;

destroy_font:
	ftglDestroyFont(FPSfont);
destroy_window:
	glfwDestroyWindow(window);
terminate_glfw:
	glfwTerminate();
exit_program:
	exit(status);
}
