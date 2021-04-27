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
bool keys[1024];
unsigned int frames[1024];
unsigned int current;

// Mouse
bool mouse_buttons[8];
unsigned int mouse_frames[8];

int mouse_x, mouse_y;

void key_callback(GLFWwindow *window, int key, int scancode, int action,
				  int mode)
{
	if (action == GLFW_PRESS)
	{
		keys[key] = true;
		frames[key] = current;
	}
	else if (action == GLFW_RELEASE)
	{
		keys[key] = false;
		frames[key] = current;
	}
}

void viewPort_callback(GLFWwindow *window, int width, int height)
{
	glViewport(0, 0, width, height);
	Window::width = width;
	Window::height = height;
}

void mouse_callback(GLFWwindow *window, int button, int action, int none)
{

	mouse_buttons[button] = (action == GLFW_PRESS);
	mouse_frames[button] = current;
}

void mouse_pos_callback(GLFWwindow *window, double x, double y)
{
	mouse_x = x;
	mouse_y = y;
}

int Event::init()
{
	std::cout << "EVENT: Starting init Event System" << std::endl;
	std::fill(keys, keys + 1024, false);
	std::fill(frames, frames + 1024, 0);
	std::fill(mouse_buttons, mouse_buttons + 8, false);
	std::fill(mouse_frames, mouse_frames + 8, 0);
	glfwSetKeyCallback(Window::window, key_callback);
	glfwSetWindowSizeCallback(Window::window, viewPort_callback);
	glfwSetMouseButtonCallback(Window::window, mouse_callback);
	glfwSetCursorPosCallback(Window::window, mouse_pos_callback);
	mouse_x = -1, mouse_y = Window::height + 1;
	std::cout << "EVENT: Event System successfully initialized" << std::endl;
	inited = true;
	return 0;
}

bool Event::Keyboard::pressed(int keycode)
{
	if (keycode < 0 || keycode >= 1023)
		return false;
	return keys[keycode];
}

bool Event::Keyboard::jpressed(int keycode)
{
	if (keycode < 0 || keycode >= 1023)
		return false;
	return keys[keycode] && frames[keycode] == current;
}

bool Event::Mouse::pressed(int keycode)
{
	if (keycode >= 0 || keycode < 8)
		return mouse_buttons[keycode];
	return false;
}

bool Event::Mouse::jpressed(int keycode)
{
	if (keycode >= 0 || keycode < 8)
		return mouse_buttons[keycode] && mouse_frames[keycode] == current;
	return false;
}

int Event::Mouse::getX()
{
	return mouse_x;
}

int Event::Mouse::getY()
{
	return Window::height - mouse_y;
}

void Event::pollEvents()
{
	current++;
	glfwPollEvents();
}

void Event::terminate()
{
	inited = false;
}

bool Event::isInited()
{
	return inited;
}
