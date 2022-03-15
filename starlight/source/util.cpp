#include "util.h"
#include "main.hpp"
#include "sead/container/seadListImpl.h"
#include "sead/math/seadVector.h"
#include "sead/random/seadGlobalRandom.h"

// void demoToggle(bool state) {
//     smo::OutPacketDemoToggle p;
//     p.toggleState = state;
//     smo::Server::instance().sendPacket(p, smo::OutPacketType::DemoToggle);
// }

void amy::updateServerDemoState()
{
    amy::log("Demo%i", amy::getRedeemInfo().isInvalidStage);
}

StageScene*& amy::getGlobalStageScene()
{
    static StageScene* stageScene;
    return stageScene;
}

// Redeem info handlers
amy::RedeemInfo::state& amy::getRedeemInfo()
{
    static RedeemInfo::state i;
    return i;
}
amy::RedeemInfo::gravityState& amy::getGravityState()
{
    static RedeemInfo::gravityState i;
    return i;
}
amy::RedeemInfo::coinTickState& amy::getCoinTickState()
{
    static RedeemInfo::coinTickState i;
    return i;
}
amy::RedeemInfo::windState& amy::getWindState()
{
    static RedeemInfo::windState i;
    return i;
}
amy::RedeemInfo::hotFloorState& amy::getHotFloorState()
{
    static RedeemInfo::hotFloorState i;
    return i;
}
amy::RedeemInfo::stickInverState& amy::getStickInverState()
{
    static RedeemInfo::stickInverState i;
    return i;
}
amy::RedeemInfo::waterAreaState& amy::getWaterAreaState()
{
    static RedeemInfo::waterAreaState i;
    return i;
}

amy::RedeemInfo::dancePartyState& amy::getDancePartyState()
{
    static RedeemInfo::dancePartyState i;
    return i;
}

amy::RedeemInfo::shineWarpState& amy::getShineWarpState()
{
    static RedeemInfo::shineWarpState i;
    return i;
}

void amy::sendPacketStateNotice(bool rejectState)
{
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();
    amy::log("Reject/%u/%u", ri.rejectionID, rejectState);
}

sead::Vector3f amy::getRandomGravity()
{
    sead::Vector3f VectorOptions[] = {
        { -1, 0, 0 },
        { 1, 0, 0 },
        { 0, 0, -1 },
        { 0, 0, 1 },
        { 0, 1, 0 },
    };
    return VectorOptions[sead::GlobalRandom::instance()->getU32() % (sizeof(VectorOptions) / sizeof(sead::Vector3f))];
}

const char* amy::getRandomHomeStage()
{
    constexpr static const char* stageNames[] = {
        "PeachWorldHomeStage",
        "CapWorldHomeStage",
        "WaterfallWorldHomeStage",
        "SandWorldHomeStage",
        "ForestWorldHomeStage",
        "LakeWorldHomeStage",
        "CityWorldHomeStage",
        "SnowWorldHomeStage",
        "SeaWorldHomeStage",
        "LavaWorldHomeStage",
        "SkyWorldHomeStage",
        "Special1WorldHomeStage",
        "Special2WorldHomeStage"
    };

    return stageNames[sead::GlobalRandom::instance()->getU32() % (sizeof(stageNames) / sizeof(const char*))];
}

void amy::updateRedeemStatus()
{
    StageScene* stageScene = amy::getGlobalStageScene();
    al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);
    amy::getRedeemInfo().isRedeemsValid = !(stageScene->isPause() || PlayerFunction::isPlayerDeadStatus(player) || rs::isActiveDemo(player) || amy::getDancePartyState().timer > 0);
    // amy::log("Info: %i %i %s", amy::getRedeemInfo().isRedeemsValid, amy::getRedeemInfo().isInvalidStage, GameDataFunction::getCurrentStageName(*amy::getGlobalStageScene()->mHolder));
    return;
}