#pragma once

#include "fl/packet.h"
#include "fl/server.h"
#include "game/StageScene/StageScene.h"
#include "sead/math/seadVector.h"
#include <cstdio>

namespace amy {
struct RedeemInfo {
    struct state {
        bool isInvalidStage = true;
        bool isRedeemsValid = false;
        bool isTransition = true;
        u8 rejectionID = 0;
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
        float addLength = 10; // Second of time added to timer when called
        float comboDec = 0.95; // Amount of add time to remove on repeated calls
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

void updateServerDemoState();

void sendPacketStateNotice(bool rejectState);

const char* getRandomHomeStage();
sead::Vector3f getRandomGravity();
void updateRedeemStatus();

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