#include "fl/packet.h"
#include "al/util.hpp"
#include "game/StageScene/StageScene.h"
#include "rs/util.hpp"
#include "util.h"
#include <string.h>

namespace smo
{
    u32 OutPacketLog::calcLen()
    {
        return strlen(message) + 1;
    }

    void OutPacketLog::construct(u8* dst)
    {
        *dst = type;
        strcpy((char*) dst + 1, message);
    }

    void InPacketEvent::parse(const u8 *data, u32 len){
        eventID = data[0];
    }

    void InPacketEvent::on(Server &server){
        amy::log("Event Redeem Claimed! EventID: %i - Rejected: %s", eventID, !amy::getRedeemInfo().isRedeemsValid ? "true" : "false");
        if(amy::getRedeemInfo().isRedeemsValid){
            switch(eventID){
                case 1: //PrevScene - See Myself Out
                    amy::getGlobalStageScene()->mHolder->returnPrevStage();
                    break;
                case 2: //GravFlip - Change Gravity
                    al::setGravity(rs::getPlayerActor(amy::getGlobalStageScene()), amy::getRedeemInfo().getRandomGravity());
                    amy::getRedeemInfo().gravityTimer += 3*60;
                    break;
                default:
                    amy::log("Invalid EventID sent? EventID: %i", eventID);
                    break;
            }
        }
    }

    void InPacketResize::parse(const u8 *data, u32 len){
        scaleVector = *(sead::Vector3f*) &data[0];
    }

    void InPacketResize::on(Server &server){
        amy::log("Resize Redeem Claimed! Rejected: %s", !amy::getRedeemInfo().isRedeemsValid ? "true" : "false");
        if(amy::getRedeemInfo().isRedeemsValid){
            al::PlayerHolder *pHolder = al::getScenePlayerHolder(amy::getGlobalStageScene());
            PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);

            al::setScale(player, scaleVector);
        }
    }

    void InPacketPosRandomize::parse(const u8 *data, u32 len){
        posVector = *(sead::Vector3f*) &data[0];
    }

    void InPacketPosRandomize::on(Server &server){
        amy::log("PosRandomize Redeem Claimed! Rejected: %s", !amy::getRedeemInfo().isRedeemsValid ? "true" : "false");
        if(amy::getRedeemInfo().isRedeemsValid){
            al::PlayerHolder *pHolder = al::getScenePlayerHolder(amy::getGlobalStageScene());
            PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);

            player->startDemoPuppetable(); //Not working, figure out why
            sead::Vector3f *trans = al::getTrans(player);
            trans->x += posVector.x;
            trans->y += posVector.y;
            trans->z += posVector.z;
            player->endDemoPuppetable();
        }
    }
}