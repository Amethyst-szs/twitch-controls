#include "al/layout/LayoutActor.h"
#include "al/layout/LayoutInitInfo.h"
#include "al/util/NerveUtil.h"

class Gauge : public al::LayoutActor {
public:
    Gauge(const char* name, const al::LayoutInitInfo& initInfo);

    // Use 0.0f to 1.0f range
    void setPerc(float perc)
    {
        mPerc = perc;
    }
    void updateGauge();
    void exeAppear();
    void exeWait();
    void exeEnd();

private:
    bool decreasing;
    float mPerc;
    const char16_t* mText;
};

namespace {
NERVE_HEADER(Gauge, Appear)
NERVE_HEADER(Gauge, Wait)
NERVE_HEADER(Gauge, End)
}