/*
 * Texture.hpp
 *
 *  Created on: 27 мар. 2021 г.
 *      Author: programier
 */

#ifndef TEXTURE_HPP
#define TEXTURE_HPP

#include <string>
#include "Texture_Loader.hpp"

typedef struct _Texture {
	struct Dot{
		float x = 0, y = 0;
	};
	Dot dot_1, dot_2, dot_3, dot_4;
	int	initial_width, initial_height;
	float vertices[30];
	unsigned int id;
	int width;
	int height;
} Texture;

// Texture functions

void loadTexture(std::string textureFile, Texture& texture);
void setPart(Texture& texture, unsigned int x, unsigned int y, unsigned int width, unsigned int heigth);
void draw(Texture& texture, int x = 0, int y = 0, int width = 0, int height = 0, bool inversed = false);
void fullScreenDraw(Texture& texture);
int getInitialWidth(Texture& texture);
int getInitialHeight(Texture& texture);
void copyTexture(Texture &copy_from, Texture &copy_to);
void invertVertically(Texture& texture);
void deleteTexture(Texture &texture);
#endif /* TEXTURE_HPP */
