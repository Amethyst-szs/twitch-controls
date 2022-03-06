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
            bool isInvalidStage;
            bool isRedeemsValid;
    };
    RedeemInfo& getRedeemInfo();
    
    const char *getRandomHomeStage();
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

}