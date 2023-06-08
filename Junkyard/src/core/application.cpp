#include "pch.h"
#include "application.h"
#include "core/window.h"

/*
application* application::_instance = nullptr;

void application::_handle_events(const event& ev) {
    if (ev.type() == event_type::window_close) {
        _shutdown();
    }

    if (ev.type() == event_type::window_resize) {
        glViewport(0, 0, window::get_width(), window::get_height());
        std::cout << "x: " << window::get_width() << ", y: " << window::get_height() << "\n";
    }
}

void application::_init_systems() {
	window::init();
}

void application::_run() {
	while (_is_running) {
		glfwPollEvents();
	}
}

void application::_shutdown() {
    _is_running = false;
    window::shutdown();
}
*/
