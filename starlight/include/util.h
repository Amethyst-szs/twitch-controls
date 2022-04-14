#pragma once

#include "fl/packet.h"
#include "fl/server.h"
#include "game/StageScene/StageScene.h"
#include "sead/gfx/seadColor.h"
#include "sead/math/seadVector.h"
#include "sead/prim/seadSafeString.h"
#include <cstdio>

namespace amy {
struct RedeemInfo {
    struct state {
        // State
        bool isInvalidStage = true;
        bool isRedeemsValid = false;
        bool isTransition = true;
        bool isMusic = false;

        // Frame timer based states
        int isSaveLoad = 0;
        int isLangChange = 0;
        int isSceneKill = 0;
        int isRecoverBubble = 0;

        // Kingdom flee
        int fleeDelay = 300;
        int fleeFrames = -1;

        // Say message
        int sayTimer = -1;
        int sayLines = 1;
        int sayAnimator = 0;
        const char* sayText;

        // Restriction and rejection values
        u8 rejectionID = 0;
        u8 rejectionIDMax = 50;
        u8 restrictionTier = 0;
        u8 lastPublishRestrictionTier = 0;
    };
    struct gravityState {
        float timer = -1;
        float addLength = 3; // Second of time added to timer when called
        float comboDec = 0.25; // Amount of add time to remove on repeated calls
        float comboLength; // Float to track current spam combo
    };
    struct coinTickState {
        float timer = -1;
        float speed = 240.f;
    };
    struct windState {
        float timer;
        float strength = 6.5;
        float addLength = 5; // Second of time added to timer when called
        float comboDec = 0.49; // Amount of add time to remove on repeated calls
        float comboLength; // Float to track current spam combo
        sead::Vector3f vect;
    };
    struct hotFloorState {
        float timer = -1;
        float addLength = 10; // Second of time added to timer when called
        float comboDec = 0.9; // Amount of add time to remove on repeated calls
        float comboLength; // Float to track current spam combo
    };
    struct stickInverState {
        float timer = -1;
        float addLength = 12.5; // Second of time added to timer when called
        float comboDec = 1.15; // Amount of add time to remove on repeated calls
        float comboLength; // Float to track current spam combo
    };
    struct waterAreaState {
        float timer = -1;
        float addLength = 8; // Second of time added to timer when called
        float comboDec = 0.75; // Amount of add time to remove on repeated calls
        float comboLength; // Float to track current spam combo
    };
    struct dancePartyState {
        float timer = -1;
        float addLength = 20; // Second of time added to timer when called
        sead::SafeString selectedAnim = "AreaWaitDance01";
        sead::Vector2f overlayPos = { 0.f, 0.f };
        sead::Vector2f overlayDirecton = { 1.f, 1.f };
        sead::Vector2f overlayMaxPos = { 0.f, 0.f };
        float overlaySpeed = 15.f;
        bool enableFrame = false;
    };
    struct shineWarpState {
        bool isWarp = false;
        int targetShineID = -1;
    };
};

RedeemInfo::state& getRedeemInfo();
RedeemInfo::gravityState& getGravityState();
RedeemInfo::coinTickState& getCoinTickState();
RedeemInfo::windState& getWindState();
RedeemInfo::hotFloorState& getHotFloorState();
RedeemInfo::stickInverState& getStickInverState();
RedeemInfo::waterAreaState& getWaterAreaState();
RedeemInfo::dancePartyState& getDancePartyState();
RedeemInfo::shineWarpState& getShineWarpState();

// Restriction functions
void setRestrictionTier(u8 tier);
void calcWorldTier(s32 worldID, const char* stageName);

void updateServerDemoState();
void sendPacketStateNotice(bool rejectState, bool isTwitch);

const char* getRandomHomeStage(const char* curStage);
sead::Vector3f getRandomGravity();
void updateRedeemStatus(bool isTwitch);

void triggerKingdomFlee(StageScene* stageScene, PlayerActorHakoniwa* player);

void drawBackground(agl::DrawContext* context, sead::Vector2f position, sead::Vector2f size, sead::Vector2f offset, sead::Color4f color);
int limitInt(int min, int value, int max);
float limitFloat(float min, float value, float max);

StageScene*& getGlobalStageScene();
template <typename... Args>
void log(const char* format, Args... args)
{
    int size = snprintf(nullptr, 0, format, args...);
    char buf[size + 1];
    sprintf(buf, format, args...);
    smo::OutPacketLog p;
    p.type = smo::OutPacketLog::LogType::Log;
    p.message = buf;
    smo::Server::instance().sendPacket(p, smo::OutPacketType::Log);
}

// void demoToggle(bool state);
}