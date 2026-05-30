#pragma once

#include "elements/Wheel.h"
#include "EditorState.h"
#include "persistence/AppSettings.h"
#include "persistence/FileBrowser.h"

struct AppContext
{
    Wheel wheel;
    EditorState editor;
    WheelSpinState spin;

    AppSettings appSettings;
    FileBrowserState fileBrowser;

    AppState state = AppState::MainView;

    bool menuOpen = false;
};