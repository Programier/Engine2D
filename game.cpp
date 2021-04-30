#include "Engine2D/Engine.hpp"
#include <vector>
#include <chrono>
#include <thread>
#include <cmath>
#include <iostream>
#include <fstream>

#define GROUND_SPEED 3.5
#define BIRD_X 250
#define BIRD_FRAME_SPEED 0.2
#define GROUND_POS_Y 86
#define JUMP 7
#define DIGIT_SIZE 32
#define COLUMN_DISTANCE_Y 140
#define COLUMN_DISTANCE_X 250
#define COLUMN_DIFF_Y 230
#define MAX_FALLING_SPEED -15
#define MIN_WINDOW_WIDTH 400
#define MIN_WINDOW_HEIGHT 200
#define OUT_BINARY_FILE "state.bin"

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
    bool pause = false, gameOver = true, change_column = false,
         gameOverAudioIsReproduced = true, gameStarted = false;
    double sky_x = 0, ground_x, diff = 1;
    int background_width, background_frame_count, ground_pos = GROUND_POS_Y,
                                                  column_width, column_height, colums_count;
    unsigned int score = 0, max_score = 0;
    Texture _column, best_score;

    // Programm Variables
    std::string programmPath;
    const int window_initial_width = 1280, window_initial_height = 720;
    int FPS = 60, tmp_FPS = FPS;
    Texture fps_texture;
    bool show_fps = true;
    bool rezisable = false;
    struct
    {
        Texture txr;
        Button button;
    } continueButton, play, quit;

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
        loadTexture(programmPath + "images/continue.png", continueButton.txr);
        createButton(continueButton.button, continueButton.txr, continueButton.txr, continueButton.txr);
        loadTexture(programmPath + "images/play.png", play.txr);
        createButton(play.button, play.txr, play.txr, play.txr);
        loadTexture(programmPath + "images/quit.png", quit.txr);
        createButton(quit.button, quit.txr, quit.txr, quit.txr);
        loadTexture(programmPath + "images/best.png", best_score);
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
        deleteTexture(continueButton.txr);
        deleteTexture(play.txr);
        deleteTexture(quit.txr);
        deleteTexture(best_score);
    }

    void resetData()
    {
        score = 0;
        pause = false;
        gameOver = false;
        gameOverAudioIsReproduced = false;
        bird.y = 350, bird.x = BIRD_X, bird.speed_y = 0;
        bird.frame = 0;
        bird.angle = 0, bird.prev_angle = 0;
        columns.clear();
        gameStarted = false;
        change_column = false;
    }
    bool updateBird()
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
            return true;
        }
        return false;
    }
    void updateDiff()
    {
        diff = static_cast<double>(Window::height) / window_initial_height;
    }
    void updateInfo(bool blockInput = false)
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

        if (Event::Keyboard::jpressed(GLFW_KEY_ESCAPE) && !blockInput)
        {
            pause = !pause;
        }
        updateDiff();
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

            if (Event::Keyboard::jpressed(GLFW_KEY_SPACE) && !blockInput)
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
                    Speaker::play(audio.point);
                }

                // Check collisions
                if (bird.y + 2 * diff < columns[change_column].y_lower * diff + Window::height || bird.y + bird.size_y - 3 * diff > columns[change_column].y_upper * diff)
                    if (bird.x + bird.size_x - 3 * diff >= columns[change_column].x &&
                        bird.x - 3 * diff <= columns[change_column].x + column_width)
                    {
                        pause = true;
                        gameOver = true;
                    }

                if (!change_column && columns[0].x + column_width < bird.x)
                {
                    change_column = true;
                    gameStarted = true;
                    std::cout << "GAME: Column index for collision detection changed" << std::endl;
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

        if (score > max_score)
            max_score = score;
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
        y_pos = Window::height - digits.size - 8;
        int fps_width = (fps_texture.width / 3) * diff;
        draw(fps_texture, 10, y_pos, 0, fps_width, digits.size * 2);
        for (std::size_t i = 0; i < _digits.size(); i++)
            draw(digits.digits[_digits[i]], 10 + fps_width + i * digits.size, y_pos, 0, digits.size, digits.size);
    }

    // Draw best score

    _digits.clear();
    _digits = countToVector(max_score);
    int __width = best_score.width * diff / 2;
    int __height = best_score.height * diff / 2;
    y_pos = Window::height - __height - 5 * diff;
    begin = Window::width - __width - digits.size * _digits.size();
    draw(GameData::best_score, begin, y_pos, 0, __width, __height);
    begin += __width;
    for (std::size_t i = 0; i < _digits.size(); i++)
        draw(digits.digits[_digits[i]], begin + i * digits.size, y_pos, 0, digits.size, digits.size);
}

