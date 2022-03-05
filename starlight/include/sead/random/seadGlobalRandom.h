#pragma once


#include "sead/heap/seadDisposer.h"
#include "sead/random/seadRandom.h"

namespace sead
{
class GlobalRandom : public Random
{
    SEAD_SINGLETON_DISPOSER(GlobalRandom)
    GlobalRandom() = default;
};
}  // namespace sead
