#include "Engine2D/Engine.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <chrono>
#include <fstream>

#define GROUND_SPEED 3.5
#define BIRD_X 250
#define BIRD_FRAME_SPEED 0.2
#define GROUND_POS_Y 98
#define JUMP 7
#define DIGIT_SIZE 32
#define COLUMN_DISTANCE_Y 140
#define COLUMN_DISTANCE_X 250
#define COLUMN_DIFF_Y 230
#define MAX_FALLING_SPEED -15
#define MIN_WINDOW_WIDTH 800
#define MIN_WINDOW_HEIGHT 400
#define WINDOW_INITIAL_HEIGHT 720.0
#define OUT_BINARY_FILE "state.bin"

namespace GameData
{
    bool rezisable = true, gameOver = false, pause = true, gameOverAudioIsReproduced = true, gameStarted = false;
    bool change_column = false;
    std::string path;
    double diff = 1;
    unsigned int score = 0, max_score = 0;
    int FPS = 60, tmp_FPS = 60, column_width, colums_count = 0;
    Texture _column, fps_texture, best_score;

    struct
    {
        Texture base;
        struct
        {
            Texture txr;
            double x = 0, height;
        } ground, sky;

        double width, initial_width, initial_height;
        int frames = 2;
    } background;

    struct
    {
        double y = 350;
        const double x = BIRD_X;
        double speed_y = 0;
        Texture texture[4];
        double frame = 0;
        int size_x, size_y;
        float angle = 0;
        float prev_angle = 0;
    } bird;

    typedef struct _Column
    {
        double x, y_upper, y_lower;
    } Column;
    std::vector<Column> columns;

    struct
    {
        Texture txr;
        Button button;
    } quit, play, _continue;

    struct
    {
        Music wings, die, hit, point;
    } audio;

    struct
    {
        Texture baseTexture;
        Texture digits[10];
        int size;
    } digits;

    void resetData()
    {
        score = 0;
        pause = true;
        gameOver = false;
        gameOverAudioIsReproduced = false;
        bird.y = 350, bird.speed_y = 0;
        bird.frame = 0;
        bird.angle = 0, bird.prev_angle = 0;
        columns.clear();
        gameStarted = false;
        change_column = false;
    }

    int loadResources()
    {
        // Background
        loadTexture(path + "images/Background.png", background.base);
        background.width = background.base.width;
        background.ground.txr = inheritPartFrom(background.base, 0, 0, background.width, GROUND_POS_Y);
        background.sky.txr = inheritPartFrom(background.base, 0, GROUND_POS_Y, background.width, background.base.height - GROUND_POS_Y);
        background.initial_height = getInitialHeight(background.base);
        background.initial_width = getInitialWidth(background.base);

        // Buttons
        loadTexture(path + "images/continue.png", _continue.txr);
        createButton(_continue.button, _continue.txr, _continue.txr, _continue.txr);
        loadTexture(path + "images/play.png", play.txr);
        createButton(play.button, play.txr, play.txr, play.txr);
        loadTexture(path + "images/quit.png", quit.txr);
        createButton(quit.button, quit.txr, quit.txr, quit.txr);

        // Bird
        for (int i = 0; i < 4; i++)
            loadTexture(path + "images/flap" + std::to_string(i + 1) + ".png",
                        bird.texture[i]);
        bird.size_y = getInitialHeight(bird.texture[0]);
        bird.size_x = getInitialWidth(bird.texture[0]);

        // Column
        loadTexture(path + "images/column.png", _column);
        column_width = _column.width;

        loadWAV(path + "audio/sfx_wing.wav", audio.wings);
        loadWAV(path + "audio/sfx_die.wav", audio.die);
        loadWAV(path + "audio/sfx_hit.wav", audio.hit);
        loadWAV(path + "audio/sfx_point.wav", audio.point);

        loadTexture(path + "images/counts.png", digits.baseTexture);
        for (int i = 0; i < 10; i++)
        {
            inheritTexture(digits.baseTexture, digits.digits[i]);
            setPart(digits.digits[i], i * 102, 0, 102, 102);
        }

        loadTexture(path + "images/fps.png", fps_texture);
        loadTexture(path + "images/best.png", best_score);
        return 0;
    }

