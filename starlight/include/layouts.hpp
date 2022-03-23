#pragma once

#include "main.hpp"
#include <stdlib.h>

namespace smo {
class Layouts {
public:
    void init(al::LayoutInitInfo const& initInfo);

    // -- Game Layouts --
    bool isConnectionWaitOverlay = false;
    // al::WindowConfirmWait* mConnectionWait;
};

smo::Layouts& getLayouts();
};