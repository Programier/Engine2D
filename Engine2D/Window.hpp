/*
 * Window.cpp
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: programier
 */

#ifndef WINDOW_HPP
#define WINDOW_HPP

//We do not include the whole library, but simply use the class pre-declaration
class GLFWwindow;

namespace Window
{
    extern GLFWwindow *window;
    extern int width, height;
    int init(int window_width, int window_height,
             const char *windowname, bool rezisable);
    void terminate();

    bool isOpen();
    void setShouldClose(bool flag);
    void swapBuffers();
    void clearBuffer();
    bool isInited();
} // namespace Window

#endif