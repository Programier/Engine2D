#include "Engine.hpp"
#include <vector>

// Please remove the following definition if you compile this project on Windows
#define LINUX

#ifdef LINUX
#include <dlfcn.h>
#else
#include <windows.h>
#endif

#include <iostream>

static void *openLibrary(const char *path)
{
#ifdef LINUX
    return dlopen(path, RTLD_LAZY);
#else
    return LoadLibrary(path);
#endif
}

void *getSymLink(void *library, const char *name)
{
#ifdef LINUX
    return dlsym(library, name);
#else
    GetProcAddress((HINSTANCE)library, name);
#endif
}

GLFWwindow *Window::window;
int Window::width, Window::height;

typedef struct Functions
{
    void *lib;
    bool inited;
    std::vector<void *> func;
} Functions;

static std::vector<Functions> _functions;

void closeLibrary(void *library)
{

#ifdef LINUX
    dlclose(library);
#else
    FreeLibrary((HINSTANCE)library);
#endif
}

int loadLib(std::string libpath, std::vector<std::string> &func_name)
{
    int func_index = _functions.size();
    _functions.push_back(Functions());
    std::cout << "Engine Loader: Start loading " << libpath << " library" << std::endl;
    _functions[func_index].lib = openLibrary(libpath.c_str());
    if (_functions[func_index].lib == nullptr)
    {
        std::cerr << "Engine Loader: Failed to load " << libpath << " library" << std::endl;
        _functions.erase(_functions.begin() + func_index);
        return -1;
    }
    std::cout << "Engine Loader: Library loaded successfully.\nEngine Loader: Loading functions from this library" << std::endl;
    for (int i = 0; i < func_name.size(); i++)
    {
        _functions[func_index].func.push_back(getSymLink(_functions[func_index].lib, func_name[i].c_str()));
        if (_functions[func_index].func[i] == nullptr)
        {
            std::cerr << "Engine Loader: Failed to load \"" << func_name[i] << "\" function" << std::endl;
            return -1;
        }
    }
    _functions[func_index].inited = true;
    std::cout << "Engine Loader: All functions loaded successfully from " << libpath << " library" << std::endl;
    return 0;
}

void closeEngine()
{
    for (auto ell : _functions)
    {
        closeLibrary(ell.lib);
    }
    _functions.clear();
}

int loadEngine(const std::string enginePath)
{
    // Window library loading
    std::vector<std::string> funcs = {"init", "terminate", "isOpen", "setShouldClose", "swapBuffers", "clearBuffer", "isInited"};
    std::string _path = enginePath + "Window.lib";
    if (loadLib(_path, funcs) == -1)
    {
        closeEngine();
        return -1;
    }

    // Event library loading
    _path = enginePath + "Event.lib";
    funcs = {"init", "terminate", "pollEvents", "k_pressed", "k_jpressed", "m_pressed", "m_jpressed", "getX", "getY", "isInited"};

    if (loadLib(_path, funcs) == -1)
    {
        closeEngine();
        return -1;
    }

    // Music library loading
    _path = enginePath + "Music.lib";
    funcs = {"init", "terminate", "play", "stop", "isPlaying", "stopLastInStack", "stopStackAt", "clearStack", "isInited", "loadWAV", "deleteMusic"};
    if (loadLib(_path, funcs) == -1)
    {
        closeEngine();
        return -1;
    }
    _path = enginePath + "Graphics.lib";
    funcs = {"loadTexture", "setPart", "getPartFrom", "inheritPartFrom",
             "draw", "getInitialWidth", "getInitialHeight", "copyTexture",
             "invertVertically", "invertHorizontally", "deleteTexture",
             "toDefault", "inheritTexture", "createButton", "drawButton",
             "deleteButton", "ButtonClicked", "initTextureRenderer",
             "terminate", "isInited", "initGraphics"};

    if (loadLib(_path, funcs) == -1)
    {
        closeEngine();
        return -1;
    }

    ((void (*)(bool (*)(), int (*)(), int (*)(), bool (*)(int)))_functions[3].func[20])(((bool (*)())_functions[1].func[9]),
                                                                                        ((int (*)())_functions[1].func[7]),
                                                                                        ((int (*)())_functions[1].func[8]),
                                                                                        ((bool (*)(int))_functions[1].func[5]));
    std::cout << "Engine Loader: Engine loaded succesfully" << std::endl;
    return 0;
}

