#include <iostream>
#include "Engine2D/Engine.hpp"

int main(){
    Window::init(1280, 720, "Test", false);
    Event::init();
    Texture_Renderer::init();
    while (Window::isOpen())
    {
        Event::pollEvents();
        Window::swapBuffers();
    }
    Window::terminate();
    Event::terminate();
}