#include "Engine2D/Engine.hpp"
#include <vector>
#define GROUND_SPEED 5
#define BIRD_X 150
#define BIRD_FRAME_SPEED 0.2
#define GROUND_POS_Y 86
#define JUMP 12
#define DIGIT_SIZE 32

namespace GameData
{
    // Game Variables
    bool pause = false, gameOver = false;
    int background_x = 0;
    unsigned int score = 0;
    int ground_pos = GROUND_POS_Y;

    // Programm Variables
    std::string programmPath;
    const int window_initial_width = 1280, window_initial_height = 720;

    // Textures
    Texture background;
    struct
    {
        int y = 350, x = BIRD_X;
        double speed_y = 0;
        Texture texture[4];
        double frame = 0;
        int size_x, size_y;

    } bird;

    struct
    {
        Texture baseTexture;
        Texture digits[10];
        int size;
    } digits;

    void loadAllTextures()
    {
        loadTexture(GameData::programmPath + "images/Background.png", background);
        loadTexture(programmPath + "images/counts.png", digits.baseTexture);
        for (int i = 0; i < 10; i++)
        {
            inheritTexture(digits.baseTexture, digits.digits[i]);
            setPart(digits.digits[i], i * 102, 0, 102, 102);
        }
        for (int i = 0; i < 4; i++)
            loadTexture(programmPath + "images/flap" + std::to_string(i + 1) + ".png", bird.texture[i]);
    }

    void deleteAllTextures()
    {
        deleteTexture(background);
        deleteTexture(digits.baseTexture);
        for (int i = 0; i < 4; i++)
            deleteTexture(bird.texture[i]);
    }

    void updateInfo()
    {
        digits.size = DIGIT_SIZE * (static_cast<double>(Window::height) / GameData::window_initial_height);
        bird.x = BIRD_X * (static_cast<double>(Window::width) / GameData::window_initial_width);
        bird.size_y = getInitialHeight(bird.texture[0]) * (static_cast<double>(Window::height) / window_initial_height);
        bird.size_x = getInitialWidth(bird.texture[0]) * (static_cast<double>(Window::height) / window_initial_height);
    }
}

void drawScore()
{
    std::vector<int> _digits;
    GameData::updateInfo();
    unsigned int _score = GameData::score;

    if (_score == 0)
        _digits.push_back(0);
    else
        while (_score != 0)
        {
            _digits.insert(_digits.begin(), _score % 10);
            _score /= 10;
        }

    int begin = (Window::width / 2) - GameData::digits.size + (_digits.size() % 2 == 0) * (GameData::digits.size / 2) - ((_digits.size() / 2) * GameData::digits.size);
    int y_pos = Window::height - GameData::digits.size - 8;
    for (int i = 0; i < _digits.size(); i++)
        draw(GameData::digits.digits[_digits[i]], begin + i * GameData::digits.size, y_pos, GameData::digits.size, GameData::digits.size);
}

int start_game(int argc, char **argv)
{
    GameData::programmPath = argv[0];
    while (!GameData::programmPath.empty() && GameData::programmPath[GameData::programmPath.length() - 1] != '/' && GameData::programmPath[GameData::programmPath.length() - 1] != '\\')
        GameData::programmPath.erase(GameData::programmPath.end() - 1);

    Window::init(1280, 720, "FlappyBird", true);
    Event::init();
    Texture_Renderer::init();

    /* Loading Data from external files */
    GameData::loadAllTextures();

    while (Window::isOpen() /* && !GameData::gameOver */)
    {
        Event::pollEvents();
        Window::clearBuffer();

        if (Event::jpressed(GLFW_KEY_P))
            GameData::pause = !GameData::pause;

        // Logic block
        if (!GameData::pause)
        {
            GameData::updateInfo();

            GameData::background_x -= GROUND_SPEED;
            GameData::bird.frame += BIRD_FRAME_SPEED;
            GameData::bird.y += GameData::bird.speed_y;
            GameData::bird.speed_y -= 0.75;
            if (GameData::bird.frame >= 4)
                GameData::bird.frame = 0;
            if (GameData::background_x <= -1 * Window::width)
                GameData::background_x = 0;
            if (GameData::bird.y <= GROUND_POS_Y)
            {
                GameData::gameOver = true;
                GameData::bird.speed_y = 0;
                GameData::bird.y = GROUND_POS_Y;
            }
            if (GameData::bird.y > Window::height - GameData::bird.texture->height)
            {
                GameData::bird.y = Window::height - GameData::bird.texture->height;
                GameData::bird.speed_y = 0;
            }
            if (Event::jpressed(GLFW_KEY_SPACE))
            {
                GameData::bird.speed_y = JUMP;
                GameData::score++;
            }
        }

        // Draw block
        draw(GameData::background, GameData::background_x, 0, Window::width, Window::height);
        draw(GameData::background, GameData::background_x + Window::width, 0, Window::width, Window::height);
        drawScore();
        draw(GameData::bird.texture[static_cast<int>(GameData::bird.frame)], GameData::bird.x, GameData::bird.y, GameData::bird.size_x, GameData::bird.size_y);

        Window::swapBuffers();
    }
    Window::terminate();
    Event::terminate();
    Texture_Renderer::terminate();
    GameData::deleteAllTextures;
    return GameData::score;
}

int main(int argc, char **argv)
{
    return start_game(argc, argv);
}
