#include "Graphics.hpp"
#include <iostream>

extern "C"
{
    bool (*Graphics::_Event::EventisInited)() = nullptr;
    int (*Graphics::_Event::getX)() = nullptr;
    int (*Graphics::_Event::getY)() = nullptr;
    bool (*Graphics::_Event::pressed)(int) = nullptr;

    void Graphics::initGraphics(bool (*EventIsInited)(), int (*MouseGetX)(), int (*MouseGetY)(), bool (*MousePressed)(int))
    {
        Graphics::_Event::EventisInited = EventIsInited;
        Graphics::_Event::getX = MouseGetX;
        Graphics::_Event::getY = MouseGetY;
        Graphics::_Event::pressed = MousePressed;
        printf("GRAPHICS: %p\t%p\t%p\t%p\n", EventIsInited, MouseGetX, MouseGetY, MousePressed);
    }
}