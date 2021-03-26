#include <iostream>
#include "Engine2D/Engine.hpp"

int main(){
    Window::init(1280, 720, "Test", false);
    while(Window::isOpen()){
        Window::swapBuffers();
    }
    Window::terminate();
}