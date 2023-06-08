#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
/*
class window {
public:
	static window& get_instance() {
		static window instance;
		return instance;
	}

	window(const window&) = delete;
	void operator=(const window&) = delete;

	static void init() { get_instance()._init(); }
	static void shutdown() { get_instance()._shutdown(); }
	
	static unsigned int get_width() { return get_instance()._get_width(); }
	static unsigned int get_height() { return get_instance()._get_height(); }
	static GLFWwindow* get_native_window() { return get_instance()._get_native_window(); }

private:
	window() {}
	void _init();
	void _shutdown();
	unsigned int _get_width() const;
	unsigned int _get_height() const;
	GLFWwindow* _get_native_window() const;

	GLFWwindow* _window = nullptr;
	const char* _title = "";
	unsigned int _width = 0;
	unsigned int _height = 0;
};
*/

