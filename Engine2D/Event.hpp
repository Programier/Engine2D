/*
 * Event.hpp
 *
 *  Created on: 26 мар. 2021 г.
 *      Author: programier
 */

#ifndef EVENT_HPP
#define EVENT_HPP

#include "Window.hpp"

namespace Event
{
    extern bool _keys[1024];
    extern unsigned int _frames[1024];
    extern unsigned int _current;

    int init();
    void pollEvents();
    void terminate();
    bool pressed(int keycode);
    bool jpressed(int keycode);
    bool isInited();
} // namespace Event

#endif
