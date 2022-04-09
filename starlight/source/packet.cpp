#include "fl/packet.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util.hpp"
#include "game/GameData/GameDataFunction.h"
#include "game/StageScene/StageScene.h"
#include "layouts.hpp"
#include "rs/util.hpp"
#include "sead/math/seadVector.h"
#include "sead/prim/seadSafeString.h"
#include "util.h"
#include <cmath>
#include <string.h>

namespace smo {
u32 OutPacketLog::calcLen()
{
    return strlen(message) + 1;
}

void OutPacketLog::construct(u8* dst)
{
    *dst = type;
    strcpy((char*)dst + 1, message);
}

// u32 OutPacketDemoToggle::calcLen()
// {
//     return 1; //1 byte for bool
// }

// void OutPacketDemoToggle::construct(u8* dst)
// {
//     *dst = toggleState;
// }

void InPacketPing::parse(const u8* data, u32 len)
{
    // Nothing to parse
}

void InPacketPing::on(Server& server)
{
    smo::Layouts& layouts = smo::getLayouts();
    layouts.pingFrames = 0;
    amy::log("Ping");
}

void InPacketKick::parse(const u8* data, u32 len)
{
    // Nothing to parse
}

void InPacketKick::on(Server& server)
{
    smo::Layouts& layouts = smo::getLayouts();
    server.isKicked = true;
    layouts.pingFrames = 300;
}

void InPacketSay::parse(const u8* data, u32 len)
{
    message = strdup((const char*)data);
    textFrames = (5.5 * 60) + (len * 3);
}

void InPacketSay::on(Server& server)
{
    amy::log("Say message: %s", message);
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();
    if (ri.sayText)
        ri.sayText = nullptr;

    ri.sayText = message;
    ri.sayTimer = textFrames;
    ri.sayAnimator = 20;
}

void InPacketEvent::parse(const u8* data, u32 len)
{
    eventID = data[0];
    isTwitch = data[1];
}

void InPacketEvent::on(Server& server)
{
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();
    if (isTwitch) {
        ri.rejectionID++;
        if (ri.rejectionID >= ri.rejectionIDMax)
            ri.rejectionID = 1;
    }

    if (!amy::getRedeemInfo().isTransition) {
        amy::updateRedeemStatus(isTwitch);
    } else {
        amy::sendPacketStateNotice(true, isTwitch);
        return;
    }

    if (!ri.isRedeemsValid)
        amy::sendPacketStateNotice(true, isTwitch);

    StageScene* stageScene = amy::getGlobalStageScene();
    al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);
    al::LiveActor* curHack = player->getPlayerHackKeeper()->currentHackActor;

    if (ri.isRedeemsValid) {
        // Send an update about successful packet
        amy::sendPacketStateNotice(false, isTwitch);

        switch (eventID) {
        case 1: { // PrevScene - See Myself Out
            stageScene->mHolder->returnPrevStage();
            break;
        }
        case 2: { // GravFlip - Change Gravity
            amy::RedeemInfo::gravityState& grav = amy::getGravityState();
            al::setGravity(player, amy::getRandomGravity());

            // If timer ended up at an unresonably low value, bump to zero
            if (grav.timer < -1)
                grav.timer = 0;

            if (grav.timer <= 0)
                grav.comboLength = 0;
            if (grav.comboLength >= 10)
                grav.comboLength--;
            grav.timer += (grav.addLength - (grav.comboLength * grav.comboDec)) * 60;
            grav.comboLength++;
            break;
        }
        case 3: { // Fling - Up we go
            al::LiveActor* targetActor = player;
            if (player->getPlayerHackKeeper()->getCurrentHackName() != nullptr)
                targetActor = curHack;
            al::setTransY(targetActor, al::getTrans(targetActor)->y + 250.f);
            al::setVelocityZero(targetActor);
            al::addVelocity(targetActor, { 0.f, 80.f, 0.f });
            break;
        }
        case 4: {
            GameDataFunction::disableCapByPlacement(player);
            break;
        }
        case 5: {
            ChangeStageInfo stageInfo(stageScene->mHolder, "start", amy::getRandomHomeStage(), false, -1, ChangeStageInfo::SubScenarioType::UNK);
            stageScene->mHolder->changeNextStage(&stageInfo, 0);
            break;
        }
        case 6: {
            amy::getCoinTickState().speed /= 2;
            amy::getCoinTickState().timer = amy::getCoinTickState().speed;
            break;
        }
        case 7: {
            amy::RedeemInfo::windState& wind = amy::getWindState();
            wind.vect = amy::getRandomGravity();

            // If timer ended up at an unresonably low value, bump to zero
            if (wind.timer < -1)
                wind.timer = 0;

            if (wind.timer <= 0)
                wind.comboLength = 0;
            if (wind.comboLength >= 10)
                wind.comboLength--;
            wind.timer += (wind.addLength - (wind.comboLength * wind.comboDec)) * 60;
            wind.comboLength++;
            break;
        }
        case 8: {
            amy::RedeemInfo::hotFloorState& hot = amy::getHotFloorState();

            // If timer ended up at an unresonably low value, bump to zero
            if (hot.timer < -1)
                hot.timer = 0;

            if (hot.timer <= 0)
                hot.comboLength = 0;
            if (hot.comboLength >= 10)
                hot.comboLength--;
            hot.timer += (hot.addLength - (hot.comboLength * hot.comboDec)) * 60;
            hot.comboLength++;
            break;
        }
        case 9: {
            amy::RedeemInfo::stickInverState& stick = amy::getStickInverState();

            // If timer ended up at an unresonably low value, bump to zero
            if (stick.timer < -1)
                stick.timer = 0;

            if (stick.timer <= 0)
                stick.comboLength = 0;
            if (stick.comboLength >= 10)
                stick.comboLength--;
            stick.timer += (stick.addLength - (stick.comboLength * stick.comboDec)) * 60;
            stick.comboLength++;
            break;
        }
        case 10: {
            amy::RedeemInfo::waterAreaState& water = amy::getWaterAreaState();

            // If timer ended up at an unresonably low value, bump to zero
            if (water.timer < -1)
                water.timer = 0;

            if (water.timer <= 0)
                water.comboLength = 0;
            if (water.comboLength >= 10)
                water.comboLength--;
            water.timer += (water.addLength - (water.comboLength * water.comboDec)) * 60;
            water.comboLength++;
            break;
        }
        case 11: {
            if (rs::isPlayer2D(player)) {
                al::setTransY(player, al::getTrans(player)->y + 2000.f);
                break;
            }

            amy::RedeemInfo::dancePartyState& party = amy::getDancePartyState();

            // If timer ended up at an unresonably low value, bump to zero
            if (party.timer < -1)
                party.timer = 0;

            party.timer += party.addLength * 60;

            if (party.enableFrame == false)
                party.enableFrame = true;

            break;
        }
        case 12: {
            amy::getShineWarpState().isWarp = true;
            break;
        }
        case 13: {
            ChangeStageInfo stageInfo(stageScene->mHolder, "start", "MoonWorldHomeStage", false, -1, ChangeStageInfo::SubScenarioType::UNK);
            stageScene->mHolder->changeNextStage(&stageInfo, 0);
            break;
        }
        case 14: {
            player->endDemoPuppetable();
            break;
        }
        case 15: {
            GameDataFunction::killPlayer(*stageScene->mHolder);
            player->startDemoPuppetable();
            al::setVelocityZero(player);
            rs::faceToCamera(player);
            player->mPlayerAnimator->endSubAnim();
            player->mPlayerAnimator->startAnim("RaceResultLose");
            break;
        }
        case 16:
            player->mDamageKeeper->activatePreventDamage();
            break;
        case 17:
            ri.isMusic = !ri.isMusic;
            break;
        default: {
            amy::log("Invalid EventID sent? EventID: %i", eventID);
            break;
        }
        }
    }
}

