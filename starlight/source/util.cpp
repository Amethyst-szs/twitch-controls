#include "util.h"
#include "al/util.hpp"
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
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();
    amy::log("Demo%i", ri.isInvalidStage);
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

void amy::calcWorldTier(s32 worldID, const char* stageName)
{
    constexpr static const u8 worldTier[] = {
        1, // Cap kingdom
        1, // Cascase Kingdom
        0, // Sand Kingdom
        0, // Wooded Kingdom
        0, // Lake Kingdom
        1, // Cloud Kingdom
        1, // Lost Kingdom
        0, // Metro Kingdom
        0, // Seaside Kingdom
        0, // Snow Kingdom
        0, // Luncheon Kingdom
        3, // Ruined Kingdom
        1, // Bowsers Kingdom
        2, // Moon Kingdom
        0, // Mushroom Kingdom
        1, // Dark Side
        1 // Darker Side
    };

    amy::RedeemInfo::state& ri = amy::getRedeemInfo();
    ri.restrictionTier = worldTier[worldID];

    // Defines all stages that have custom restriction tiers
    constexpr static const char* stageNames[] = {
        "SnowWorldRace000Stage", // 0
        "SnowWorldRace001Stage", // 1
        "RevengeBossKnuckleStage", // 2
        "RevengeBossMagmaStage", // 3
        "RevengeBossRaidStage", // 4
        "RevengeForestBossStage", // 5
        "RevengeGiantWanderBossStage", // 6
        "RevengeMofumofuStage", // 7
        "MoonWorldKoopa1Stage", // 8
        "MoonWorldKoopa2Stage", // 9
        "MoonWorldBasementStage", // 10
        "HomeShipInsideStage", // 11
        "SnowWorldShopStage", // 12
        "SkyWorldShopStage", // 13
        "SandWorldShopStage", // 14
        "PeachWorldShopStage", // 15
        "MoonWorldShopRoom", // 16
        "LavaWorldShopStage", // 17
        "LakeWorldShopStage", // 18
        "CityWorldShop01Stage", // 19
        "ClashWorldShopStage", // 20
        "SandWorldSlotStage", // 21
        "CityWorldSandSlotStage" // 22
    };

    // And then definie what tiers these stages get
    constexpr static const u8 tiers[] = {
        5, // 0
        5, // 1
        1, // 2
        1, // 3
        1, // 4
        1, // 5
        1, // 6
        1, // 7
        3, // 8
        3, // 9
        3, // 10
        4, // 11
        4, // 12
        4, // 13
        4, // 14
        4, // 15
        4, // 16
        4, // 17
        4, // 18
        4, // 19
        4, // 20
        4, // 21
        4 // 22
    };

    // Basic variables
    int stageCount = *(&stageNames + 1) - stageNames;
    int matchIndex = -1;

    // Sets the matchIndex to the index of the stage, if the current stage isn't a restricted stage, it stays -1
    for (int i = 0; i < stageCount; i++) {
        if (al::isEqualString(stageName, stageNames[i])) {
            matchIndex = i;
            i = stageCount;
        }
    }

    // Finally, if a match was found, set the restriction tier to the match
    if (matchIndex != -1)
        ri.restrictionTier = tiers[matchIndex];

    return;
}

void amy::sendPacketStateNotice(bool rejectState, bool isTwitch)
{
    if (isTwitch) {
        amy::RedeemInfo::state& ri = amy::getRedeemInfo();
        if (ri.rejectionID > 9)
            amy::log("Reject/%u/%u", ri.rejectionID, rejectState);
        else
            amy::log("Reject/0%u/%u", ri.rejectionID, rejectState);
    }
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
        "ForestWorldHomeStage",
        "LakeWorldHomeStage",
        "CityWorldHomeStage",
        "SnowWorldHomeStage",
        "SeaWorldHomeStage",
        "LavaWorldHomeStage",
        "Special1WorldHomeStage",
        "Special2WorldHomeStage"
    };

    return stageNames[sead::GlobalRandom::instance()->getU32() % (sizeof(stageNames) / sizeof(const char*))];
}

void amy::updateRedeemStatus(bool isTwitch)
{
    StageScene* stageScene = amy::getGlobalStageScene();
    al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();

    if (isTwitch)
        amy::getRedeemInfo().isRedeemsValid = !(stageScene->isPause()
            || PlayerFunction::isPlayerDeadStatus(player)
            || rs::isActiveDemo(player)
            || amy::getDancePartyState().timer > 0);
    else
        amy::getRedeemInfo().isRedeemsValid = !(stageScene->isPause()
            || PlayerFunction::isPlayerDeadStatus(player));
    // amy::log("Info: %i %i %s", amy::getRedeemInfo().isRedeemsValid, amy::getRedeemInfo().isInvalidStage, GameDataFunction::getCurrentStageName(*amy::getGlobalStageScene()->mHolder));
    return;
}

void amy::drawBackground(agl::DrawContext* context, sead::Vector2f position, sead::Vector2f size, sead::Vector2f offset, sead::Color4f color)
{
    sead::Vector3f p1 = { position.x + offset.x, position.y + offset.y, 0.f }; // top left
    sead::Vector3f p2 = { position.x + size.x + offset.x, position.y + offset.y, 0.f }; // top right
    sead::Vector3f p3 = { position.x + offset.x, position.y + size.y + offset.y, 0.f }; // bottom left
    sead::Vector3f p4 = { position.x + size.x + offset.x, position.y + size.y + offset.y, 0.f }; // bottom right

    agl::utl::DevTools::beginDrawImm(context, sead::Matrix34<float>::ident, sead::Matrix44<float>::ident);
    agl::utl::DevTools::drawTriangleImm(context, p1, p2, p3, color);
    agl::utl::DevTools::drawTriangleImm(context, p3, p4, p2, color);
}

int amy::limitInt(int min, int value, int max)
{
    if (value > max)
        value = min;
    else if (value < min)
        value = max;
    return value;
}

float amy::limitFloat(float min, float value, float max)
{
    if (value > max)
        value = max;
    else if (value < min)
        value = min;
    return value;
}