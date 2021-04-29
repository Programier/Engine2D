/*
 * Texture_Renderer.hpp
 *
 *  Created on: 27 мар. 2021 г.
 *      Author: programier
 */

#ifndef TEXTURE_RENDERER_HPP
#define TEXTURE_RENDERER_HPP
#include "GLFW/glfw3.h"
extern "C"
{
	namespace Texture_Renderer
	{
		extern GLuint VAO, VBO;
		int initTextureRenderer();
		void terminate();
		bool isInited();
	}
}

#endif /* TEXTURE_RENDERER_HPP */