// Implementing function calls from the Window.lib library
int Window::init(int window_width, int window_height, const char *windowname, bool rezisable)
{
    if (_functions[0].inited)
    {
        if (((int (*)(int, int, const char *, bool))(_functions[0].func[0]))(window_width, window_height, windowname, rezisable) == -1)
            return -1;

        Window::window = *(GLFWwindow **)getSymLink(_functions[0].lib, "_window");
        if (Window::window == nullptr)
        {
            return -1;
        }
        Window::width = window_width;
        Window::height = window_height;
        return 0;
    }
    else
    {
        std::cerr << "Please, init Engine first" << std::endl;
        return -1;
    }
}

void Window::terminate()
{
    if (_functions[0].inited)
    {
        ((void (*)())(_functions[0].func[1]))();
    }
}

bool Window::isOpen()
{
    return (_functions[0].inited) ? ((bool (*)())(_functions[0].func[2]))() : false;
}
void Window::setShouldClose(bool flag)
{
    if (_functions[0].inited)
        ((void (*)(bool))(_functions[0].func[3]))(flag);
}

void Window::swapBuffers()
{
    if (_functions[0].inited)
        ((void (*)())(_functions[0].func[4]))();
}

void Window::clearBuffer()
{
    if (_functions[0].inited)
        ((void (*)())(_functions[0].func[5]))();
}

bool Window::isInited()
{
    return (_functions[0].inited) ? ((bool (*)())(_functions[0].func[6]))() : false;
}

//Implementing function calls from the Event.lib library

int Event::init()
{
    return (_functions[1].inited) ? ((int (*)(GLFWwindow *, int *, int *))_functions[1].func[0])(Window::window, &Window::width, &Window::height) : -1;
}

void Event::terminate()
{
    if (_functions[1].inited)
        ((void (*)())_functions[1].func[1])();
}

void Event::pollEvents()
{
    if (_functions[1].inited)
        ((void (*)())_functions[1].func[2])();
}

bool Event::Keyboard::pressed(int keycode)
{
    return (_functions[1].inited) ? ((bool (*)(int))_functions[1].func[3])(keycode) : false;
}
bool Event::Keyboard::jpressed(int keycode)
{
    return (_functions[1].inited) ? ((bool (*)(int))_functions[1].func[4])(keycode) : false;
}

bool Event::Mouse::pressed(int keycode)
{
    return (_functions[1].inited) ? ((bool (*)(int))_functions[1].func[5])(keycode) : false;
}
bool Event::Mouse::jpressed(int keycode)
{
    return (_functions[1].inited) ? ((bool (*)(int))_functions[1].func[6])(keycode) : false;
}
int Event::Mouse::getX()
{
    return (_functions[1].inited) ? ((int (*)())_functions[1].func[7])() : -1;
}
int Event::Mouse::getY()
{
    return (_functions[1].inited) ? ((int (*)())_functions[1].func[8])() : -1;
}

bool Event::isInited()
{
    return (_functions[1].inited) ? ((bool (*)())_functions[1].func[9])() : false;
}

//Implementing function calls from the Music.lib library
int Speaker::init()
{
    return (_functions[2].inited) ? ((int (*)())_functions[2].func[0])() : -1;
}
void Speaker::terminate()
{
    if (_functions[2].inited)
        ((void (*)())_functions[2].func[1])();
}
void Speaker::play(Music &music)
{
    if (_functions[2].inited)
        ((void (*)(Music &))_functions[2].func[2])(music);
}
void Speaker::stop(Music &music)
{
    if (_functions[2].inited)
        ((void (*)(Music &))_functions[2].func[2])(music);
}
bool Speaker::isPlaying(Music &music)
{
    return (_functions[2].inited) ? ((bool (*)(Music &))_functions[2].func[4])(music) : false;
}
void Speaker::stopLastInStack()
{
    if (_functions[2].inited)
        ((void (*)())_functions[2].func[5])();
}
void Speaker::stopStackAt(int index)
{
    if (_functions[2].inited)
        ((void (*)(int))_functions[2].func[6])(index);
}
void Speaker::clearStack()
{
    if (_functions[2].inited)
        ((void (*)())_functions[2].func[7])();
}
bool Speaker::isInited()
{
    return (_functions[2].inited) ? ((bool (*)())_functions[2].func[8])() : false;
}

