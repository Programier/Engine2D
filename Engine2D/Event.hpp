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
    int init();
    void pollEvents();
    void terminate();
    namespace Keyboard
    {
        bool pressed(int keycode);
        bool jpressed(int keycode);
    } // namespace Keyboard

    namespace Mouse
    {
        bool pressed(int keycode);
        bool jpressed(int keycode);
        int getX();
        int getY();
    } // namespace Mouse

    bool isInited();

} // namespace Event

#endif
