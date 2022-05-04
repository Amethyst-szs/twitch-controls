#pragma once

#include "al/async/AsyncFunctorThread.h"
#include "al/async/FunctorV0M.hpp"
#include "al/layout/WindowConfirmWait.h"
#include "layouts/Gauge.h"
#include "main.hpp"
#include <stdlib.h>

namespace smo {
struct Layouts {
    HakoniwaSequence* mainSeq;

    // -- Game Layouts --
    // Disconnection screen
    int pingFrames = 0;
    int pingPreviousFrame;
    int pingFrameStuckFrames = 0;
    bool firstBoot = false;
    al::WindowConfirmWait* mConnectionWait;

    // Gauges
    Gauge* mGauge;
};
smo::Layouts& getLayouts();
const char16_t* getFunFact();

// Functions
void layoutInit(al::LayoutInitInfo const& initInfo);
};