    void deleteResources()
    {
        // Background
        deleteTexture(background.base);

        // Buttons
        deleteTexture(_continue.txr);
        deleteTexture(play.txr);
        deleteTexture(quit.txr);

        // Bird
        for (int i = 0; i < 4; i++)
            deleteTexture(bird.texture[i]);

        deleteMusic(audio.wings);
        deleteMusic(audio.die);
        deleteMusic(audio.hit);
        deleteMusic(audio.point);

        deleteTexture(digits.baseTexture);
        deleteTexture(fps_texture);
        deleteTexture(best_score);
    }

    void updateDiff()
    {
        diff = Window::height / WINDOW_INITIAL_HEIGHT;
    }

    // Background part

    // Background motion function
    void moveBackground()
    {
        background.sky.x -= GROUND_SPEED / 3;
        if (background.sky.x * diff <= -1 * (background.width))
            background.sky.x = 0;

        background.ground.x -= GROUND_SPEED;
        if (background.ground.x * diff <= -1 * background.width)
            background.ground.x = 0;
    }

    /*
        Updating the number of frames required to draw the background to the full width of the window
    */
    void updateFrameCount()
    {
        background.width = background.initial_width * static_cast<double>(Window::height) / background.initial_height;
        background.frames = Window::width * 2 / background.width;
        if (background.frames * background.width < Window::width * 2)
            background.frames++;
        if (background.frames < 2)
            background.frames = 2;
    }

    // Sky rendering function
    void drawSky()
    {
        int _x;
        for (int i = 0; i < background.frames; i++)
        {
            _x = background.sky.x * diff + i * background.width - 2 * i;
            draw(background.sky.txr, _x, GROUND_POS_Y * diff, 0, background.width, Window::height - GROUND_POS_Y * diff);
        }
    }

    // Ground rendering function
    void drawGround()
    {
        int _x;
        for (int i = 0; i < background.frames; i++)
        {
            _x = background.ground.x * diff + (i * background.width - 2 * i);
            draw(background.ground.txr, _x, -1, 0, background.width, GROUND_POS_Y * diff + 1);
        }
    }

    // Bird part

    void checkKeys()
    {
        if (Event::Keyboard::jpressed(GLFW_KEY_SPACE))
        {
            bird.speed_y = JUMP;
            Speaker::play(audio.wings);
        }

        if (Event::Keyboard::jpressed(GLFW_KEY_ESCAPE))
        {
            pause = true;
        }
    }
    bool updateBirdData()
    {
        if (bird.y >= GROUND_POS_Y + 10)
        {
            if (!pause)
            {
                bird.frame = (bird.speed_y < -6 * diff) ? 3 : bird.frame + (BIRD_FRAME_SPEED * 60 / FPS);
                bird.y += bird.speed_y * 60.0 / FPS;
                if (bird.y + bird.size_y > WINDOW_INITIAL_HEIGHT)
                    bird.y = WINDOW_INITIAL_HEIGHT - bird.size_y;
                bird.speed_y -= 0.4 * 60. / FPS;
                if (bird.speed_y < MAX_FALLING_SPEED)
                    bird.speed_y = MAX_FALLING_SPEED;
                if (bird.frame >= 4)
                    bird.frame = 0;

                bird.angle = atan(fabs((bird.speed_y * 60.0 / FPS) / (2 * GROUND_SPEED))) * 180 / M_PI;
                bird.angle = bird.prev_angle + (bird.angle - bird.prev_angle) / 1.5;
                if ((bird.speed_y * 60.0 / FPS) < 0)
                {
                    bird.angle *= -1.0;
                }
            }
            return true;
        }
        return false;
    }

    void drawBird()
    {
        draw(bird.texture[static_cast<int>(bird.frame)], bird.x * diff, bird.y * diff, bird.angle, bird.size_x * diff, bird.size_y * diff);
    }

    // Column part

