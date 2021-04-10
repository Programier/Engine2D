/*
 * Texture.cpp
 *
 *  Created on: 27 мар. 2021 г.
 *      Author: programier
 */

#include "Texture.hpp"
#include "Shader.hpp"
#include <GL/glew.h>
#include "Texture_Renderer.hpp"
#include <algorithm>
#include <cmath>
#include "Window.hpp"

float tmp[] = {
			// x    y     z     u     v
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,

			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, };

typedef struct _TexturePrivateData
{
	struct Dot{
		float x = 0, y = 0;
	};
	struct Dot dot_1, dot_2, dot_3, dot_4;
	std::string pathToFile;
	bool inited, inherit;
	int initial_width, initial_height;
} PrivateField;

PrivateField* writePrivateData(std::string filePath, bool inited, int width, int heigth, bool inherit = false){
	PrivateField *data = new PrivateField();
	data->inited = inited;
	data->pathToFile = filePath;
	data->initial_height = heigth;
	data->initial_width = width;
	data->inherit = inherit;
	return data;
}

static void rewrite_vertices(struct _Texture &texture,  int x, int y, int width, int height, bool add,
		bool inversed) {
	if(texture.privateData == nullptr){
		std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
		return;
	}
	y = -1 * (Window::height - height - y);
	float x_diff = ((1.0 * x) / (Window::width/2));
	float y_diff = ((1.0 * y) / (Window::height/2));
	float width_diff = (1.0 * width) / (Window::width / 2);
	float height_diff = (1.0 * height) / (Window::height / 2);

	if (add) {
		texture.privateData->dot_1.x = -1 + x_diff;
		texture.privateData->dot_1.y = 1 + y_diff;
		texture.privateData->dot_2.x = texture.privateData->dot_1.x + width_diff;
		texture.privateData->dot_2.y = texture.privateData->dot_1.y;
		texture.privateData->dot_3.x = texture.privateData->dot_1.x;
		texture.privateData->dot_3.y = texture.privateData->dot_1.y - height_diff;
		texture.privateData->dot_4.x = texture.privateData->dot_1.x + width_diff;
		texture.privateData->dot_4.y = texture.privateData->dot_1.y - height_diff;
	} else {
		texture.privateData->dot_1.x = -1;
		texture.privateData->dot_1.y = 1;
		texture.privateData->dot_2.x = 1;
		texture.privateData->dot_2.y = 1;
		texture.privateData->dot_3.x = -1;
		texture.privateData->dot_3.y = -1;
		texture.privateData->dot_4.x = 1;
		texture.privateData->dot_4.y = -1;
	}
	if (inversed)
		invertVertically(texture);

	texture.vertices[10] = texture.privateData->dot_1.x;
	texture.vertices[11] = texture.privateData->dot_1.y;
	texture.vertices[25] = texture.privateData->dot_1.x;
	texture.vertices[26] = texture.privateData->dot_1.y;
	texture.vertices[20] = texture.privateData->dot_2.x;
	texture.vertices[21] = texture.privateData->dot_2.y;
	texture.vertices[0] = texture.privateData->dot_3.x;
	texture.vertices[1] = texture.privateData->dot_3.y;
	texture.vertices[5] = texture.privateData->dot_4.x;
	texture.vertices[6] = texture.privateData->dot_4.y;
	texture.vertices[15] = texture.privateData->dot_4.x;
	texture.vertices[16] = texture.privateData->dot_4.y;

}

void Swap(float &a, float &b){
	float tmp = a;
	a = b;
	b = tmp;
}

void deleteTexture(struct _Texture& texture){
	if(!texture.privateData->inherit){
		std::cout << "TEXTURE: Start deleting texture" << std::endl;
		glDeleteTextures(1, &texture.id);
		delete texture.privateData;
		if(texture.privateData != nullptr)
			texture.privateData = nullptr;
	}else
		std::cerr << "TEXTURE: Can't delete inherited texture. Permission denied!" << std::endl;
}

void invertVertically(struct _Texture& texture) {
	if(texture.privateData == nullptr){
		std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
		return;
	}
	setPart(texture, texture.width, 0, -1 * texture.width, texture.height);
}

void invertHorizontally(struct _Texture& texture){
	if(texture.privateData == nullptr){
		std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
		return;
	}
	setPart(texture, 0, texture.height, texture.width, -1 * texture.height);
}

void draw(struct _Texture& texture, int x, int y, int width, int height, bool inversed) {
	if(texture.privateData == nullptr){
		std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
		return;
	}
	if(width == 0) width = texture.width;
	if(height == 0) height = texture.height;
	rewrite_vertices(texture, x, y, width, height, true, inversed);

	/*We have changed the rendering parameters, so the drawing function will temporarily draw according to the specified parameters*/

	fullScreenDraw(texture);
	glBindVertexArray(0);
	rewrite_vertices(texture, x, y, width, height, false, inversed);
}

