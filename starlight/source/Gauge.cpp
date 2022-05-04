#include "layouts/Gauge.h"
#include "al/layout/LayoutActor.h"
#include "al/util.hpp"
#include "al/util/LayoutUtil.h"
#include "al/util/LiveActorUtil.h"
#include "al/util/MathUtil.h"
#include "al/util/NerveUtil.h"

Gauge::Gauge(const char* name, const al::LayoutInitInfo& initInfo)
    : al::LayoutActor(name)
    , mText(u"bussing")
    , mPerc(1.0f)
{
    al::initLayoutActor(this, initInfo, "GaugeAir", nullptr);
    // al::initLayoutActor(this, initInfo, "TwTimer", nullptr);
    // al::setPaneString(this, "TxtMessage", u"Deez Nuts", 0);
    initNerve(&nrvGaugeAppear, 0);
    appear();
}

void Gauge::updateGauge()
{
    if (mPerc < 1.0) {
        al::startFreezeAction(this, "Decrease", al::getActionFrameMax(this, "Decrease", "Gauge") * (1.0f - mPerc), "Gauge");
    }
}

void Gauge::exeAppear()
{
    if (al::isFirstStep(this)) {
        al::startAction(this, "Wait", nullptr);
        mPerc = 1.0;
    }

    updateGauge();

    if (al::isActionEnd(this, nullptr)) {
        al::setNerve(this, &nrvGaugeWait);
    }
}

void Gauge::exeWait()
{
    updateGauge();
    mPerc -= 0.01;

    if (al::isNearZeroOrLess(mPerc, 0.001)) {
        al::setNerve(this, &nrvGaugeAppear);
    }
}

void Gauge::exeEnd() { }

namespace {
NERVE_IMPL(Gauge, Appear)
NERVE_IMPL(Gauge, Wait)
NERVE_IMPL(Gauge, End)
} // namespace