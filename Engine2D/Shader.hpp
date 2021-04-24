/*
 * Shader.hpp
 *
 *  Created on: 27 мар. 2021 г.
 *      Author: programier
 */

#ifndef MYENGINE_SHADER_HPP_
#define MYENGINE_SHADER_HPP_

#include <string>

#include <glm/glm.hpp>
namespace Shader
{
    extern unsigned int id;
    int init();
    void deleteShader();
    void use();
    void useMatrix(std::string name, glm::mat4 matrix);
}

#endif /* MYENGINE_SHADER_HPP_ */
