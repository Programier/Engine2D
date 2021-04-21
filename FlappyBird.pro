TEMPLATE = app
CONFIG += console c++11
CONFIG -= app_bundle
CONFIG -= qt
LIBS += -lglfw -lpng -lGL -lGLEW -lalut -lopenal -laudio

SOURCES += \
        Engine2D/Texture_Loader.cpp \
        Engine2D/Texture_Renderer.cpp \
        Engine2D/Event.cpp \
        Engine2D/Shader.cpp \
        Engine2D/Window.cpp \
        Engine2D/Music.cpp \
        Engine2D/Texture.cpp \
        game.cpp

HEADERS += \
        Engine.hpp  \
        Event.hpp   \
        Music.hpp   \
        Shader.hpp  \
        Texture.hpp \
        Texture_Loader.hpp  \
        Texture_Renderer.hpp    \
        Window.hpp
