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
#include "Window.hpp"

static void rewrite_vertices(struct _Texture &texture,  int x, int y, int width, int height, bool add,
		bool inversed) {
	y = -1 * (Window::height - height - y);
	float x_diff = ((1.0 * x) / (Window::width/2));
	float y_diff = ((1.0 * y) / (Window::height/2));
	float width_diff = (1.0 * width) / (Window::width / 2);
	float height_diff = (1.0 * height) / (Window::height / 2);

	if (add) {
		texture.dot_1.x = -1 + x_diff;
		texture.dot_1.y = 1 + y_diff;
		texture.dot_2.x = texture.dot_1.x + width_diff;
		texture.dot_2.y = texture.dot_1.y;
		texture.dot_3.x = texture.dot_1.x;
		texture.dot_3.y = texture.dot_1.y - height_diff;
		texture.dot_4.x = texture.dot_1.x + width_diff;
		texture.dot_4.y = texture.dot_1.y - height_diff;
	} else {
		texture.dot_1.x = -1;
		texture.dot_1.y = 1;
		texture.dot_2.x = 1;
		texture.dot_2.y = 1;
		texture.dot_3.x = -1;
		texture.dot_3.y = -1;
		texture.dot_4.x = 1;
		texture.dot_4.y = -1;
	}
	if (inversed)
		invertVertically(texture);

	texture.vertices[10] = texture.dot_1.x;
	texture.vertices[11] = texture.dot_1.y;
	texture.vertices[25] = texture.dot_1.x;
	texture.vertices[26] = texture.dot_1.y;
	texture.vertices[20] = texture.dot_2.x;
	texture.vertices[21] = texture.dot_2.y;
	texture.vertices[0] = texture.dot_3.x;
	texture.vertices[1] = texture.dot_3.y;
	texture.vertices[5] = texture.dot_4.x;
	texture.vertices[6] = texture.dot_4.y;
	texture.vertices[15] = texture.dot_4.x;
	texture.vertices[16] = texture.dot_4.y;

}

void Swap(float &a, float &b){
	float tmp = a;
	a = b;
	b = tmp;
}

void deleteTexture(struct _Texture& texture){
	std::cout << "Start deleting texture" << std::endl;
	glDeleteTextures(1, &texture.id);
}

void invertVertically(struct _Texture& texture) {
#ifdef DEBUG
	std::cout << "BEGIN" << std::endl;
	for(int i = 3; i < 30; i += 5){
		std::cout << texture.vertices[i] << "\t" << texture.vertices[i+1] << std::endl;
	}
#endif
	Swap(texture.vertices[3], texture.vertices[8]);
	Swap(texture.vertices[4], texture.vertices[9]);
	texture.vertices[18] = texture.vertices[8];
	texture.vertices[19] = texture.vertices[9];

	Swap(texture.vertices[13], texture.vertices[23]);
	Swap(texture.vertices[14], texture.vertices[24]);
	texture.vertices[28] = texture.vertices[13];
	texture.vertices[29] = texture.vertices[14];
#ifdef DEBUG
	std::cout << "END" << std::endl;
	for(int i = 3; i < 30; i += 5){
		std::cout << texture.vertices[i] << "\t" << texture.vertices[i+1] << std::endl;
	}
#endif
}

void draw(struct _Texture& texture, int x, int y, int width, int height, bool inversed) {
	if(width == 0) width = texture.width;
	if(height == 0) height = texture.height;
	rewrite_vertices(texture, x, y, width, height, true, inversed);

	/*We have changed the rendering parameters, so the drawing function will temporarily draw according to the specified parameters*/

	fullScreenDraw(texture);
	glBindVertexArray(0);
	rewrite_vertices(texture, x, y, width, height, false, inversed);
}

void fullScreenDraw(struct _Texture& texture){
	if(!Texture_Renderer::isInited()){
		std::cerr << "Please, initialize the Texture Renderer system first before drowing the texture" << std::endl;
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

void setPart(struct _Texture& texture, unsigned int x, unsigned int y, unsigned int _width,
		unsigned int _height) {
	if (x + _width > static_cast<unsigned int>(texture.initial_width) || y + _height > static_cast<unsigned int>(texture.initial_height)) {
		std::cerr << "Failed to resize a texture" << std::endl;
		return;
	}
	texture.width = _width;
	texture.height = _height;
	float x_diff = ((1.0 * x) / texture.initial_width);
	float y_diff = ((1.0 * y) / texture.initial_height);
	float width_diff = (1.0 * texture.width) / texture.initial_width;
	float height_diff = (1.0 * texture.height) / texture.initial_height;

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
	texture.vertices[29] = y_diff + height_diff;
}

int getInitialHeight(struct _Texture& texture) {
	return texture.initial_height;
}

int getInitialWidth(struct _Texture& texture) {
	return texture.initial_width;
}

void copyTexture(Texture &copy_from, Texture &copy_to){
	copy_to.dot_1 = copy_from.dot_1;
	copy_to.dot_2 = copy_from.dot_2;
	copy_to.dot_3 = copy_from.dot_3;
	copy_to.dot_4 = copy_from.dot_4;
	copy_to.initial_height = copy_from.initial_height;
	copy_to.initial_width = copy_from.initial_width;
	copy_to.height = copy_from.height;
	copy_to.width = copy_from.width;
	copy_to.id = copy_from.id;
	std::copy(copy_from.vertices, copy_from.vertices + 30, copy_to.vertices);
}

static void setVectices(float* vertices){
	float tmp[] = {
			// x    y     z     u     v
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,

			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f, };
	std::copy(tmp, tmp + 30, vertices);
}

void loadTexture(std::string textureFile, struct _Texture& texture){
	setVectices(texture.vertices);
	texture.id = _png_load(textureFile.c_str(), &texture.width, &texture.height);
	if(texture.id == 0)
		std::cerr << "Failed to load texture " << textureFile << std::endl;
	texture.initial_height = texture.height;
	texture.initial_width = texture.width;
}
