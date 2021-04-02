#include <iostream>
#include "Engine2D/Engine.hpp"

int main()
{
    Window::init(1280, 720, "Test", true);
    Event::init();
    Texture_Renderer::init();
    Speaker::init();
    Music test;
    loadWAV("test.wav", test);
    Texture texture;
    loadTexture("test.png", texture);
    while (Window::isOpen())
    {
        Window::clearBuffer();
        Event::pollEvents();
        if (Event::jpressed(GLFW_KEY_H))
            invertHorizontally(texture);
        if (Event::jpressed(GLFW_KEY_V))
            invertVertically(texture);
        if (Event::jpressed(GLFW_KEY_D))
        {
            toDefault(texture);
        }
        if (Event::jpressed(GLFW_KEY_E))
        {
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

        if (Event::jpressed(GLFW_KEY_P))
            Speaker::play(test);
        if (Event::jpressed(GLFW_KEY_S))
            Speaker::stop(test);
        if (Event::pressed(GLFW_KEY_G))
            std::cout << "AUDIO PLAYING: " << Speaker::isPlaying(test) << std::endl;
        if(Event::jpressed(GLFW_KEY_A)){
            Speaker::clearStack();
        }

        fullScreenDraw(texture);
        Window::swapBuffers();
    }
    Window::terminate();
    Event::terminate();
    Texture_Renderer::terminate();
    Speaker::terminate();
    deleteTexture(texture);
    deleteMusic(test);
}
