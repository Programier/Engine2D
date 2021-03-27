/*
 * Shader.hpp
 *
 *  Created on: 27 мар. 2021 г.
 *      Author: programier
 */

#ifndef MYENGINE_SHADER_HPP_
#define MYENGINE_SHADER_HPP_

#include <string>

namespace Shader{
    extern unsigned int id;
    int init();
    void deleteShader();
    void use();
}

#endif /* MYENGINE_SHADER_HPP_ */
