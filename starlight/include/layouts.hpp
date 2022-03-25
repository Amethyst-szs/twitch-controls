#pragma once

#include "al/layout/WindowConfirmWait.h"
#include "main.hpp"
#include <stdlib.h>

namespace smo {
struct Layouts {
    // -- Game Layouts --
    bool isConnectionWaitOverlay = false;
    al::WindowConfirmWait* mConnectionWait;
};
smo::Layouts& getLayouts();

// Functions
void layoutInit(al::LayoutInitInfo const& initInfo);
};