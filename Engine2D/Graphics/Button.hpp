#ifndef BUTTON_HPP
#define BUTTON_HPP

#include "Texture.hpp"
extern "C"
{
    typedef struct Button
    {
        struct PrivateField *privateField;
        bool visible = true;
        int x = 0, y = 0, width = 0, height = 0;
        float angle = 0;
    } Button;

    void createButton(Button &button, Texture &button_texture, Texture &underCursor, Texture &pressed,
                      int x = 0, int y = 0, int width = 0, int height = 0, float angle = 0, bool visible = true);

    void drawButton(Button &button);
    void deleteButton(Button &button);
    bool ButtonClicked(Button &button);
}
#endif