#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "Window.hpp"

bool inited = false;

int Window::init(int window_width, int window_height,
                 const char *windowname, bool rezisable)
{
    Window::width = window_width;
    Window::height = window_height;
    glfwInit();
    std::cout << "Start to set glfw" << std::endl;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, rezisable);
    window = glfwCreateWindow(width, height, windowname, nullptr, nullptr);
    if (window == nullptr)
    {
        std::cerr << "Filed to create new window. TERMINATED!\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK)
    {
        std::cerr << "Filed to initialize new window\n";
        return -1;
    }

    glViewport(0, 0, width, height);
    std::cout << "The window was created successfully" << std::endl;
    inited = true;
    return 0;
}

void Window::terminate() {
	std::cout << "Terminated window" << std::endl;
	glfwTerminate();
	inited = false;
}

void Window::swapBuffers() {
	glfwSwapBuffers(window);
}

bool Window::isOpen() {
	glfwGetWindowSize(Window::window, &Window::width, &Window::height);
	glViewport(0, 0, Window::width, Window::height);
	return !glfwWindowShouldClose(Window::window);
}
void Window::setShouldClose(bool flag) {
	std::cout << "Closing Window" << std::endl;
	glfwSetWindowShouldClose(Window::window, flag);
	inited = false;
}

void Window::clearBuffer(){
	glClear(GL_COLOR_BUFFER_BIT);
}

bool Window::isInited(){
	return inited;
}
