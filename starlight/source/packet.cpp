#include "fl/packet.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util.hpp"
#include "game/StageScene/StageScene.h"
#include "game/GameData/GameDataFunction.h"
#include "rs/util.hpp"
#include "sead/math/seadVector.h"
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

    // u32 OutPacketDemoToggle::calcLen()
    // {
    //     return 1; //1 byte for bool
    // }

    // void OutPacketDemoToggle::construct(u8* dst)
    // {
    //     *dst = toggleState;
    // }

    void InPacketEvent::parse(const u8 *data, u32 len){
        eventID = data[0];
    }

    void InPacketEvent::on(Server &server){
        if(!amy::getRedeemInfo().isTransition){
            amy::updateRedeemStatus();
        } else return;
        amy::log("Event Redeem Claimed! EventID: %i - Rejected: %s", eventID, !amy::getRedeemInfo().isRedeemsValid ? "true" : "false");
        
        StageScene *stageScene = amy::getGlobalStageScene();
        al::PlayerHolder *pHolder = al::getScenePlayerHolder(stageScene);
        PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);
        al::LiveActor *curHack = player->getPlayerHackKeeper()->currentHackActor;
        amy::RedeemInfo &ri = amy::getRedeemInfo();

        if(ri.isRedeemsValid){
            switch(eventID){
                case 1:{ //PrevScene - See Myself Out
                    stageScene->mHolder->returnPrevStage();
                    break;
                }
                case 2:{ //GravFlip - Change Gravity
                    sead::Vector3f gravVector = ri.getRandomGravity();
                    al::setGravity(player, gravVector);
                    ri.gravityTimer += 3*60;
                    break;
                }
                case 3:{ //Fling - Up we go
                    al::LiveActor *targetActor = player;
                    if(player->getPlayerHackKeeper()->getCurrentHackName() != nullptr)
                        targetActor = curHack;
                    al::setTransY(targetActor, al::getTrans(targetActor)->y+250.f);
                    al::setVelocityZero(targetActor);
                    al::addVelocity(targetActor, {0.f,80.f,0.f});
                    break;
                }
                case 4:{
                    GameDataFunction::disableCapByPlacement(player);
                    break;
                }
                case 5:{
                    if(!stageScene->mHolder->mGameDataFile->isUnlockedWorld(2)){
                        amy::log("Rejected random kingdom because Sand Kingdom isn't unlocked yet!");
                        break;
                    }

                    ChangeStageInfo stageInfo(stageScene->mHolder, "start", amy::getRandomHomeStage(), false, -1, ChangeStageInfo::SubScenarioType::UNK);
                    stageScene->mHolder->changeNextStage(&stageInfo, 0);
                    break;
                }
                case 6:{
                    ri.coinTickRunning = true;
                    ri.coinTickRate = ri.coinTickRate/2;
                    break;
                }
                case 7:{
                    ri.windVect = ri.getRandomGravity();
                    ri.windTimer += 10*60;
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
        if(!amy::getRedeemInfo().isTransition){
            amy::updateRedeemStatus();
        } else return;
        amy::log("Resize Redeem Claimed! Rejected: %s", !amy::getRedeemInfo().isRedeemsValid ? "true" : "false");
        if(amy::getRedeemInfo().isRedeemsValid){
            al::PlayerHolder *pHolder = al::getScenePlayerHolder(amy::getGlobalStageScene());
            PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);
            al::LiveActor *curHack = player->getPlayerHackKeeper()->currentHackActor;
            al::setScale(player, scaleVector);

            if(player->getPlayerHackKeeper()->getCurrentHackName() != nullptr){
                sead::Vector3f *scale = al::getScale(curHack);
                scale->x = scaleVector.x;
                scale->y = scaleVector.y;
                scale->z = scaleVector.z;
            }
        }
    }

    void InPacketPosRandomize::parse(const u8 *data, u32 len){
        posVector = *(sead::Vector3f*) &data[0];
    }

    void InPacketPosRandomize::on(Server &server){
        if(!amy::getRedeemInfo().isTransition){
            amy::updateRedeemStatus();
        } else return;
        amy::log("PosRandomize Redeem Claimed! Rejected: %s", !amy::getRedeemInfo().isRedeemsValid ? "true" : "false");
        if(amy::getRedeemInfo().isRedeemsValid){
            al::PlayerHolder *pHolder = al::getScenePlayerHolder(amy::getGlobalStageScene());
            PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);
            al::LiveActor *curHack = player->getPlayerHackKeeper()->currentHackActor;

            if(rs::isPlayer2D(player)){
                amy::log("Rejected PosRandomize because of 2D Area");
                return;
            }

            if(player->getPlayerHackKeeper()->getCurrentHackName() != nullptr){
                sead::Vector3f *trans = al::getTrans(curHack);
                trans->x += posVector.x;
                trans->y += posVector.y;
                trans->z += posVector.z;
            } else {
                player->startDemoPuppetable();
                sead::Vector3f *trans = al::getTrans(player);
                trans->x += posVector.x;
                trans->y += posVector.y;
                trans->z += posVector.z;
                player->endDemoPuppetable();
            }
        }
    }
}