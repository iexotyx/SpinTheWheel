#pragma once

#include "AppContext.h"

void updateEditWheel(
    sf::RenderWindow& window,
    AppContext& app,
    const ColourPickerLayout& picker,
    float dt)
{
    updateCursor(
        app.editor,
        app.state,
        dt
    );

    updateColourPickerDrag(
        window,
        app.state,
        app.editor,
        picker
    );
}