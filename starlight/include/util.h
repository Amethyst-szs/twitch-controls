#pragma once

#include "fl/packet.h"
#include "fl/server.h"
#include "game/StageScene/StageScene.h"
#include "sead/math/seadVector.h"
#include <cstdio>

namespace amy
{
    class RedeemInfo{
        public:
            class state{
                public:
                    bool isInvalidStage = true;
                    bool isRedeemsValid = false;
                    bool isTransition = true;
            };
            class gravityState{
                public:
                    int gravityTimer;
            };
            class coinTickState{
                public:
                    int coinTickCurrent = 0;
                    float coinTickRate = 240.f;
                    bool coinTickRunning = false;
            };
            class windState{
                public:
                    sead::Vector3f windVect;
                    int windTimer;
            };
            class hotFloorState{
                public:
                    bool isHotFloor = false;
                    int hotFloorTimer;
            };
            class stickInverState{
                public:
                    bool isStickInver = false;
                    int stickInverTimer;
            };
    };

    RedeemInfo::state& getRedeemInfo();
    RedeemInfo::gravityState& getGravityState();
    RedeemInfo::coinTickState& getCoinTickState();
    RedeemInfo::windState& getWindState();
    RedeemInfo::hotFloorState& getHotFloorState();
    RedeemInfo::stickInverState& getStickInverState();

    void updateServerDemoState();
    
    const char *getRandomHomeStage();
    sead::Vector3f getRandomGravity();
    void updateRedeemStatus();

    StageScene*& getGlobalStageScene();
    template <typename ...Args>
    void log(const char* format, Args... args) {
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