void loadWAV(std::string filename, Music &music)
{
    if (_functions[2].inited)
        ((void (*)(std::string, Music &))_functions[2].func[9])(filename, music);
}
void deleteMusic(Music &music)
{
    if (_functions[2].inited)
        ((void (*)(Music &))_functions[2].func[10])(music);
}

// Implementing function calls from the Graphics.lib library

// Texture
int loadTexture(std::string textureFile, Texture &texture)
{
    return (_functions[3].inited) ? ((int (*)(std::string, Texture &))_functions[3].func[0])(textureFile, texture) : -1;
}

void setPart(Texture &texture, int x, int y, int width, int heigth)
{
    if (_functions[3].inited)
        ((void (*)(Texture &, int, int, int, int))_functions[3].func[1])(texture, x, y, width, heigth);
}

Texture getPartFrom(Texture &texture, int x, int y, int width, int heigth)
{
    return (_functions[3].inited) ? ((Texture(*)(Texture &, int, int, int, int))_functions[3].func[2])(texture, x, y, width, heigth) : Texture();
}

Texture inheritPartFrom(Texture &texture, int x, int y, int width, int heigth)
{
    return (_functions[3].inited) ? ((Texture(*)(Texture &, int, int, int, int))_functions[3].func[3])(texture, x, y, width, heigth) : Texture();
}

void draw(Texture &texture, int x, int y, float angle, std::size_t width, std::size_t height, Inversion invetion_status)
{
    if (_functions[3].inited)
        ((void (*)(Texture &, int, int, float, std::size_t, std::size_t, Inversion))_functions[3].func[4])(texture, x, y, angle, width, height, invetion_status);
}

int getInitialWidth(Texture &texture)
{
    return (_functions[3].inited) ? ((int (*)(Texture &))_functions[3].func[5])(texture) : -1;
}

int getInitialHeight(Texture &texture)
{
    return (_functions[3].inited) ? ((int (*)(Texture &))_functions[3].func[6])(texture) : -1;
}

void copyTexture(Texture &copy_from, Texture &copy_to)
{
    if (_functions[3].inited)
        ((void (*)(Texture &, Texture &))_functions[3].func[7])(copy_from, copy_to);
}

void invertVertically(Texture &texture)
{
    if (_functions[3].inited)
        ((void (*)(Texture &))_functions[3].func[8])(texture);
}

void invertHorizontally(Texture &texture)
{
    if (_functions[3].inited)
        ((void (*)(Texture &))_functions[3].func[9])(texture);
}

void deleteTexture(Texture &texture)
{
    if (_functions[3].inited)
        ((void (*)(Texture &))_functions[3].func[10])(texture);
}

void toDefault(Texture &texture)
{
    if (_functions[3].inited)
        ((void (*)(Texture &))_functions[3].func[11])(texture);
}

void inheritTexture(Texture &inherit_from, Texture &to)
{
    if (_functions[3].inited)
        ((void (*)(Texture &, Texture &))_functions[3].func[12])(inherit_from, to);
}

// Button

void createButton(Button &button, Texture &button_texture, Texture &underCursor, Texture &pressed,
                  int x, int y, int width, int height, float angle, bool visible)
{
    if (_functions[3].inited)
        ((void (*)(Button &, Texture &, Texture &,
                   Texture &, int, int, int, int, float, bool))_functions[3]
             .func[13])(button, button_texture, underCursor, pressed, x, y, width, height, angle, visible);
}

void drawButton(Button &button)
{
    if (_functions[3].inited)
        ((void (*)(Button &))_functions[3].func[14])(button);
}

void deleteButton(Button &button)
{
    if (_functions[3].inited)
        ((void (*)(Button &))_functions[3].func[15])(button);
}

bool buttonClicked(Button &button)
{
    return (_functions[3].inited) ? ((bool (*)(Button &))_functions[3].func[16])(button) : false;
}

// Texture Renderer System

int Texture_Renderer::init()
{
    return (_functions[3].inited) ? ((int (*)())_functions[3].func[17])() : -1;
}

void Texture_Renderer::terminate()
{
    if (_functions[3].inited)
        ((void (*)())_functions[3].func[18])();
}

bool Texture_Renderer::isInited()
{
    return (_functions[3].inited) ? ((bool (*)())_functions[3].func[19])() : false;
}