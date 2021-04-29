#include "Engine2D/Engine.hpp"
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>

#define GROUND_SPEED 3.5
#define BIRD_X 250
#define BIRD_FRAME_SPEED 0.2
#define GROUND_POS_Y 86
#define JUMP 7
#define DIGIT_SIZE 32
#define COLUMN_DISTANCE_Y 135
#define COLUMN_DISTANCE_X 250
#define COLUMN_DIFF_Y 250
#define MAX_FALLING_SPEED -15

namespace GameData
{
    // Game Variables
    struct
    {
        Texture background, sky, ground;
    } background;

    struct
    {
        double y = 350, x = BIRD_X;
        double speed_y = 0;
        Texture texture[4];
        double frame = 0;
        int size_x, size_y;
        float angle = 0;
        float prev_angle = 0;
    } bird;

    struct
    {
        Texture baseTexture;
        Texture digits[10];
        int size;
    } digits;

    struct
    {
        Music wings, die, hit, point;
    } audio;

    typedef struct _Column
    {
        double x, y_upper, y_lower;
    } Column;

    std::vector<Column> columns;
    bool pause = false, gameOver = false, change_column = false,
         gameOverAudioIsReproduced = false;
    double sky_x = 0, ground_x, diff = 1;
    int background_width, background_frame_count, ground_pos = GROUND_POS_Y,
                                                  column_width, column_height, colums_count;
    unsigned int score = 0;
    Texture _column;

    // Programm Variables
    std::string programmPath;
    const int window_initial_width = 1280, window_initial_height = 720;
    int FPS = 60, tmp_FPS = FPS;
    Texture fps_texture;
    bool show_fps = true;

    void loadResources()
    {
        loadTexture(GameData::programmPath + "images/Background.png",
                    background.background);
        inheritTexture(background.background, background.ground);
        inheritTexture(background.background, background.sky);
        setPart(background.ground, 0, 0, background.background.width, 100);
        setPart(background.sky, 0, 100, background.background.width,
                background.background.height - 100);
        loadTexture(programmPath + "images/counts.png", digits.baseTexture);
        for (int i = 0; i < 10; i++)
        {
            inheritTexture(digits.baseTexture, digits.digits[i]);
            setPart(digits.digits[i], i * 102, 0, 102, 102);
        }
        for (int i = 0; i < 4; i++)
            loadTexture(programmPath + "images/flap" + std::to_string(i + 1) + ".png",
                        bird.texture[i]);
        loadTexture(programmPath + "images/fps.png", fps_texture);
        loadWAV(programmPath + "audio/sfx_wing.wav", audio.wings);
        loadWAV(programmPath + "audio/sfx_die.wav", audio.die);
        loadWAV(programmPath + "audio/sfx_hit.wav", audio.hit);
        loadWAV(programmPath + "audio/sfx_point.wav", audio.point);
        loadTexture(programmPath + "images/column.png", _column);
    }

    void deleteResources()
    {
        deleteTexture(background.background);
        deleteTexture(digits.baseTexture);
        for (int i = 0; i < 4; i++)
            deleteTexture(bird.texture[i]);
        deleteTexture(fps_texture);
        deleteMusic(audio.wings);
        deleteMusic(audio.die);
        deleteMusic(audio.hit);
        deleteMusic(audio.point);
        deleteTexture(_column);
    }

    void updateBird()
    {
        if (bird.y > GROUND_POS_Y * diff)
        {
            bird.y += bird.speed_y * 60 / FPS;
            bird.speed_y -= 0.4 * diff * 60 / FPS;
            if (bird.speed_y < MAX_FALLING_SPEED)
                bird.speed_y = MAX_FALLING_SPEED;
            if (bird.frame >= 4)
                bird.frame = 0;
            bird.x = BIRD_X * diff;
            bird.size_y = getInitialHeight(bird.texture[0]) * diff;
            bird.size_x = getInitialWidth(bird.texture[0]) * diff;

            bird.angle = atan(fabs((bird.speed_y * 60.0 / FPS) / (2 * GROUND_SPEED))) * 180 / M_PI;
            bird.angle = bird.prev_angle + (bird.angle - bird.prev_angle) / 1.5;
            if ((bird.speed_y * 60.0 / FPS) < 0)
            {
                bird.angle *= -1.0;
            }
        }
    }

