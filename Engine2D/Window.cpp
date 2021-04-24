/*
 * Window.cpp
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: programier
 */

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"


static bool inited = false;
GLFWwindow *Window::window;
int Window::width, Window::height;


int Window::init(int window_width, int window_height,
                 const char *windowname, bool rezisable)
{
    width = window_width;
    height = window_height;
    glfwInit();
    std::cout << "WINDOW: Start to set glfw" << std::endl;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, rezisable);
    window = glfwCreateWindow(width, height, windowname, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "WINDOW: Filed to create new window. TERMINATED!\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "WINDOW: Fаiled to initialize new window\n";
        return -1;
    }

    glViewport(0, 0, width, height);
    std::cout << "WINDOW: The window was created successfully" << std::endl;
    inited = true;
    return 0; 
    
}


void Window::terminate() {
	std::cout << "WINDOW: Terminated window" << std::endl;
	glfwTerminate();
	inited = false;
}

void Window::swapBuffers() {
	glfwSwapBuffers(window);
}

bool Window::isOpen() {
	return !glfwWindowShouldClose(window);
}
void Window::setShouldClose(bool flag) {
	std::cout << "WINDOW: Closing Window" << std::endl;
	glfwSetWindowShouldClose(window, flag);
	inited = false;
}

void Window::clearBuffer(){
	glClear(GL_COLOR_BUFFER_BIT);
}

bool Window::isInited(){
	return inited;
}