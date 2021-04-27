#ifndef ENGINE_HPP
#define ENGINE_HPP

#include "Window.hpp"
#include "Event.hpp"
#include "Texture_Renderer.hpp"
#include "Shader.hpp"
#include "Texture.hpp"
#include "Music.hpp"
#include "Button.hpp"

inline std::string getProgrammPath(std::string binaryFilePath)
{
    while (!binaryFilePath.empty() && binaryFilePath[binaryFilePath.length() - 1] != '/' && binaryFilePath[binaryFilePath.length() - 1] != '\\')
        binaryFilePath.erase(binaryFilePath.end() - 1);
    return binaryFilePath;
}

#endif