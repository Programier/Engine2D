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

typedef struct _Texture
{
	struct _TexturePrivateData *privateData = nullptr;
	float vertices[30];
	unsigned int id;
	int width;
	int height;
} Texture;

// Texture functions

void loadTexture(std::string textureFile, Texture &texture);
void setPart(Texture &texture, int x, int y, int width, int heigth);
void draw(Texture &texture, int x = 0, int y = 0, int width = 0, int height = 0, bool inversed = false);
void fullScreenDraw(Texture &texture);
int getInitialWidth(Texture &texture);
int getInitialHeight(Texture &texture);
void copyTexture(Texture &copy_from, Texture &copy_to);
void invertVertically(Texture &texture);
void invertHorizontally(Texture &texture);
void deleteTexture(Texture &texture);
void toDefault(Texture &texture);
void inheritTexture(Texture &inherit_from, Texture &to);
#endif /* TEXTURE_HPP */
