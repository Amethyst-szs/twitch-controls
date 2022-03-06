#include "util.h"
#include "main.hpp"
#include "sead/container/seadListImpl.h"
#include "sead/math/seadVector.h"
#include "sead/random/seadGlobalRandom.h"

StageScene*& amy::getGlobalStageScene(){
    static StageScene* stageScene;
    return stageScene;
}

amy::RedeemInfo& amy::getRedeemInfo(){
    static RedeemInfo redeemInfo;
    return redeemInfo;
}

sead::Vector3f amy::RedeemInfo::getRandomGravity(){
    sead::Vector3f VectorOptions[] = {
        {-1,0,0},
        {1,0,0},
        {0,0,-1},
        {0,0,1},
        {0,1,0},
    };
    return VectorOptions[sead::GlobalRandom::instance()->getU32()%(sizeof(VectorOptions)/sizeof(sead::Vector3f))];
}

const char* amy::getRandomHomeStage(){
    constexpr static const char* stageNames[] = {
        "CapWorldHomeStage",
        "WaterfallWorldHomeStage",
        "SandWorldHomeStage",
        "ForestWorldHomeStage",
        "LakeWorldHomeStage",
        "ClashWorldHomeStage",
        "CloudWorldHomeStage",
        "CityWorldHomeStage",
        "SnowWorldHomeStage",
        "SeaWorldHomeStage",
        "LavaWorldHomeStage",
        "BossRaidWorldHomeStage",
        "SkyWorldHomeStage",
        "MoonWorldHomeStage",
        "Special1WorldHomeStage",
        "Special2WorldHomeStage"
    };

    return stageNames[sead::GlobalRandom::instance()->getU32()%(sizeof(stageNames)/sizeof(const char *))];
}

void amy::updateRedeemStatus(){
    StageScene *stageScene = amy::getGlobalStageScene();
    al::PlayerHolder *pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);
    amy::getRedeemInfo().isInvalidStage = al::isEqualSubString(GameDataFunction::getCurrentStageName(*stageScene->mHolder), "Demo");
    amy::getRedeemInfo().isRedeemsValid = !(stageScene->isPause() || PlayerFunction::isPlayerDeadStatus(player) || rs::isActiveDemo(player) || amy::getRedeemInfo().isInvalidStage);
    amy::log("Info: %i %i %s", amy::getRedeemInfo().isRedeemsValid, amy::getRedeemInfo().isInvalidStage, GameDataFunction::getCurrentStageName(*amy::getGlobalStageScene()->mHolder));
    return;
}