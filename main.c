#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>


/* sky color */
const float SKY_COLOR_RED = 0.5f;
const float SKY_COLOR_GREEN = 0.69f;
const float SKY_COLOR_BLUE = 1;
const float SKY_COLOR_ALPHA = 1;

/* world options */
const int STEP_SIZE = 1;
const int WORLD_HEIGHT = 160;


/* adds a block to the world */
static void add_block(int x, int y, int z, int tx, int ty, int tz, char immediate) {

    // TODO: add block
}


/* draws on screen */
static void draw() {

    // fill sky with color
    glClear(GL_COLOR_BUFFER_BIT);
}


/* callback for error events */
static void error_callback(int error, const char* description) {

    // log error
    fputs(description, stderr);
}


/* initializes the world */
static void initializeWorld() {

    // initial y height
    int x, y = 0, z;

    for (x = -WORLD_HEIGHT / 2; x < (WORLD_HEIGHT / 2) + 1; x++) {
        for (z = -WORLD_HEIGHT / 2; z < (WORLD_HEIGHT / 2) + 1; z++) {
            // TODO: create a layer of stone and grass everywhere
        }
    }
}


/* callback for key events */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {

    // if key is escape: close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


/* configures opengl */
static void setup () {
    GLfloat fog_color[4] = {SKY_COLOR_RED, SKY_COLOR_GREEN, SKY_COLOR_BLUE, SKY_COLOR_ALPHA};

    // set background color (sky)
    glClearColor(SKY_COLOR_RED, SKY_COLOR_GREEN, SKY_COLOR_BLUE, SKY_COLOR_ALPHA);

    // enable culling (meaning that the program won't render unseen polygons)
    glEnable(GL_CULL_FACE);

    // set texture min/magnification filters to nearest
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    // enable fog
    glEnable(GL_FOG);

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
int main(void) {

    // define window
    GLFWwindow* window;

    // set error callback
    glfwSetErrorCallback(error_callback);

    // try to initialize glfw, abort on failure
    if (!glfwInit())
        exit(EXIT_FAILURE);

    // create window
    window = glfwCreateWindow(640, 480, "mainCraft", NULL, NULL);

    // if failed to create window: abort with failure
    if (!window)
    {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    // set window as context
    glfwMakeContextCurrent(window);

    // define buffer swap interval
    glfwSwapInterval(1);

    // set key callback
    glfwSetKeyCallback(window, key_callback);

    // configure opengl preferences
    setup();

    // initialize world
    initializeWorld();

    // main program loop
    while (!glfwWindowShouldClose(window))
    {
        int width, height;

        // handle screen resize
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);

        // draw
        draw();

        // swap buffers
        glfwSwapBuffers(window);

        // handle events
        glfwPollEvents();
    }

    // destroy window
    glfwDestroyWindow(window);

    // terminate glfw
    glfwTerminate();

    // exit with success
    exit(EXIT_SUCCESS);
}
