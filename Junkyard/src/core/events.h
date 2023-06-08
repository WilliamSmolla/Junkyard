#pragma once
#include "pch.h"
#include "core/input_codes.h"

enum class event_type {
	none = 0,
	window_close, window_resize,
	key_pressed, key_released,
	mouse_moved
};

class event {
public:
	event() = default;
	event(const std::string& name = "", event_type type = event_type::none)
		: _type{ type }, _name{ name } {}

	inline const event_type type() const { return _type; }
	inline const std::string& name() const { return _name; }
	virtual bool is_handled() const { return _isHandled; }
	inline void handle() { _isHandled = true; }

protected:
	event_type _type;
	std::string _name;
	bool _isHandled = false;
};

class window_close_event : public event {
public:
	window_close_event()
		: event{ "WindowCloseEvent", event_type::window_close } {}
	virtual ~window_close_event() = default;
};

class window_resize_event : public event {
public:
	window_resize_event(unsigned int width, unsigned int height)
		: event("WindowResizeEvent", event_type::window_resize), _width{ width }, _height{ height } {}
	virtual ~window_resize_event() = default;

	unsigned int width() const { return _width; }
	unsigned int height() const { return _height; }

private:
	unsigned int _width;
	unsigned int _height;
};

class key_pressed_event : public event {
public:
	key_pressed_event() = default;
	key_pressed_event(input_codes::keycode key, bool is_repeated = false)
		: event("KeyPressedEvent", event_type::key_pressed), _keycode{ key }, _is_repeated{ is_repeated } {}
	virtual ~key_pressed_event() = default;

	input_codes::keycode keycode() const { return _keycode; }
	bool is_repeated() const { return _is_repeated; }

private:
	input_codes::keycode _keycode;
	bool _is_repeated;
};

class key_released_event : public event {
public:
	key_released_event() = default;
	key_released_event(input_codes::keycode key)
		: event("KeyReleasedEvent", event_type::key_released), _keycode{ key } {}
	virtual ~key_released_event() = default;

	input_codes::keycode keycode() const { return _keycode; }

private:
	input_codes::keycode _keycode;
};

