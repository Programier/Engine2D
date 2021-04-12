#include "Engine2D/Engine.hpp"
#include <vector>
#include <chrono>
#include <thread>

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
    int background_x = 0, background_width;
    unsigned int score = 0;
    int ground_pos = GROUND_POS_Y;
    double diff;

    // Programm Variables
    std::string programmPath;
    const int window_initial_width = 1280, window_initial_height = 720;
    int FPS = 60, tmp_FPS = FPS;
    Texture fps_texture;

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

    struct
    {
        Music wings, die;
    } audio;

    void loadResources()
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
        loadTexture(programmPath + "images/fps.png", fps_texture);
        loadWAV(programmPath + "audio/sfx_wing.wav", audio.wings);
        loadWAV(programmPath + "audio/sfx_die.wav", audio.die);
    }

    void deleteResources()
    {
        deleteTexture(background);
        deleteTexture(digits.baseTexture);
        for (int i = 0; i < 4; i++)
            deleteTexture(bird.texture[i]);
        deleteTexture(fps_texture);
        deleteMusic(audio.wings);
        deleteMusic(audio.die);
    }

    void updateInfo()
    {
        diff = static_cast<double>(Window::height) / window_initial_height;
        digits.size = DIGIT_SIZE * diff;
        bird.x = BIRD_X * (static_cast<double>(Window::width) / GameData::window_initial_width);
        bird.size_y = getInitialHeight(bird.texture[0]) * diff;
        bird.size_x = getInitialWidth(bird.texture[0]) * diff;
        background_width = getInitialWidth(background) * static_cast<double>(Window::height) / getInitialHeight(background);
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

void drawFPS()
{
    std::vector<int> _digits;
    int _FPS = GameData::tmp_FPS;
    GameData::updateInfo();
    if (_FPS == 0)
        _digits.push_back(0);
    else
        while (_FPS != 0)
        {
            _digits.insert(_digits.begin(), _FPS % 10);
            _FPS /= 10;
        }

    int y_pos = Window::height - GameData::digits.size - 8;
    int fps_width = (GameData::fps_texture.width / 3) * GameData::diff;
    draw(GameData::fps_texture, 10, y_pos, fps_width, GameData::digits.size * 2);
    for (int i = 0; i < _digits.size(); i++)
        draw(GameData::digits.digits[_digits[i]], 10 + fps_width + i * GameData::digits.size, y_pos, GameData::digits.size, GameData::digits.size);
}

int start_game(int argc, char **argv)
{
    GameData::programmPath = argv[0];
    while (!GameData::programmPath.empty() && GameData::programmPath[GameData::programmPath.length() - 1] != '/' && GameData::programmPath[GameData::programmPath.length() - 1] != '\\')
        GameData::programmPath.erase(GameData::programmPath.end() - 1);

    Window::init(1280, 720, "FlappyBird", true);
    Event::init();
    Texture_Renderer::init();
    Speaker::init();

    /* Loading Data from external files */
    GameData::loadResources();

    auto begin = std::chrono::steady_clock::now(), fps_updater = std::chrono::steady_clock::now();
    int background_frame_count;
    while (Window::isOpen() /* && !GameData::gameOver */)
    {
        begin = std::chrono::steady_clock::now();
        Event::pollEvents();
        Window::clearBuffer();
        if (Event::jpressed(GLFW_KEY_P))
            GameData::pause = !GameData::pause;

        GameData::updateInfo();

        // Logic block
        if (!GameData::pause)
        {
            GameData::background_x -= (GROUND_SPEED * GameData::diff) * 60 / GameData::FPS;
            GameData::bird.frame += BIRD_FRAME_SPEED * 60 / GameData::FPS;
            GameData::bird.y += GameData::bird.speed_y * 60 / GameData::FPS;
            GameData::bird.speed_y -= 0.75 * GameData::diff * 60 / GameData::FPS;

            if (GameData::bird.frame >= 4)
                GameData::bird.frame = 0;

            if (GameData::background_x <= -1 * GameData::background_width)
                GameData::background_x = 0;

            if (GameData::bird.y <= GROUND_POS_Y * GameData::diff)
            {
                GameData::gameOver = true;
                GameData::bird.speed_y = 0;
                GameData::bird.y = GROUND_POS_Y * GameData::diff;
                if (!Speaker::isPlaying(GameData::audio.die))
                    Speaker::play(GameData::audio.die);
            }

            if (GameData::bird.y > Window::height - GameData::bird.size_y)
            {
                GameData::bird.y = Window::height - GameData::bird.size_y;
                GameData::bird.speed_y = 0;
            }

            if (Event::jpressed(GLFW_KEY_SPACE))
            {
                GameData::bird.speed_y = JUMP * GameData::diff;
                GameData::score++;
                Speaker::play(GameData::audio.wings);
            }
        }

        // Draw block
        background_frame_count = Window::width * 2 / GameData::background_width;
        if (background_frame_count * GameData::background_width < Window::width * 2)
            background_frame_count++;
        if (background_frame_count < 2)
            background_frame_count = 2;
        for (int i = 0; i < background_frame_count; i++)
            draw(GameData::background, GameData::background_x + i * GameData::background_width, 0, GameData::background_width, Window::height);

        drawScore();
        draw(GameData::bird.texture[static_cast<int>(GameData::bird.frame)], GameData::bird.x, GameData::bird.y, GameData::bird.size_x, GameData::bird.size_y);
        drawFPS();

        Window::swapBuffers();
        auto elapsed_mcs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin);
        GameData::FPS = 1000000 / elapsed_mcs.count();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - fps_updater).count() >= 300)
        {
            fps_updater = std::chrono::steady_clock::now();
            GameData::tmp_FPS = GameData::FPS;
        }
    }
    Window::terminate();
    Event::terminate();
    Texture_Renderer::terminate();
    Speaker::terminate();
    GameData::deleteResources();
    return GameData::score;
}

int main(int argc, char **argv)
{
    return start_game(argc, argv);
}