    void moveColumn()
    {
        for (std::size_t i = 0; i < columns.size(); i++)
            columns[i].x -= GROUND_SPEED * 60 / FPS;
    }

    void updateColumn()
    {

        auto getMinLimit = [](int index) -> int
        {
            int min = COLUMN_DIFF_Y;
            if (columns[index].y_lower - min + _column.height < 10)
                min = -1 * (10 - _column.height - columns[index].y_lower);
            return min;
        };

        auto getMaxLimit = [](int index) -> int
        {
            int max = COLUMN_DIFF_Y;
            if (columns[index].y_lower + max + _column.height > WINDOW_INITIAL_HEIGHT)
                max = (WINDOW_INITIAL_HEIGHT - _column.height - columns[index].y_lower);
            return max;
        };

        // Deleting first column
        while (!columns.empty() && columns[0].x * diff + column_width * diff < 0)
        {
            columns.erase(columns.begin());
        }

        if (columns.size() == 0)
        {
            columns.push_back(Column());
            columns[0].x = Window::width + COLUMN_DISTANCE_X;
            columns[0].y_lower = (-1 * (_column.height - GROUND_POS_Y)) + rand() % (static_cast<int>((_column.height - COLUMN_DISTANCE_Y - GROUND_POS_Y)));
            columns[0].y_upper = columns[0].y_lower + (_column.height + COLUMN_DISTANCE_Y);
        }

        for (colums_count = 0; columns[colums_count].x * diff < Window::width; colums_count++)
        {
            if (colums_count + 1 >= static_cast<int>(columns.size()))
            {
                columns.push_back(Column());
                columns[colums_count + 1].x = columns[colums_count].x + COLUMN_DISTANCE_X;
                int tmp = getMinLimit(colums_count);
                columns[colums_count + 1].y_lower = columns[colums_count].y_lower + (rand() % (getMaxLimit(colums_count) + tmp) - tmp);
                if (columns[colums_count + 1].y_lower + _column.height + COLUMN_DISTANCE_Y > WINDOW_INITIAL_HEIGHT - 10)
                    columns[colums_count + 1].y_lower = WINDOW_INITIAL_HEIGHT - COLUMN_DISTANCE_Y - _column.height - 10;

                if (columns[colums_count + 1].y_lower + _column.height <= GROUND_POS_Y + 10)
                    columns[colums_count + 1].y_lower = GROUND_POS_Y + 10 - _column.height;

                columns[colums_count + 1].y_upper = columns[colums_count + 1].y_lower + (_column.height + COLUMN_DISTANCE_Y);
            }
        }
    }

