/*
 * texture_renderer.cpp
 *
 *  Created on: 8 мар. 2021 г.
 *      Author: programier
 */
#include <GL/glew.h>
#include "Texture_Renderer.hpp"
#include "Shader.hpp"
#include <iostream>

static bool inited = false;
GLuint Texture_Renderer::VAO, Texture_Renderer::VBO;


int Texture_Renderer::init() {
	if (Shader::init() == -1) {
		std::cerr << "Failed to load shader" << std::endl;
		return -1;
	}

	glGenVertexArrays(1, &Texture_Renderer::VAO);
	glGenBuffers(1, &Texture_Renderer::VBO);

	glBindVertexArray(Texture_Renderer::VAO);
	glBindBuffer(GL_ARRAY_BUFFER, Texture_Renderer::VBO);

	// position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
			(GLvoid*) (0 * sizeof(GLfloat)));
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat),
			(GLvoid*) (3 * sizeof(GLfloat)));
	glEnableVertexAttribArray(1);

	glBindVertexArray(0);

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	std::cout << "Texture Renderer System successfully initialized" << std::endl;
	inited = true;
	return 0;
}

void Texture_Renderer::terminate() {
	std::cout << "Delete buffers and Vertex Array in Texture Renderer System" << std::endl;
	glDeleteBuffers(1, &Texture_Renderer::VBO);
	glDeleteVertexArrays(1, &Texture_Renderer::VAO);
	inited = false;
}

bool Texture_Renderer::isInited(){
	return inited;
}
