#pragma once

#include "al/layout/LayoutActor.h"
#include "al/layout/LayoutInitInfo.h"
#include "al/util/NerveUtil.h"

namespace al {

class GaugeAir : public al::LayoutActor {
public:
    GaugeAir(char const*, char const*, al::LayoutInitInfo const&);

    void appear(void);
    bool tryEnd(void);
    bool tryEndForce(void);

    void exeAppear(void);
    void exeEnd(void);
};
} // namespace al

namespace {
NERVE_HEADER(GaugeAir, Appear)
NERVE_HEADER(GaugeAir, End)
} // namespace

// static_assert(sizeof(al::WindowConfirmWait) == 0x138, "Size of WindowConfirmWait");