    void checkCollision()
    {
        if (!columns.empty())
        {
            if ((bird.x >= columns[change_column].x + column_width - bird.size_y / 2) &&
                (bird.x < GROUND_SPEED + columns[change_column].x + column_width -
                              bird.size_y / 2))
            {
                score++;
                Speaker::play(audio.point);
            }

            // Check collisions
            if (bird.y < columns[change_column].y_lower + WINDOW_INITIAL_HEIGHT || bird.y + bird.size_y > columns[change_column].y_upper)
                if (bird.x + bird.size_x - 3 >= columns[change_column].x &&
                    bird.x - 5 <= columns[change_column].x + column_width)
                {
                    gameOver = true;
                }

            if (bird.y <= GROUND_POS_Y + 10)
            {
                gameOver = true;
            }
            if (!change_column && columns[0].x + column_width < bird.x)
            {
                change_column = true;
                gameStarted = true;
                std::cout << "GAME: Column index for collision detection changed" << std::endl;
            }

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

    void drawColumn()
    {
        for (int i = 0; i < colums_count; i++)
        {
            draw(_column, columns[i].x * diff, columns[i].y_lower * diff, 0, column_width * diff, Window::height);
            draw(_column, columns[i].x * diff, columns[i].y_upper * diff, 0, column_width * diff, Window::height, HORIZONTAL);
        }
    }

    std::vector<int> countToVector(int value)
    {
        std::vector<int> _digits;

        if (value == 0)
            _digits.push_back(0);
        else
            while (value != 0)
            {
                _digits.insert(_digits.begin(), value % 10);
                value /= 10;
            }
        return _digits;
    }

    void drawScore()
    {
        digits.size = DIGIT_SIZE * diff;
        std::vector<int> _digits = countToVector(score);
        int begin = (Window::width / 2) - digits.size + (_digits.size() % 2 == 0) * (digits.size / 2) - ((_digits.size() / 2) * digits.size);
        int y_pos = Window::height - digits.size - 8;
        for (std::size_t i = 0; i < _digits.size(); i++)
            draw(digits.digits[_digits[i]], begin + i * digits.size, y_pos, 0, digits.size, digits.size);
    }
    void drawFPS()
    {
        auto _digits = countToVector(GameData::tmp_FPS);
        int __width = fps_texture.width * diff / 2;
        int __height = fps_texture.height * diff / 2;
        int y_pos = Window::height - __height;
        int begin = 10 * diff;
        draw(GameData::fps_texture, begin, y_pos, 0, __width, __height);
        begin += __width;
        for (std::size_t i = 0; i < _digits.size(); i++)
            draw(digits.digits[_digits[i]], begin + i * digits.size, y_pos, 0, digits.size, digits.size);
    }

    void drawBestScore()
    {
        auto _digits = countToVector(max_score);
        int __width = best_score.width * diff / 2;
        int __height = best_score.height * diff / 2;
        int y_pos = Window::height - __height;
        int begin = Window::width - __width - digits.size * _digits.size();
        draw(GameData::best_score, begin, y_pos, 0, __width, __height);
        begin += __width;
        for (std::size_t i = 0; i < _digits.size(); i++)
            draw(digits.digits[_digits[i]], begin + i * digits.size, y_pos, 0, digits.size, digits.size);
    }
}

void startTimer()
{
    auto begin = std::chrono::steady_clock::now();
    int time;
    while ((time = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - begin).count()) < 3000 && Window::isOpen())
    {
        Event::pollEvents();
        Window::clearBuffer();
        GameData::updateDiff();
        GameData::updateFrameCount();
        GameData::drawSky();
        GameData::drawColumn();
        GameData::drawGround();
        GameData::drawBird();
        GameData::drawScore();
        GameData::drawFPS();
        GameData::drawBestScore();

        GameData::updateDiff();
        draw(GameData::digits.digits[3 - static_cast<int>(time / 1000)], Window::width / 2 - DIGIT_SIZE * GameData::diff, Window::height / 2, 0, DIGIT_SIZE * 2 * GameData::diff, DIGIT_SIZE * 2 * GameData::diff);
        Window::swapBuffers();
    }

    GameData::pause = false;
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
        GameData::updateDiff();
        GameData::updateFrameCount();
        GameData::checkKeys();
        GameData::updateBirdData();
        GameData::moveBackground();
        GameData::moveColumn();
        GameData::updateColumn();
        GameData::checkCollision();
        GameData::drawSky();
        GameData::drawColumn();
        GameData::drawGround();
        GameData::drawBird();
        GameData::drawScore();
        GameData::drawFPS();
        GameData::drawBestScore();
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
    while (Window::isOpen() && GameData::updateBirdData())
    {
        Event::pollEvents();
        Window::clearBuffer();
        GameData::updateDiff();
        GameData::updateFrameCount();
        GameData::updateColumn();
        GameData::checkCollision();
        GameData::drawSky();
        GameData::drawColumn();
        GameData::drawGround();
        GameData::drawBird();
        GameData::drawScore();
        GameData::drawFPS();
        GameData::drawBestScore();
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
    std::ifstream file(GameData::path + OUT_BINARY_FILE, std::ios::binary);
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
        _read(file, &GameData::background.sky.x);
        _read(file, &GameData::background.ground.x);
        _read(file, &GameData::diff);
        _read(file, &GameData::background.width);
        _read(file, &GameData::background.frames);
        _read(file, &GameData::column_width);
        _read(file, &GameData::score);
        _read(file, &GameData::max_score);
        _read(file, &GameData::gameStarted);
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
    std::ofstream file(GameData::path + OUT_BINARY_FILE, std::ios::binary);
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
        _write(file, &GameData::background.sky.x);
        _write(file, &GameData::background.ground.x);
        _write(file, &GameData::diff);
        _write(file, &GameData::background.width);
        _write(file, &GameData::background.frames);
        _write(file, &GameData::column_width);
        _write(file, &GameData::score);
        _write(file, &GameData::max_score);
        _write(file, &GameData::gameStarted);
        // Column positions
        int size = static_cast<int>(GameData::columns.size());
        _write(file, &size);
        for (int i = 0; i < size; i++)
        {
            GameData::columns.push_back(GameData::Column());
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

// Initialize the entire game, launch the start menu
void start()
{
    if (Window::init(1280, 720, "Flappy Bird", GameData::rezisable, MIN_WINDOW_WIDTH, MIN_WINDOW_HEIGHT) == -1)
        return;
    if (Event::init() == -1)
    {
        Window::terminate();
        return;
    }
    if (Texture_Renderer::init() == -1)
    {
        Event::terminate();
        Window::terminate();
        return;
    }
    Speaker::init();
    GameData::loadResources();
    deserialize();
    if (GameData::gameOver || !GameData::gameStarted)
    {
        GameData::resetData();
        GameData::gameOver = true;
        GameData::gameOverAudioIsReproduced = true;
    }

    while (Window::isOpen())
    {
        Event::pollEvents();
        Window::clearBuffer();

        // Background drawing
        GameData::updateDiff();
        GameData::updateFrameCount();
        GameData::updateBirdData();
        GameData::updateColumn();
        GameData::drawSky();
        GameData::drawColumn();
        GameData::drawBird();
        GameData::drawScore();
        GameData::drawFPS();
        GameData::drawBestScore();
        GameData::drawGround();

        // Buttons drawing
        GameData::_continue.button.x = Window::width / 2 - GameData::_continue.txr.width * GameData::diff / 2;
        GameData::_continue.button.y = Window::height / 2 + 5 * GameData::diff + GameData::_continue.txr.height / 2;
        GameData::_continue.button.width = GameData::_continue.txr.width * GameData::diff;
        GameData::_continue.button.height = GameData::_continue.txr.height * GameData::diff;
        GameData::_continue.button.visible = !GameData::gameOver;

        GameData::play.button.x = Window::width / 2 - GameData::play.txr.width * GameData::diff / 2;
        GameData::play.button.y = GameData::_continue.button.y - 5 * GameData::diff - GameData::play.txr.height * GameData::diff;
        GameData::play.button.width = GameData::play.txr.width * GameData::diff;
        GameData::play.button.height = GameData::play.txr.height * GameData::diff;

        GameData::quit.button.x = Window::width / 2 - GameData::_continue.txr.width * GameData::diff / 2;
        GameData::quit.button.y = GameData::play.button.y - 5 * GameData::diff - GameData::quit.txr.height * GameData::diff;
        GameData::quit.button.width = GameData::quit.txr.width * GameData::diff;
        GameData::quit.button.height = GameData::quit.txr.height * GameData::diff;

        draw(GameData::play.button);
        draw(GameData::_continue.button);
        draw(GameData::quit.button);

        if (buttonClicked(GameData::play.button))
        {
            GameData::resetData();
            GameData::updateColumn();
            play();
        }
        else if (buttonClicked(GameData::_continue.button))
        {
            play();
        }
        else if (buttonClicked(GameData::quit.button))
        {
            Window::setShouldClose(true);
        }
        Window::swapBuffers();
    }

    serialize();
    GameData::deleteResources();
    Speaker::terminate();
    Texture_Renderer::terminate();
    Event::terminate();
    Window::terminate();
}

int main(int argc, char **argv)
{
    GameData::path = getProgrammPath(argv[0]);
    if (loadEngine(GameData::path + "lib/") == -1)
        return EXIT_FAILURE;
    start();
    closeEngine();
}