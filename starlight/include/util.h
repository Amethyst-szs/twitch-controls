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
            int gravityTimer;
            int invisTimer;
            sead::Vector3f getRandomGravity();
            bool isInvalidStage = true;
            bool isRedeemsValid = false;
            bool isTransition = true;
    };
    RedeemInfo& getRedeemInfo();
    
    const char *getRandomHomeStage();
    void updateRedeemStatus();

    StageScene*& getGlobalStageScene();
    template <typename ...Args>
    void log(const int type, const char* format, Args... args) {
        int size = snprintf(nullptr, type, format, args...);
        char buf[size + 1];
        sprintf(buf, format, args...);
        smo::OutPacketLog p;
        switch(type){
            case 0:
                p.type = smo::OutPacketLog::LogType::Log;
                break;
            case 1:
                p.type = smo::OutPacketLog::LogType::DemoToggle;
                break;
        }
        // p.type = smo::OutPacketLog::LogType::Log;
        
        p.message = buf;
        smo::Server::instance().sendPacket(p, smo::OutPacketType::Log);
    }
}