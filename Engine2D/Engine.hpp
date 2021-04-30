#ifndef ENGINE_HPP
#define ENGINE_HPP
#include <string>
#include <GLFW/glfw3.h>

int loadEngine(std::string enginePath);
void closeEngine();

// Engine function prototypes

class GLFWwindow;

namespace Window
{
    extern GLFWwindow *window;
    extern int width, height;
    int init(int window_width, int window_height,
             const char *windowname, bool rezisable);
    void terminate();

    bool isOpen();
    void setShouldClose(bool flag);
    void swapBuffers();
    void clearBuffer();
    bool isInited();
} // namespace Window

namespace Event
{
    int init();
    void pollEvents();
    void terminate();
    namespace Keyboard
    {
        bool pressed(int keycode);
        bool jpressed(int keycode);
    } // namespace Keyboard

    namespace Mouse
    {
        bool pressed(int keycode);
        bool jpressed(int keycode);
        int getX();
        int getY();
    } // namespace Mouse

    bool isInited();

} // namespace Event

// Music
typedef struct Music
{
    struct _MusicData *privateField = nullptr;
} Music;

namespace Speaker
{
    int init();
    void terminate();
    void play(Music &music);
    bool isInited();
    void stop(Music &music);
    bool isPlaying(Music &music);
    void stopLastInStack();
    void stopStackAt(int index);
    void clearStack();
}

void loadWAV(std::string filename, Music &music);
void deleteMusic(Music &music);

// Texture
enum Inversion
{
    NONE,
    HORIZONTAL,
    VERTICAL
};

typedef struct _Texture
{
    struct _TexturePrivateData *privateData = nullptr;
    float vertices[30];
    unsigned int id;
    int width;
    int height;
} Texture;

// Texture functions

int loadTexture(std::string textureFile, Texture &texture);
void setPart(Texture &texture, int x, int y, int width, int heigth);
Texture getPartFrom(Texture &texture, int x, int y, int width, int heigth);
Texture inheritPartFrom(Texture &texture, int x, int y, int width, int heigth);
void draw(Texture &texture, int x = 0, int y = 0, float angle = 0, std::size_t width = 0, std::size_t height = 0, Inversion invetion_status = NONE);
int getInitialWidth(Texture &texture);
int getInitialHeight(Texture &texture);
void copyTexture(Texture &copy_from, Texture &copy_to);
void invertVertically(Texture &texture);
void invertHorizontally(Texture &texture);
void deleteTexture(Texture &texture);
void toDefault(Texture &texture);
void inheritTexture(Texture &inherit_from, Texture &to);

// Button

typedef struct Button
{
    struct PrivateField *privateField;
    bool visible = true;
    int x = 0, y = 0, width = 0, height = 0;
    float angle = 0;
} Button;

void createButton(Button &button, Texture &button_texture, Texture &underCursor, Texture &pressed,
                  int x = 0, int y = 0, int width = 0, int height = 0, float angle = 0, bool visible = true);

void draw(Button &button);
void deleteButton(Button &button);
bool buttonClicked(Button &button);

// Texture Renderer
namespace Texture_Renderer
{
    int init();
    void terminate();
    bool isInited();
}

inline std::string getProgrammPath(std::string binaryFilePath)
{
    while (!binaryFilePath.empty() && binaryFilePath[binaryFilePath.length() - 1] != '/' && binaryFilePath[binaryFilePath.length() - 1] != '\\')
        binaryFilePath.erase(binaryFilePath.end() - 1);
    return binaryFilePath;
}

#endif