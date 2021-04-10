#include "Engine2D/Engine.hpp"
#define GROUND_SPEED 7
std::string programmPath;
int background_x = 0;


int main(int argc, char **argv)
{
    programmPath = argv[0];
    while(programmPath[programmPath.length() - 1] != '/')
        programmPath.erase(programmPath.end() - 1);
    std::cout << programmPath << std::endl;
    Window::init(1280, 720, "FlappyBird", false);
    Event::init();
    Texture_Renderer::init();
    Texture background;
    loadTexture(programmPath + "images/Background.png", background);
    while (Window::isOpen())
    {
        Event::pollEvents();
        Window::clearBuffer();
        draw(background, background_x, 0, 1280, 720);
        draw(background, background_x + 1280, 0, 1280, 720);
        background_x -= GROUND_SPEED;
        if (background_x <= -1280)
            background_x = 0;
        Window::swapBuffers();
    }
    Window::terminate();
    Event::terminate();
    Texture_Renderer::terminate();
    deleteTexture(background);
}