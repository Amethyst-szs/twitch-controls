#pragma once

#include "sead/math/seadVector.h"
class PlayerInput {
    public:
        bool isMove(void) const;
        bool isMoveDeepDown(void) const;
        bool isMoveDeepDownNoSnap(void) const;
        bool isNoInput(void) const;
        bool isTriggerJump(void) const;
        sead::Vector2f getStickMoveRaw(void) const;
};