#pragma once

#include "al/audio/AudioDirector.h"
#include "al/camera/CameraDirector.h"
#include "al/effect/EffectSystemInfo.h"
#include "al/execute/ExecuteDirector.h"
#include "al/layout/LayoutKit.h"
#include "al/layout/LayoutSceneInfo.h"
#include "al/rumble/PadRumbleDirector.h"
#include "al/scene/SceneObjHolder.h"
#include "game/System/GameSystemInfo.h"

namespace al {

class LayoutInitInfo : public LayoutSceneInfo {
public:
    void init(al::ExecuteDirector*, al::EffectSystemInfo const*, al::SceneObjHolder*,
        al::AudioDirector const*, al::CameraDirector*, al::LayoutSystem const*,
        al::MessageSystem const*, al::GamePadSystem const*, al::PadRumbleDirector*);

    void* qword30;
    void* qword38;
    void* qword40;
    al::ExecuteDirector* mExecuteDirector;
    al::EffectSystemInfo* mEffectSysInfo;
    al::AudioDirector* mAudioDirector;
    al::LayoutSystem* mLayoutSystem;
};

void initLayoutInitInfo(al::LayoutInitInfo*, al::LayoutKit const*, al::SceneObjHolder*, al::AudioDirector const*, al::LayoutSystem const*, al::MessageSystem const*, al::GamePadSystem const*);
} // namespace al