void startTimer()
{
    auto begin = std::chrono::steady_clock::now();
    int time;
    while ((time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count()) < 3000 && Window::isOpen())
    {
        Event::pollEvents();
        Window::clearBuffer();
        drawFrame();
        GameData::updateDiff();
        draw(GameData::digits.digits[3 - static_cast<int>(time / 1000)], Window::width / 2 - DIGIT_SIZE * GameData::diff, Window::height / 2, 0, DIGIT_SIZE * 2 * GameData::diff, DIGIT_SIZE * 2 * GameData::diff);
        Window::swapBuffers();
    }
}

void play()
{
    startTimer();
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
        if (GameData::pause && !GameData::gameOver)
            return;
    }

    // Game Already is over
    // Show bird animation
    while (Window::isOpen() && GameData::updateBird())
    {
        Event::pollEvents();
        Window::clearBuffer();
        drawFrame();
        Window::swapBuffers();
    }
}

template <typename T>
inline void _write(std::ofstream &to, T *value)
{
    to.write((char *)value, sizeof(T));
}

template <typename T>
inline void _read(std::ifstream &to, T *value)
{
    to.read((char *)value, sizeof(T));
}

void deserialize()
{
    std::cout << "Deserialization start" << std::endl;
    std::ifstream file(OUT_BINARY_FILE, std::ios::binary);
    if (file.is_open())
    {
        // Bird
        _read(file, &GameData::bird.y);
        _read(file, &GameData::bird.x);
        _read(file, &GameData::bird.speed_y);
        _read(file, &GameData::bird.frame);
        _read(file, &GameData::bird.angle);
        _read(file, &GameData::bird.prev_angle);

        // Game variables
        _read(file, &GameData::pause);
        _read(file, &GameData::gameOver);
        _read(file, &GameData::change_column);
        _read(file, &GameData::gameOverAudioIsReproduced);
        _read(file, &GameData::sky_x);
        _read(file, &GameData::ground_x);
        _read(file, &GameData::diff);
        _read(file, &GameData::background_width);
        _read(file, &GameData::background_frame_count);
        _read(file, &GameData::ground_pos);
        _read(file, &GameData::column_width);
        _read(file, &GameData::column_height);
        _read(file, &GameData::colums_count);
        _read(file, &GameData::score);
        _read(file, &GameData::max_score);
        _read(file, &GameData::gameStarted);
        std::cout << "HERE" << std::endl;
        // Column positions
        int size;
        _read(file, &size);
        for (int i = 0; i < size; i++)
        {
            GameData::columns.push_back(GameData::Column());
            _read(file, &GameData::columns[i].x);
            _read(file, &GameData::columns[i].y_lower);
            _read(file, &GameData::columns[i].y_upper);
        }
        file.close();
        std::cout << "End of deserialization" << std::endl;
    }
    else
    {
        std::cerr << "Could not open file. State is not deserialized" << std::endl;
    }
}
void serialize()
{
    std::cout << "Serialization start" << std::endl;
    std::ofstream file(OUT_BINARY_FILE, std::ios::binary);
    if (file.is_open())
    {
        // Bird
        _write(file, &GameData::bird.y);
        _write(file, &GameData::bird.x);
        _write(file, &GameData::bird.speed_y);
        _write(file, &GameData::bird.frame);
        _write(file, &GameData::bird.angle);
        _write(file, &GameData::bird.prev_angle);

        // Game variables
        _write(file, &GameData::pause);
        _write(file, &GameData::gameOver);
        _write(file, &GameData::change_column);
        _write(file, &GameData::gameOverAudioIsReproduced);
        _write(file, &GameData::sky_x);
        _write(file, &GameData::ground_x);
        _write(file, &GameData::diff);
        _write(file, &GameData::background_width);
        _write(file, &GameData::background_frame_count);
        _write(file, &GameData::ground_pos);
        _write(file, &GameData::column_width);
        _write(file, &GameData::column_height);
        _write(file, &GameData::colums_count);
        _write(file, &GameData::score);
        _write(file, &GameData::max_score);
        _write(file, &GameData::gameStarted);

        // Column positions
        int size = static_cast<int>(GameData::columns.size());
        _write(file, &size);
        for (int i = 0; i < GameData::columns.size(); i++)
        {
            _write(file, &GameData::columns[i].x);
            _write(file, &GameData::columns[i].y_lower);
            _write(file, &GameData::columns[i].y_upper);
        }
        file.close();
        std::cout << "End of serialization" << std::endl;
    }
    else
    {
        std::cerr << "Could not open file. State is not serialized" << std::endl;
    }
}

