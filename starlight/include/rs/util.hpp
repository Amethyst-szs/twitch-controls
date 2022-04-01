#pragma once

#include "al/area/ChangeStageInfo.h"
#include "al/sensor/SensorMsg.h"
#include "al/util.hpp"
#include "game/Actors/Shine.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "sead/math/seadVector.h"

namespace rs {
bool isMainShine(Shine const*);

uint32_t getStageShineAnimFrame(const al::LiveActor*, const char*);

ChangeStageInfo* createChangeStageInfo(al::LiveActor const* actor, char const* changeStageID, char const* changeStageName, bool, int scenarioNo, ChangeStageInfo::SubScenarioType type);
ChangeStageInfo* createChangeStageInfo(al::LiveActor const*, al::PlacementInfo const&, char const*, char const*, bool, int, ChangeStageInfo::SubScenarioType);
ChangeStageInfo* createChangeStageInfo(const al::LiveActor*, const al::PlacementInfo*);

bool isPlayerDamageStopDemo(const al::LiveActor*);

bool isDemoEnvironmentChange(al::Scene const*);

al::LiveActor* getPlayerActor(const al::Scene*);

void get2DAreaPos(sead::Vector3<f32>*, al::AreaObj const*);

bool isInChangeStageArea(PlayerActorHakoniwa const*, sead::Vector3f const*);

bool isPlayerOnGround(const al::LiveActor*);

void faceToCamera(al::LiveActor* actor);

bool isPlayerActiveMarioAmiiboInvincible(al::LiveActor*);

bool isMsgPlayerAndCapObjHipDropAll(al::SensorMsg const*);

bool isMsgPlayerDamage(al::SensorMsg const*);

bool isMsgShineGet(al::SensorMsg const*);

void saveCoinStack(al::LiveActor const* actor, al::PlacementId const* placement, int stackCount);

bool isActiveDemo(al::Scene const*);

bool isActiveDemoWithPlayer(al::Scene const*);

bool isActiveDemoWithPlayerKeepCarry(al::Scene const*);

bool isActiveDemoWithPlayerUseCoin(al::Scene const*);

bool isActiveDemoShineGet(al::Scene const*);

bool isActiveDemoWarp(al::Scene const*);

bool isActiveDemoScenarioCamera(al::Scene const*);

bool isActiveDemoTalk(al::Scene const*);

void recoveryPlayerOxygen(const al::LiveActor*);

bool is2D(IUseDimension const*);

void calcGroundNormalOrGravityDir(sead::Vector3f* result, al::LiveActor const* actor, IUsePlayerCollision const* col);

void calcPlayerFrontDir(sead::Vector3f* result, al::LiveActor const*);

void showHackCap(IUsePlayerHack*);
void hideHackCap(IUsePlayerHack*);
void forcePutOffMarioHeadCap(al::LiveActor*);
bool isPlayer2D(const al::LiveActor*);
bool isActiveDemo(al::LiveActor const*);
bool isDemoEnvironmentChange(al::Scene const*);
}