    void updateInfo()
    {

        auto getMinLimit = [](int index) -> int {
            int min = COLUMN_DIFF_Y;
            if (columns[index].y_lower - min + _column.height < 10)
                min = -1 * (10 - _column.height - columns[index].y_lower);
            return min;
        };

        auto getMaxLimit = [](int index) -> int {
            int max = COLUMN_DIFF_Y;
            if (columns[index].y_lower + max + _column.height > window_initial_height)
                max = (window_initial_height - _column.height - columns[index].y_lower);
            return max;
        };

        if (Event::Keyboard::jpressed(GLFW_KEY_P))
        {
            if (pause)
            {
                bird.speed_y = 0;
                gameOverAudioIsReproduced = false;
                gameOver = false;
            }
            pause = !pause;
        }

        diff = static_cast<double>(Window::height) / window_initial_height;
        column_height = _column.height * diff;
        column_width = _column.width * diff;
        if (!pause)
        {
            sky_x -= ((GROUND_SPEED / 3.0) * diff) * 60 / FPS;
            ground_x -= (GROUND_SPEED * diff) * 60 / FPS;
            //bird.frame += BIRD_FRAME_SPEED * 60 / FPS;
            bird.frame = (bird.speed_y < -6 * diff) ? 3 : bird.frame + (BIRD_FRAME_SPEED * 60 / FPS);

            if (sky_x <= -1 * background_width)
                sky_x = 0;

            if (ground_x <= -1 * background_width)
                ground_x = 0;

            if (bird.y <= (GROUND_POS_Y - 2) * diff)
            {
                gameOver = true;
                pause = true;
                bird.speed_y = 0;
                bird.y = (GROUND_POS_Y - 2) * diff;
            }

            if (bird.y > Window::height - bird.size_y)
            {
                bird.y = Window::height - bird.size_y;
                bird.speed_y = 0;
            }

            if (Event::Keyboard::jpressed(GLFW_KEY_SPACE))
            {
                bird.speed_y = JUMP * diff;
                Speaker::play(audio.wings);
            }

            if (columns.size() > 1)
            {
                if ((bird.x >= columns[change_column].x + column_width - bird.size_y / 2) &&
                    (bird.x < GROUND_SPEED * diff + columns[change_column].x + column_width -
                                  bird.size_y / 2))
                {
                    score++;
                    change_column = true;
                    Speaker::play(audio.point);
                }

                // Check collisions
                if (bird.y + 2 * diff < columns[change_column].y_lower * diff + Window::height || bird.y + bird.size_y - 3 * diff > columns[change_column].y_upper * diff)
                    if (bird.x + bird.size_x - 4 * diff >= columns[change_column].x &&
                        bird.x - 4 * diff <= columns[change_column].x + column_width)
                    {
                        pause = true;
                        gameOver = true;
                    }
            }
        }

        digits.size = DIGIT_SIZE * diff;
        //bird.x = BIRD_X * (static_cast<double>(Window::width) / GameData::window_initial_width);
        if (!pause)
            updateBird();
        background_width = getInitialWidth(background.background) * static_cast<double>(Window::height) / getInitialHeight(background.background);

        // Columns generation
        colums_count = (3 + (Window::width / ((static_cast<double>(column_width) + COLUMN_DISTANCE_X) * diff)));
        for (int i = 0; i < colums_count; i++)
        {
            if (i >= static_cast<int>(columns.size()))
            {
                columns.push_back(Column());
                if (columns.size() == 1)
                {
                    columns[0].x = Window::width + COLUMN_DISTANCE_X * diff;
                    columns[i].y_lower = (-1 * (_column.height - GROUND_POS_Y)) + rand() % (static_cast<int>((_column.height - COLUMN_DISTANCE_Y - GROUND_POS_Y)));
                }
                else
                {
                    columns[i].x = columns[i - 1].x + COLUMN_DISTANCE_X * diff;

                    int tmp = getMinLimit(i - 1);
                    columns[i].y_lower = columns[i - 1].y_lower + (rand() % (getMaxLimit(i - 1) + tmp) - tmp);
                }
                if (columns[i].y_lower + _column.height + COLUMN_DISTANCE_Y >
                    window_initial_height - 10)
                    columns[i].y_lower = window_initial_height - COLUMN_DISTANCE_Y - _column.height - 10;

                if (columns[i].y_lower + _column.height <= GROUND_POS_Y + 10)
                    columns[i].y_lower = GROUND_POS_Y + 10 - _column.height;

                columns[i].y_upper = columns[i].y_lower + (_column.height + COLUMN_DISTANCE_Y);
            }
            else
            {
                if (i > 0)
                    columns[i].x = columns[i - 1].x + COLUMN_DISTANCE_X * diff;
                else
                {
                    if (columns[0].x <= -1 * column_width)
                        columns.erase(columns.begin() + i--);
                    else if (!pause)
                        columns[0].x -= (GROUND_SPEED * diff) * 60 / FPS;
                }
            }
        }
        background_frame_count = Window::width * 2 / background_width;
        if (background_frame_count * background_width < Window::width * 2)
            background_frame_count++;
        if (background_frame_count < 2)
            background_frame_count = 2;

        if (gameOver && !gameOverAudioIsReproduced)
        {
            gameOverAudioIsReproduced = true;
            Speaker::play(audio.hit);
            if (score > 0)
                Speaker::play(audio.die);
        }
    }
}