int start_game(char *argv)
{
    srand(time(NULL));
    Window::init(1280, 720, "FlappyBird", GameData::rezisable, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT);
    Event::init();
    Texture_Renderer::init();
    Speaker::init();

    /* Loading Data from external files */
    GameData::loadResources();
    deserialize();
    if (GameData::gameOver || !GameData::gameStarted)
    {
        GameData::resetData();
        GameData::gameOver = true;
        GameData::gameOverAudioIsReproduced = true;
    }
    GameData::updateInfo(true);
    while (Window::isOpen())
    {
        Event::pollEvents();
        Window::clearBuffer();
        GameData::updateDiff();

        GameData::continueButton.button.x = Window::width / 2 - GameData::continueButton.txr.width * GameData::diff / 2;
        GameData::continueButton.button.y = Window::height / 2 + 5 * GameData::diff + GameData::continueButton.txr.height / 2;
        GameData::continueButton.button.width = GameData::continueButton.txr.width * GameData::diff;
        GameData::continueButton.button.height = GameData::continueButton.txr.height * GameData::diff;
        GameData::continueButton.button.visible = !GameData::gameOver;

        GameData::play.button.x = Window::width / 2 - GameData::play.txr.width * GameData::diff / 2;
        GameData::play.button.y = GameData::continueButton.button.y - 5 * GameData::diff - GameData::play.txr.height * GameData::diff;
        GameData::play.button.width = GameData::play.txr.width * GameData::diff;
        GameData::play.button.height = GameData::play.txr.height * GameData::diff;

        GameData::quit.button.x = Window::width / 2 - GameData::continueButton.txr.width * GameData::diff / 2;
        GameData::quit.button.y = GameData::play.button.y - 5 * GameData::diff - GameData::quit.txr.height * GameData::diff;
        GameData::quit.button.width = GameData::quit.txr.width * GameData::diff;
        GameData::quit.button.height = GameData::quit.txr.height * GameData::diff;

        drawFrame();
        draw(GameData::play.button);
        draw(GameData::continueButton.button);
        draw(GameData::quit.button);
        if (buttonClicked(GameData::play.button))
        {
            GameData::resetData();
            GameData::updateInfo(true);
            play();
        }
        else if (buttonClicked(GameData::continueButton.button))
        {
            GameData::pause = false;
            play();
        }
        else if (buttonClicked(GameData::quit.button))
        {
            Window::setShouldClose(true);
        }
        Window::swapBuffers();
    }
    serialize();
    Window::terminate();
    Event::terminate();
    Texture_Renderer::terminate();
    Speaker::terminate();
    GameData::deleteResources();
    closeEngine();
    return EXIT_SUCCESS;
}

int main(int argc, char **argv)
{
    if (argc > 1 && std::string(argv[1]) == "1")
    {
        GameData::rezisable = true; // Experimental settings
    }
    GameData::programmPath = getProgrammPath(argv[0]);
    if (loadEngine(GameData::programmPath + "lib/") == -1)
        return EXIT_FAILURE;
    return start_game(argv[0]);
}
