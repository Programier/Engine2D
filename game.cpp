#include <iostream>
#include "Engine2D/Engine.hpp"

int main()
{
    Window::init(1280, 720, "Test", false);
    Event::init();
    Texture_Renderer::init();
    Texture texture;
    loadTexture("test.png", texture);
    while (Window::isOpen())
    {
        Window::clearBuffer();
        Event::pollEvents();
        if(Event::jpressed(GLFW_KEY_H))
            invertHorizontally(texture);
        if(Event::jpressed(GLFW_KEY_V))
            invertVertically(texture);
        if (Event::jpressed(GLFW_KEY_D))
        {
            toDefault(texture);
        }
        if(Event::jpressed(GLFW_KEY_E)){
            system("clear");
            std::cout << "Окей, текущие размеры текстуры: " << texture.width << "\t" << texture.height << std::endl;
            int x, y, width, heigth;
            std::cout << "Введите Х: ";
            std::cin >> x;
            std::cout << "Введите Y: ";
            std::cin >> y;
            std::cout << "Введите ширину: ";
            std::cin >> width;
            std::cout << "Введите высоту: ";
            std::cin >> heigth;
            setPart(texture, x, y, width, heigth);
        }
        draw(texture);
        Window::swapBuffers();
    }
    Window::terminate();
    Event::terminate();
    Texture_Renderer::terminate();
    deleteTexture(texture);
}