void fullScreenDraw(struct _Texture& texture){
	if(texture.privateData == nullptr){
		std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
		return;
	}
	if(!Texture_Renderer::isInited()){
		std::cerr << "TEXTURE: Please, initialize the Texture Renderer system first before drowing the texture" << std::endl;
		return;
	}
	glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), texture.vertices,
	GL_STATIC_DRAW);
	Shader::use();
	glBindTexture(GL_TEXTURE_2D, texture.id);
	glBindVertexArray(Texture_Renderer::VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void setPart(struct _Texture& texture, int x,  int y,  int _width,
		 int _height) {
	if(texture.privateData == nullptr){
		std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
		return;
	}
		// Check inverting
	bool inverted_x = (texture.vertices[3] > texture.vertices[8]);
	bool inverted_y = (texture.vertices[4] > texture.vertices[14]);
	float begin_x = texture.vertices[3], begin_y = texture.vertices[4];

	if(!inverted_x){
		x += static_cast<int>(begin_x * getInitialWidth(texture));
	}else{
		x = static_cast<int>(begin_x * getInitialWidth(texture)) - x;
		_width *= -1;
	}

	if(inverted_y){
		y = static_cast<int>(begin_y * getInitialHeight(texture)) - y;
		_height *= -1;
	}
	else{
		y += static_cast<int>(begin_y * getInitialHeight(texture));
	}
	begin_x = x + _width, begin_y = y + _height;
	if (static_cast<int>(begin_x) > getInitialWidth(texture) || begin_x < 0.f || static_cast<int>(begin_y) > getInitialWidth(texture) || begin_y < 0.f)
	{
		std::cerr << "TEXTURE: Failed to resize a texture" << std::endl;
		return;
	}
	toDefault(texture);
	texture.width = abs(_width);
	texture.height = abs(_height);
	float x_diff = ((1.0 * x) / texture.privateData->initial_width);
	float y_diff = ((1.0 * y) / texture.privateData->initial_height);
	float width_diff = (1.0 * _width) / texture.privateData->initial_width;
	float height_diff = (1.0 * _height) / texture.privateData->initial_height;
	

	texture.vertices[3] = x_diff;
	texture.vertices[4] = y_diff;

	texture.vertices[8] = x_diff + width_diff;
	texture.vertices[9] = y_diff;

	texture.vertices[13] = x_diff;
	texture.vertices[14] = y_diff + height_diff;

	texture.vertices[18] = x_diff + width_diff;
	texture.vertices[19] = y_diff;

	texture.vertices[23] = x_diff + width_diff;
	texture.vertices[24] = y_diff + height_diff;

	texture.vertices[28] = x_diff;
	texture.vertices[29] = y_diff + height_diff;;

}

static void setVectices(float* vertices){
	std::copy(tmp, tmp + 30, vertices);
}

int getInitialHeight(struct _Texture& texture) {
	if(texture.privateData == nullptr){
		std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
		return -1;
	}
		return texture.privateData->initial_height;
}

int getInitialWidth(struct _Texture& texture) {
	if(texture.privateData == nullptr){
		std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
		return -1;
	}
	return texture.privateData->initial_width;
}

void copyTexture(Texture &copy_from, Texture &copy_to){
	if(copy_from.privateData == nullptr){
		std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
		return;
	}
	loadTexture(copy_from.privateData->pathToFile, copy_to);
}

void loadTexture(std::string textureFile, struct _Texture& texture){
	setVectices(texture.vertices);
	texture.id = _png_load(textureFile.c_str(), &texture.width, &texture.height);
	if(texture.id == 0)
		std::cerr << "TEXTURE: Failed to load texture " << textureFile << std::endl;
	texture.privateData = writePrivateData(textureFile, true, texture.width, texture.height);
}

void toDefault(struct _Texture& texture){
	setVectices(texture.vertices);
	texture.width = getInitialWidth(texture);
	texture.height = getInitialHeight(texture);
}

void inheritTexture(Texture &inherit_from, Texture &to){
	to.id = inherit_from.id;
	to.width = inherit_from.width;
	to.height = inherit_from.height;
	std::copy(inherit_from.vertices, inherit_from.vertices + 30, to.vertices);
	if (to.privateData != nullptr)
		delete to.privateData;
	to.privateData = writePrivateData(inherit_from.privateData->pathToFile, true, getInitialWidth(inherit_from), getInitialHeight(inherit_from), true);
}
