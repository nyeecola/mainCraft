#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>


/* callback for error events */
static void error_callback(int error, const char* description)
{
    // log error
    fputs(description, stderr);
}


/* callback for key events */
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    // if key is escape: close window
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}


/* main program */
int main(void)
{
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

    // main program loop
    while (!glfwWindowShouldClose(window))
    {
        float ratio;
        int width, height;

        // handle screen resize
        glfwGetFramebufferSize(window, &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT);

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
