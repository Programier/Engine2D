#include <iostream>
#include "Engine2D/Engine.hpp"

int main(){
    Window::init(1280, 720, "Test", true);
    Event::init();
    Texture_Renderer::init();
    Texture texture;
    loadTexture("test.png", texture);
    while (Window::isOpen())
    {
        Window::clearBuffer();
        Event::pollEvents();
        if(Event::jpressed(GLFW_KEY_I))
            invertVertically(texture);
        fullScreenDraw(texture);
        Window::swapBuffers();
    }
    Window::terminate();
    Event::terminate();
    Texture_Renderer::terminate();
    deleteTexture(texture);
}