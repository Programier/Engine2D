
/*
 * Window.cpp
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: programier
 */

#include <iostream>
#include <GL/glew.h>
#include <GLFW/glfw3.h>

extern "C"
{
    namespace Window
    {
        static bool inited = false;
        GLFWwindow *_window;
        int width, height;

        int init(int window_width, int window_height,
                 const char *windowname, bool rezisable, int min_width, int min_height, int max_width, int max_height)
        {
            width = window_width;
            height = window_height;
            glfwInit();
            std::cout << "WINDOW: Start to set glfw" << std::endl;
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
            glfwWindowHint(GLFW_RESIZABLE, rezisable);
            _window = glfwCreateWindow(width, height, windowname, nullptr, nullptr);
            if (_window == nullptr)
            {
                std::cerr << "WINDOW: Filed to create new window. TERMINATED!\n";
                glfwTerminate();
                return -1;
            }
            glfwMakeContextCurrent(_window);
            glewExperimental = GL_TRUE;
            if (glewInit() != GLEW_OK)
            {
                std::cerr << "WINDOW: Fаiled to initialize new window\n";
                return -1;
            }
            glfwSetWindowSizeLimits(_window, min_width, min_height, max_width, max_height);
            glViewport(0, 0, width, height);
            std::cout << "WINDOW: The window was created successfully" << std::endl;
            inited = true;
            return 0;
        }

        void terminate()
        {
            std::cout << "WINDOW: Terminated window" << std::endl;
            glfwTerminate();
            inited = false;
        }

        void swapBuffers()
        {
            glfwSwapBuffers(_window);
        }

        bool isOpen()
        {
            return !glfwWindowShouldClose(_window);
        }
        void setShouldClose(bool flag)
        {
            std::cout << "WINDOW: Closing Window" << std::endl;
            glfwSetWindowShouldClose(_window, flag);
            inited = false;
        }

        void clearBuffer()
        {
            glClear(GL_COLOR_BUFFER_BIT);
        }

        bool isInited()
        {
            return false;
        }
    }
}