std::vector<int> countToVector(int count)
{
    std::vector<int> _digits;

    if (count == 0)
        _digits.push_back(0);
    else
        while (count != 0)
        {
            _digits.insert(_digits.begin(), count % 10);
            count /= 10;
        }
    return _digits;
}

void drawFrame()
{
    using namespace GameData;
    // Draw sky
    for (int i = 0; i < background_frame_count; i++)
        draw(background.sky, sky_x + i * background_width, GROUND_POS_Y * diff, 0, background_width, Window::height - GROUND_POS_Y * diff);

    // Draw Column
    for (int i = 0; i < colums_count; i++)
    {
        draw(_column, columns[i].x, columns[i].y_lower * diff, 0, column_width, Window::height);
        draw(_column, columns[i].x, columns[i].y_upper * diff, 0, column_width, Window::height, HORIZONTAL);
    }

    // Draw ground
    for (int i = 0; i < background_frame_count; i++)
        draw(background.ground, ground_x + i * background_width, 0, 0, background_width, GROUND_POS_Y * diff);

    // Draw score
    std::vector<int> _digits = countToVector(score);
    int begin = (Window::width / 2) - digits.size + (_digits.size() % 2 == 0) * (digits.size / 2) - ((_digits.size() / 2) * digits.size);
    int y_pos = Window::height - digits.size - 8;
    for (std::size_t i = 0; i < _digits.size(); i++)
        draw(digits.digits[_digits[i]], begin + i * digits.size, y_pos, 0, digits.size, digits.size);

    // Draw bird

    draw(bird.texture[static_cast<int>(bird.frame)], bird.x, bird.y, bird.angle, bird.size_x, bird.size_y);

    // Draw FPS counter
    if (show_fps)
    {
        _digits.clear();
        _digits = countToVector(tmp_FPS);
        int y_pos = Window::height - GameData::digits.size - 8;
        int fps_width = (GameData::fps_texture.width / 3) * GameData::diff;
        draw(GameData::fps_texture, 10, y_pos, 0, fps_width, GameData::digits.size * 2);
        for (std::size_t i = 0; i < _digits.size(); i++)
            draw(GameData::digits.digits[_digits[i]], 10 + fps_width + i * GameData::digits.size, y_pos, 0, GameData::digits.size, GameData::digits.size);
    }
}

void menu()
{
}

int start_game(char *argv)
{
    srand(time(NULL));
    GameData::programmPath = getProgrammPath(argv);
    loadEngine(GameData::programmPath + "lib/");
    Window::init(1280, 720, "FlappyBird", true);
    Event::init();
    Texture_Renderer::init();
    Speaker::init();

    /* Loading Data from external files */
    GameData::loadResources();

    auto begin = std::chrono::steady_clock::now(),
         fps_updater = std::chrono::steady_clock::now();
    while (Window::isOpen() && !GameData::gameOver)
    {
        begin = std::chrono::steady_clock::now();
        Event::pollEvents();
        Window::clearBuffer();
        GameData::updateInfo();
        drawFrame();

        Window::swapBuffers();
        auto elapsed_mcs = std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::steady_clock::now() - begin);
        GameData::FPS = 1000000 / elapsed_mcs.count();
        if (std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - fps_updater).count() >= 300)
        {
            fps_updater = std::chrono::steady_clock::now();
            GameData::tmp_FPS = GameData::FPS;
        }
    }

    // Game Already is over
    // Show bird animation
    while (Window::isOpen())
    {
        Event::pollEvents();
        Window::clearBuffer();
        GameData::updateBird();
        drawFrame();
        Window::swapBuffers();
    }
    Window::terminate();
    Event::terminate();
    Texture_Renderer::terminate();
    Speaker::terminate();
    GameData::deleteResources();
    closeEngine();
    return GameData::score;
}

int main(int argc, char **argv)
{
    return start_game(argv[0]);
}
