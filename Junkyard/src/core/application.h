#pragma once

class application {
public:
	static void create_application(int argc, char* argv[]) {
		if (_instance != nullptr) {
			// meaby parse args first and passing them below
			_instance = new application();
			_instance->_init_systems();
			_instance->_run();
		}
	}

	static application* get_instance() {
		if (_instance != nullptr) {
			return _instance;
		}
	}

private:
	void _init_systems();
	void _run();

	static application* _instance;
	bool _is_running = true;
};

