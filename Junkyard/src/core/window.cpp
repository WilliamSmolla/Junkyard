#include "pch.h"
#include "window.h"
#include "events.h"
#include "application.h"
/*
void window::_init() {
	_title = "Junkyard";
	_width = 1080;
	_height = 720;

	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	_window = glfwCreateWindow(_width, _height, _title, NULL, NULL);
	glfwMakeContextCurrent(_window);
	glfwSwapInterval(1);

	gladLoadGL();
	glViewport(0, 0, _width, _height);

	glfwSetWindowUserPointer(_window, this);

    glfwSetWindowCloseCallback(_window, [](GLFWwindow* window)
    {
        window_close_event event;
        application::handle_events(event);
    });

    glfwSetFramebufferSizeCallback(_window, [](GLFWwindow* win, int width, int height)
    {
        auto& data = *(window*)glfwGetWindowUserPointer(win);

        window_resize_event event(width, height);
        data._width = width;
        data._height = height;
        application::handle_events(event);
    });

    glfwSetKeyCallback(_window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        switch (action) {
            case GLFW_PRESS: {
                key_pressed_event event(key);
                application::handle_events(event);
                break;
            }
            case GLFW_RELEASE: {
                key_released_event event(key);
                application::handle_events(event);
                break;
            }
            case GLFW_REPEAT: {
                key_pressed_event event(key, true);
                application::handle_events(event);
                break;
            }
        }
    });

	glEnable(GL_DEPTH_TEST);
}

void window::_shutdown() {
	glfwDestroyWindow(_window);
}

unsigned int window::_get_width() const {
    return _width;
}

unsigned int window::_get_height() const {
    return _height;
}

GLFWwindow* window::_get_native_window() const {
	return _window;
}

*/
