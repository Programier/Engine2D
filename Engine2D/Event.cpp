/*
 * Event.cpp
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: programier
 */

#include <GLFW/glfw3.h>
#include <string.h>
#include <iostream>
#include "Window.hpp"
#include "Event.hpp"

static bool inited = false;
bool Event::_keys[1024];
unsigned int Event::_frames[1024];
unsigned int Event::_current;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
		int mode) {
	if (action == GLFW_PRESS) {
		Event::_keys[key] = true;
		Event::_frames[key] = Event::_current;
	} else if (action == GLFW_RELEASE) {
		Event::_keys[key] = false;
		Event::_frames[key] = Event::_current;
	}
}

int Event::init()
{
    std::cout << "Starting init Event System" << std::endl;
    memset(Event::_keys, false, 1024 * sizeof(bool));
    memset(Event::_frames, 0, 1024 * sizeof(unsigned int));
    glfwSetKeyCallback(Window::window, key_callback);
    std::cout << "Event System successfully initialized" << std::endl;
    inited = true;
    return 0;
}

bool Event::pressed(int keycode) {
	if (keycode < 0 || keycode > 1023)
		return false;
	return Event::_keys[keycode];
}

bool Event::jpressed(int keycode) {
	if (keycode < 0 || keycode >= 1023)
		return false;
	return Event::_keys[keycode] && Event::_frames[keycode] == _current;
}

void Event::pollEvents() {
	Event::_current++;
	glfwPollEvents();
}

void Event::terminate() {
	inited = false;
}

bool Event::isInited(){
	return inited;
}
