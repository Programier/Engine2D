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
#include <vector>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

extern "C"
{
	const float _vertices[] = {
		// x    y     z     u     v
		-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		-1.0f, 1.0f, 0.0f, 0.0f, 1.0f};

	typedef struct _TexturePrivateData
	{
		std::string pathToFile;
		bool inited, inherit;
		int initial_width, initial_height;
	} PrivateField;

	PrivateField *writePrivateData(std::string filePath, bool inited, int width, int heigth, bool inherit = false)
	{
		PrivateField *data = new PrivateField();
		data->inited = inited;
		data->pathToFile = filePath;
		data->initial_height = heigth;
		data->initial_width = width;
		data->inherit = inherit;
		return data;
	}

	void deleteTexture(struct _Texture &texture)
	{
		if(texture.privateData == nullptr)
			return;
		if (!texture.privateData->inherit)
		{
			std::cout << "TEXTURE: Start deleting texture" << std::endl;
			if(texture.id != 0)
				glDeleteTextures(1, &texture.id);
			if (texture.privateData != nullptr)
			{
				delete texture.privateData;
				texture.privateData = nullptr;
			}
		}
		else
			std::cerr << "TEXTURE: Can't delete inherited texture. Permission denied!" << std::endl;
	}

	void invertVertically(struct _Texture &texture)
	{
		if (texture.privateData == nullptr)
		{
			std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
			return;
		}
		setPart(texture, texture.width, 0, -1 * texture.width, texture.height);
	}

	void invertHorizontally(struct _Texture &texture)
	{
		if (texture.privateData == nullptr)
		{
			std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
			return;
		}
		setPart(texture, 0, texture.height, texture.width, -1 * texture.height);
	}

	void draw(struct _Texture &texture, int x, int y, float angle, std::size_t width, std::size_t height, Inversion invetion_status)
	{

		if (texture.privateData == nullptr)
		{
			std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
			return;
		}

		if (!Texture_Renderer::isInited())
		{
			std::cerr << "TEXTURE: Please, initialize the Texture Renderer system first before drowing the texture" << std::endl;
			return;
		}

		// Inverting the texture
		if (invetion_status != NONE)
		{
			switch (invetion_status)
			{
			case HORIZONTAL:
				invertHorizontally(texture);
				break;
			case VERTICAL:
				invertVertically(texture);
				break;

			default:
				break;
			}
		}

		// Angle reduction
		angle = fmod(angle, 360);
		if (angle < 0)
			angle += 360;

		if (width == 0)
			width = texture.width;

		if (height == 0)
			height = texture.height;

		// Setting vertices for drawing
		glBufferData(GL_ARRAY_BUFFER, 30 * sizeof(float), texture.vertices,
					 GL_STATIC_DRAW);

		Shader::use();
		// Calculating Texture Parameters
		float tmp = std::sqrt(static_cast<float>((width * width / 4.0) + (height * height / 4.0))) - (static_cast<float>(width) / 2);
		float _width = static_cast<float>(width) / (width + 2 * tmp);
		float _height = static_cast<float>(height) / (width + 2 * tmp);

		// Texture rendering
		glViewport(x - tmp, y - (tmp + (static_cast<float>(width) / 2) - (static_cast<float>(height) / 2)), width + tmp + tmp, width + tmp + tmp);
		glm::mat4 matrix = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, 0)), glm::radians(angle), glm::vec3(0, 0, 1)), glm::vec3(_width, _height, 0));
		Shader::useMatrix("matrix", matrix);
		glBindTexture(GL_TEXTURE_2D, texture.id);
		glBindVertexArray(Texture_Renderer::VAO);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glBindVertexArray(0);

		if (invetion_status != NONE)
		{
			switch (invetion_status)
			{
			case HORIZONTAL:
				invertHorizontally(texture);
				break;
			case VERTICAL:
				invertVertically(texture);
				break;

			default:
				break;
			}
		}
	}

	void setPart(struct _Texture &texture, int x, int y, int _width,
				 int _height)
	{
		if (texture.privateData == nullptr)
		{
			std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
			return;
		}
		// Check inverting
		bool inverted_x = (texture.vertices[3] > texture.vertices[8]);
		bool inverted_y = (texture.vertices[4] > texture.vertices[14]);
		float begin_x = texture.vertices[3], begin_y = texture.vertices[4];

		if (!inverted_x)
		{
			x += static_cast<int>(begin_x * getInitialWidth(texture));
		}
		else
		{
			x = static_cast<int>(begin_x * getInitialWidth(texture)) - x;
			_width *= -1;
		}

		if (inverted_y)
		{
			y = static_cast<int>(begin_y * getInitialHeight(texture)) - y;
			_height *= -1;
		}
		else
		{
			y += static_cast<int>(begin_y * getInitialHeight(texture));
		}
		begin_x = x + _width, begin_y = y + _height;
		if (static_cast<int>(begin_x) > getInitialWidth(texture) || begin_x < 0.f || static_cast<int>(begin_y) > getInitialHeight(texture) || begin_y < 0.f)
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
		texture.vertices[29] = y_diff + height_diff;
	}

	int getInitialHeight(struct _Texture &texture)
	{
		if (texture.privateData == nullptr)
		{
			std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
			return -1;
		}
		return texture.privateData->initial_height;
	}

	int getInitialWidth(struct _Texture &texture)
	{
		if (texture.privateData == nullptr)
		{
			std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
			return -1;
		}
		return texture.privateData->initial_width;
	}

	void copyTexture(Texture &copy_from, Texture &copy_to)
	{
		if (copy_from.privateData == nullptr)
		{
			std::cerr << "TEXTURE: Texture has been deleted or not initialized" << std::endl;
			return;
		}
		loadTexture(copy_from.privateData->pathToFile, copy_to);
	}

	int loadTexture(std::string textureFile, struct _Texture &texture)
	{
		std::copy(_vertices, _vertices + 30, texture.vertices);
		texture.id = _png_load(textureFile.c_str(), &texture.width, &texture.height);
		if (texture.id == 0)
		{
			std::cerr << "TEXTURE: Failed to load texture " << textureFile << std::endl;
			return -1;
		}
		texture.privateData = writePrivateData(textureFile, true, texture.width, texture.height);
		return 0;
	}

	void toDefault(struct _Texture &texture)
	{
		std::copy(_vertices, _vertices + 30, texture.vertices);
		texture.width = getInitialWidth(texture);
		texture.height = getInitialHeight(texture);
	}

	void inheritTexture(Texture &inherit_from, Texture &to)
	{
		to.id = inherit_from.id;
		to.width = inherit_from.width;
		to.height = inherit_from.height;
		std::copy(inherit_from.vertices, inherit_from.vertices + 30, to.vertices);
		if (to.privateData != nullptr)
			delete to.privateData;
		to.privateData = writePrivateData(inherit_from.privateData->pathToFile, true, getInitialWidth(inherit_from), getInitialHeight(inherit_from), true);
	}

	Texture getPartFrom(Texture &texture, int x, int y, int width, int heigth)
	{
		Texture _texture;
		copyTexture(texture, _texture);
		setPart(_texture, x, y, width, heigth);
		return _texture;
	}

	Texture inheritPartFrom(Texture &texture, int x, int y, int width, int heigth)
	{
		Texture _texture;
		inheritTexture(texture, _texture);
		setPart(_texture, x, y, width, heigth);
		return _texture;
	}
}
