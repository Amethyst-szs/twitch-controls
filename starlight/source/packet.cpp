#include "fl/packet.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util.hpp"
#include "game/StageScene/StageScene.h"
#include "game/GameData/GameDataFunction.h"
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
        amy::updateRedeemStatus();
        amy::log("Event Redeem Claimed! EventID: %i - Rejected: %s", eventID, !amy::getRedeemInfo().isRedeemsValid ? "true" : "false");
        if(amy::getRedeemInfo().isRedeemsValid){
            switch(eventID){
                case 1:{ //PrevScene - See Myself Out
                    amy::getGlobalStageScene()->mHolder->returnPrevStage();
                    break;
                }
                case 2:{ //GravFlip - Change Gravity
                    al::setGravity(rs::getPlayerActor(amy::getGlobalStageScene()), amy::getRedeemInfo().getRandomGravity());
                    amy::getRedeemInfo().gravityTimer += 3*60;
                    break;
                }
                case 3:{ //Fling - Up we go
                    al::LiveActor* player = rs::getPlayerActor(amy::getGlobalStageScene());
                    al::setTransY(player, al::getTrans(player)->y+250.f);
                    al::setVelocityZero(player);
                    al::addVelocity(player, {0.f,80.f,0.f});
                    break;
                }
                case 4:{
                    al::PlayerHolder *pHolder = al::getScenePlayerHolder(amy::getGlobalStageScene());
                    PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);
                    GameDataFunction::disableCapByPlacement(player);
                    break;
                }
                case 5:{
                    if(!amy::getGlobalStageScene()->mHolder->mGameDataFile->isUnlockedWorld(2)){
                        amy::log("Rejected random kingdom because Sand Kingdom isn't unlocked yet!");
                        break;
                    }

                    ChangeStageInfo stageInfo(amy::getGlobalStageScene()->mHolder, "start", amy::getRandomHomeStage(), false, -1, ChangeStageInfo::SubScenarioType::UNK);
                    amy::getGlobalStageScene()->mHolder->changeNextStage(&stageInfo, 0);
                    break;
                }
                default:{
                    amy::log("Invalid EventID sent? EventID: %i", eventID);
                    break;
                }
            }
        }
    }

    void InPacketResize::parse(const u8 *data, u32 len){
        scaleVector = *(sead::Vector3f*) &data[0];
    }

    void InPacketResize::on(Server &server){
        amy::updateRedeemStatus();
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
        amy::updateRedeemStatus();
        amy::log("PosRandomize Redeem Claimed! Rejected: %s", !amy::getRedeemInfo().isRedeemsValid ? "true" : "false");
        if(amy::getRedeemInfo().isRedeemsValid){
            al::PlayerHolder *pHolder = al::getScenePlayerHolder(amy::getGlobalStageScene());
            PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);

            if(rs::isPlayer2D(player)){
                amy::log("Rejected PosRandomize because of 2D Area");
                return;
            }

            player->startDemoPuppetable();
            sead::Vector3f *trans = al::getTrans(player);
            trans->x += posVector.x;
            trans->y += posVector.y;
            trans->z += posVector.z;
            player->endDemoPuppetable();
        }
    }
}