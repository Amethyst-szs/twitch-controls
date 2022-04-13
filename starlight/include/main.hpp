#pragma once

#include "al/LiveActor/LiveActor.h"
#include "al/gamepad/util.h"
#include "al/layout/LayoutActor.h"
#include "al/layout/LayoutKit.h"
// #include "al/layout/WindowConfirmWait.h"
#include "al/util.hpp"

#include "al/area/AreaObjGroup.h"
#include "al/camera/CameraDirector.h"
#include "al/camera/CameraPoser.h"
#include "al/camera/Projection.h"
#include "al/camera/alCameraPoserFunction.h"
#include "al/factory/ProjectActorFactory.h"
#include "al/util/AudioUtil.h"

#include "game/Actors/Shine.h"
#include "game/Controller/ControllerAppletFunction.h"
#include "game/GameData/GameDataFunction.h"
#include "game/GameData/GameDataHolderWriter.h"
#include "game/HakoniwaSequence/HakoniwaSequence.h"
#include "game/Layouts/CoinCounter.h"
#include "game/Player/PlayerAnimControlRun.h"
#include "game/Player/PlayerCameraTarget.h"
#include "game/Player/PlayerCostumeInfo.h"
#include "game/Player/PlayerFactory.h"
#include "game/Player/PlayerFunction.h"
#include "game/StageScene/StageScene.h"

#include "rs/util.hpp"

#include "sead/basis/seadNew.hpp"
#include "sead/gfx/seadCamera.h"
#include "sead/gfx/seadColor.h"
#include "sead/math/seadMatrix.h"
#include "sead/math/seadVector.h"
#include "sead/prim/seadSafeString.h"
#include "types.h"

#include "agl/DrawContext.h"
#include "agl/RenderBuffer.h"
#include "agl/utl.h"

#include "debugMenu.hpp"
#include "helpers.hpp"
#include "ips.hpp"
#include "redeemTimer.hpp"

#define RAD(deg) (deg * (M_PI / 180)) // converts Degrees to Radians
#define DEG(rad) (rad * (180 / M_PI)) // converts Radians to Degrees
#define BTOC(bool) (bool ? "True" : "False") // converts boolean to true/false char
#define HOOK_ATTR __attribute__((section(".text.hooks")))

#define TEMPPLAYERNAME "Amy"

struct DebugWarpPoint {
    char pointName[0x20] = {};
    sead::Vector3f warpPos = sead::Vector3f();
};