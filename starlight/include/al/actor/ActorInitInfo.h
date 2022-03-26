#pragma once

#include "al/LiveActor/LiveActorGroup.h"
#include "al/actor/ActorSceneInfo.h"
#include "al/actor/Placement.h"
#include "al/audio/AudioDirector.h"
#include "al/effect/EffectSystemInfo.h"
#include "al/execute/ExecuteDirector.h"


namespace al {

class ModelDrawBufferCounter;
class ActorResourceHolder;
class HitSensorDirector;
class ScreenPointDirector;
class StageSwitchDirector;
class ViewIdHolder;
class ActorFactory;

class ActorInitInfo {
public:
    ActorInitInfo();
    void initViewIdSelf(al::PlacementInfo const*, al::ActorInitInfo const&);
    // void initNew(al::PlacementInfo const*, al::LayoutInitInfo const*, al::LiveActorGroup *, al::ActorFactory const*, al::ActorResourceHolder *, al::AreaObjDirector *, al::AudioDirector *, al::CameraDirector *, al::ClippingDirector *, al::CollisionDirector *, al::DemoDirector *, al::EffectSystemInfo *, al::ExecuteDirector *, al::GameDataHolderBase *, al::GravityHolder *, al::HitSensorDirector *, al::ItemDirectorBase *, al::NatureDirector *, al::GamePadSystem const*, al::PadRumbleDirector *, al::PlayerHolder *, al::SceneObjHolder *, al::SceneMsgCtrl *, al::SceneStopCtrl *, al::ScreenCoverCtrl *, al::ScreenPointDirector *, al::ShadowDirector *, al::StageSwitchDirector *, al::ModelGroup *, al::GraphicsSystemInfo *, al::ModelDrawBufferCounter *, al::LiveActorGroup *);
    LiveActorGroup* mLiveActorGroup;
    const al::PlacementInfo& mPlacementInfo; // 0x8
    ActorSceneInfo mActorSceneInfo; // 0x18
    ModelDrawBufferCounter* mDrawBuffCount; // 0xB0
    LiveActorGroup* mLiveActorGroup2; // 0xB8
    ActorFactory* mActorFactory; // 0xC0
    ActorResourceHolder* mResourceHolder; // 0xC8
    AudioDirector* mAudioDirector; // 0xD0
    EffectSystemInfo* mEffectSysInfo; // 0xD8
    ExecuteDirector* mExecuteDirector; // 0xE0
    HitSensorDirector* mHitSensorDirector; // 0xE8
    ScreenPointDirector* mScreenPointDirector; // 0xF0
    StageSwitchDirector* mStageSwitchDirector; // 0xF8
    ViewIdHolder* mViewIdHolder; // 0x100
    void* unkPtr9; // 0x108
};
};

// size not entirely known, guessing based off of ActorInitInfo::initNew
// static_assert(sizeof(al::ActorInitInfo) == 0x110);