#pragma once

#include "al/async/AsyncFunctorThread.h"
#include "al/async/FunctorV0M.hpp"
// #include "al/layout/GaugeAir.h"
#include "al/layout/WindowConfirmWait.h"
#include "main.hpp"
#include <stdlib.h>

namespace smo {
struct Layouts {
    // -- Game Layouts --
    int pingFrames = 0;
    int pingPreviousFrame;
    int pingFrameStuckFrames = 0;
    bool firstBoot = false;
    al::WindowConfirmWait* mConnectionWait;
    // al::GaugeAir* mGague;
};
smo::Layouts& getLayouts();
const char16_t* getFunFact();

// Functions
void layoutInit(al::LayoutInitInfo const& initInfo);
};