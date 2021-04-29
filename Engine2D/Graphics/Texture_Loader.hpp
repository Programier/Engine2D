/*
 * Texture_Loader.hpp
 *
 *  Created on: 27 мар. 2021 г.
 *      Author: programier
 */

#ifndef MYENGINE_TEXTURE_LOADER_HPP_
#define MYENGINE_TEXTURE_LOADER_HPP_

#include <iostream>
extern "C"
{
    int _png_load(const char *file, int *width, int *height);
}
#endif /* TEXTURE_LOADER_HPP */
