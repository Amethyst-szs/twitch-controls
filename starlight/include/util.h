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
                    float timer = -1;
                    float addLength = 3; //Second of time added to timer when called
                    float comboDec = 0.25; //Amount of add time to remove on repeated calls
                    float comboLength; //Float to track current spam combo
            };
            class coinTickState{
                public:
                    float timer = -1;
                    float speed = 240.f;
            };
            class windState{
                public:
                    float timer;
                    float addLength = 10; //Second of time added to timer when called
                    float comboDec = 0.95; //Amount of add time to remove on repeated calls
                    float comboLength; //Float to track current spam combo
                    sead::Vector3f vect;
            };
            class hotFloorState{
                public:
                    float timer = -1;
                    float addLength = 10; //Second of time added to timer when called
                    float comboDec = 0.9; //Amount of add time to remove on repeated calls
                    float comboLength; //Float to track current spam combo
            };
            class stickInverState{
                public:
                    float timer = -1;
                    float addLength = 12.5; //Second of time added to timer when called
                    float comboDec = 1.15; //Amount of add time to remove on repeated calls
                    float comboLength; //Float to track current spam combo
            };
            class waterAreaState{
                public:
                    float timer = -1;
                    float addLength = 8; //Second of time added to timer when called
                    float comboDec = 0.75; //Amount of add time to remove on repeated calls
                    float comboLength; //Float to track current spam combo
            };
    };

    RedeemInfo::state& getRedeemInfo();
    RedeemInfo::gravityState& getGravityState();
    RedeemInfo::coinTickState& getCoinTickState();
    RedeemInfo::windState& getWindState();
    RedeemInfo::hotFloorState& getHotFloorState();
    RedeemInfo::stickInverState& getStickInverState();
    RedeemInfo::waterAreaState& getWaterAreaState();

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