void InPacketResize::parse(const u8* data, u32 len)
{
    scaleVector = *(sead::Vector3f*)&data[0];
}

void InPacketResize::on(Server& server)
{
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();

    ri.rejectionID++;
    if (ri.rejectionID >= ri.rejectionIDMax)
        ri.rejectionID = 1;

    if (!amy::getRedeemInfo().isTransition) {
        amy::updateRedeemStatus(true);
    } else {
        amy::sendPacketStateNotice(true, true);
        return;
    }

    if (!amy::getRedeemInfo().isRedeemsValid)
        amy::sendPacketStateNotice(true, true);
    else {
        amy::sendPacketStateNotice(false, true);
        al::PlayerHolder* pHolder = al::getScenePlayerHolder(amy::getGlobalStageScene());
        PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);
        al::LiveActor* curHack = player->getPlayerHackKeeper()->currentHackActor;
        al::setScale(player, scaleVector);

        if (player->getPlayerHackKeeper()->getCurrentHackName() != nullptr) {
            sead::Vector3f* scale = al::getScale(curHack);
            scale->x = scaleVector.x;
            scale->y = scaleVector.y;
            scale->z = scaleVector.z;
        }
    }
}

void InPacketPosRandomize::parse(const u8* data, u32 len)
{
    posVector = *(sead::Vector3f*)&data[0];
}

void InPacketPosRandomize::on(Server& server)
{
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();

    ri.rejectionID++;
    if (ri.rejectionID >= ri.rejectionIDMax)
        ri.rejectionID = 1;

    if (!amy::getRedeemInfo().isTransition) {
        amy::updateRedeemStatus(true);
    } else {
        amy::sendPacketStateNotice(true, true);
        return;
    }

    if (!amy::getRedeemInfo().isRedeemsValid)
        amy::sendPacketStateNotice(true, true);
    else {
        amy::sendPacketStateNotice(false, true);
        al::PlayerHolder* pHolder = al::getScenePlayerHolder(amy::getGlobalStageScene());
        PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);
        al::LiveActor* curHack = player->getPlayerHackKeeper()->currentHackActor;

        if (rs::isPlayer2D(player)) {
            amy::log("Rejected PosRandomize because of 2D Area");
            return;
        }

        if (player->getPlayerHackKeeper()->getCurrentHackName() != nullptr) {
            sead::Vector3f* trans = al::getTrans(curHack);
            trans->x += posVector.x;
            trans->y += posVector.y;
            trans->z += posVector.z;
        } else {
            player->startDemoPuppetable();
            sead::Vector3f* trans = al::getTrans(player);
            trans->x += posVector.x;
            trans->y += posVector.y;
            trans->z += posVector.z;
            player->endDemoPuppetable();
        }
    }
}
}