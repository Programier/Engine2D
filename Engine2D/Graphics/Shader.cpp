/*
 * shader.cpp
 *
 *  Created on: 27 мар. 2021 г.
 *      Author: programier
 */

#include "Shader.hpp"

#include <exception>
#include <fstream>
#include <iostream>
#include <sstream>

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>
extern "C"
{
	unsigned int Shader::id;

	void Shader::deleteShader()
	{
		glDeleteProgram(id);
	}

	void Shader::use()
	{
		glUseProgram(id);
	}

	int Shader::initShader()
	{
		const GLchar *vShaderCode = "					\n\
#version 330 core									\n\
													\n\
layout (location = 0) in vec3 v_position;			\n\
layout (location = 1) in vec2 v_texCoord;			\n\
													\n\
out vec4 a_color;									\n\
out vec2 a_texCoord;								\n\
													\n\
uniform mat4 matrix;								\n\
													\n\
void main(){										\n\
	a_color = vec4(1.0f,1.0f,1.0f,1.0f);			\n\
	a_texCoord = v_texCoord;						\n\
	gl_Position = matrix * vec4(v_position, 1.0);	\n\
} 													\n\
";

		const GLchar *fShaderCode = "							\n\
#version 330 core											\n\
															\n\
in vec4 a_color;											\n\
in vec2 a_texCoord;											\n\
out vec4 f_color;											\n\
															\n\
uniform sampler2D u_texture0;								\n\
															\n\
void main(){												\n\
	f_color = a_color * texture(u_texture0, a_texCoord);	\n\
}";

		GLuint vertex, fragment;
		GLint success;
		GLchar infoLog[512];
		// Vertex Shader
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, nullptr);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(vertex, 512, nullptr, infoLog);
			std::cerr << "SHADER::VERTEX: compilation failed" << std::endl;
			std::cerr << infoLog << std::endl;
			return -1;
		}
		// Fragment Shader
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, nullptr);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(fragment, 512, nullptr, infoLog);
			std::cerr << "SHADER::FRAGMENT: compilation failed" << std::endl;
			std::cerr << infoLog << std::endl;
			return -1;
		}

		// Shader Program
		GLuint _id = glCreateProgram();
		glAttachShader(_id, vertex);
		glAttachShader(_id, fragment);
		glLinkProgram(_id);
		glGetProgramiv(_id, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(id, 512, nullptr, infoLog);
			std::cerr << "SHADER::PROGRAM: linking failed" << std::endl;
			std::cerr << infoLog << std::endl;

			glDeleteShader(vertex);
			glDeleteShader(fragment);
			return -1;
		}
		glDeleteShader(vertex);
		glDeleteShader(fragment);

		Shader::id = _id;
		return 0;
	}

	void Shader::useMatrix(std::string name, glm::mat4 matrix)
	{
		GLuint transformLoc = glGetUniformLocation(Shader::id, name.c_str());
		glUniformMatrix4fv(transformLoc, 1, GL_FALSE, glm::value_ptr(matrix));
	}
}