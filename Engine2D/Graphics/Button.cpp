#include "Button.hpp"
#include "Graphics.hpp"
#include "GLFW/glfw3.h"

extern "C"
{
    using namespace Graphics;

    typedef struct PrivateField
    {
        Texture button[3];
        bool pressed, released;
    } PrivateField;

    void createButton(Button &button, Texture &button_texture, Texture &underCursor, Texture &pressed, int x, int y, int width, int height, float angle, bool visible)
    {
        button.privateField = new PrivateField();
        inheritTexture(button_texture, button.privateField->button[0]);
        inheritTexture(underCursor, button.privateField->button[1]);
        inheritTexture(pressed, button.privateField->button[2]);
        button.visible = visible;
        button.x = x;
        button.y = y;
        button.width = width;
        button.height = height;
        button.angle = angle;
        button.privateField->pressed = false;
        button.privateField->released = false;
    }

    void deleteButton(Button &button)
    {
        delete button.privateField;
    }

    void drawButton(Button &button)
    {
        if (button.visible)
        {

            int frame = 0;
            int mouse_x = _Event::getX(), mouse_y = _Event::getY();
            int width = (button.width == 0) ? button.privateField->button[frame].width : button.width;
            int height = (button.height == 0) ? button.privateField->button[frame].height : button.height;

            if (mouse_x >= button.x && mouse_x <= button.x + width && mouse_y >= button.y && mouse_y <= button.y + height)
            {
                frame++;
                if (_Event::pressed(GLFW_MOUSE_BUTTON_LEFT))
                    frame++;
            }

            draw(button.privateField->button[frame], button.x, button.y, button.angle, width, height);
        }
    }

    bool ButtonClicked(Button &button)
    {
        if (button.visible)
        {
            int mouse_x = _Event::getX(), mouse_y = _Event::getY();
            int width = (button.width == 0) ? button.privateField->button[0].width : button.width;
            int height = (button.height == 0) ? button.privateField->button[0].height : button.height;

            if (mouse_x >= button.x && mouse_x <= button.x + width && mouse_y >= button.y && mouse_y <= button.y + height)
            {
                if (_Event::pressed(GLFW_MOUSE_BUTTON_LEFT))
                {
                    button.privateField->pressed = true;
                }
                if (button.privateField->pressed && !_Event::pressed(GLFW_MOUSE_BUTTON_LEFT))
                {
                    button.privateField->released = false;
                    button.privateField->pressed = false;
                    return true;
                }
            }
            else
            {
                button.privateField->pressed = false;
                button.privateField->released = false;
            }
        }
        return false;
    }
}