#include "input.h"
#include "types.h"

#define DEFINE_STATE(var, action)        \
		if (action == GLFW_PRESS)        \
			var = true;                  \
		else if (action == GLFW_RELEASE) \
			var = false                  \

void
key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
	struct glfw_callback_data *data = glfwGetWindowUserPointer(window);
	struct input *input = data->input;

	switch (key) {
	case GLFW_KEY_ESCAPE:
		if (action == GLFW_PRESS)
			glfwSetWindowShouldClose(window, GL_TRUE);
		break;
	case GLFW_KEY_A:
		DEFINE_STATE(input->key_pressed[A], action);
		break;
	case GLFW_KEY_W:
		DEFINE_STATE(input->key_pressed[W], action);
		break;
	case GLFW_KEY_S:
		DEFINE_STATE(input->key_pressed[S], action);
		break;
	case GLFW_KEY_D:
		DEFINE_STATE(input->key_pressed[D], action);
		break;
	case GLFW_KEY_SPACE:
		DEFINE_STATE(input->key_pressed[SPACE], action);
		break;
	}
}

