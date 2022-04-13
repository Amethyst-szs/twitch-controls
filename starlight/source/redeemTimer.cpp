#include "redeemTimer.hpp"

void amy::handleTimerRedeems(bool isInterupted, StageScene* stageScene, PlayerActorHakoniwa* player)
{
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();

    // Gravity timer updater
    if ((amy::getGravityState().timer < 0 && !rs::isPlayer2D(player)) || ri.isRecoverBubble > 0)
        al::setGravity(player, sead::Vector3f { 0, -1, 0 });
    else if (!isInterupted)
        amy::getGravityState().timer--;

    // Coin tick updater
    if (amy::getCoinTickState().timer > -1 && !isInterupted) {
        amy::getCoinTickState().timer--;

        // Tick the coin counter if the current rate is reached
        if (amy::getCoinTickState().timer <= 0) {
            stageScene->mHolder->mGameDataFile->addCoin(-1);
            amy::getCoinTickState().timer = amy::getCoinTickState().speed;
        }

        // Kill the player if they hit zero coins
        if (GameDataFunction::getCoinNum(*stageScene->mHolder) <= 0) {
            amy::getCoinTickState().timer = -1;
            amy::getCoinTickState().speed = 240.f;
            stageScene->mHolder->mGameDataFile->addCoin(70);
            GameDataFunction::killPlayer(*stageScene->mHolder);
            player->startDemoPuppetable();
            al::setVelocityZero(player);
            rs::faceToCamera(player);
            player->mPlayerAnimator->endSubAnim();
            player->mPlayerAnimator->startAnim("RaceResultLose");
        }
    }

    // Wind handler
    amy::RedeemInfo::windState& wind = amy::getWindState();
    if (wind.timer > 0 && !isInterupted && player->getPlayerHackKeeper()->getCurrentHackName() == nullptr) {
        sead::Vector3f* trans = al::getTrans(player);
        trans->x += wind.vect.x * wind.strength;
        trans->y += wind.vect.y * wind.strength;
        trans->z += wind.vect.z * wind.strength;
        wind.timer--;
    }

    // Hot floor updater
    if (amy::getHotFloorState().timer > 0 && !isInterupted)
        amy::getHotFloorState().timer--;

    // Stick Inver updater
    if (amy::getStickInverState().timer > 0 && !isInterupted)
        amy::getStickInverState().timer--;

    // Water Area updater
    if (amy::getWaterAreaState().timer > 0 && !isInterupted)
        amy::getWaterAreaState().timer--;

    // Dance Party updater
    amy::RedeemInfo::dancePartyState& dp = amy::getDancePartyState();
    if (dp.timer > 0 && !(stageScene->isPause() || PlayerFunction::isPlayerDeadStatus(player))) {
        dp.timer--;
        rs::faceToCamera(player);

        if (dp.enableFrame) {
            player->startDemoPuppetable();
            player->mPlayerAnimator->startAnim(dp.selectedAnim);
            stageScene->stageSceneLayout->end();
        }

        dp.enableFrame = false;
    }

    if (dp.timer == 0) {
        dp.timer--;
        player->endDemoPuppetable();
        stageScene->stageSceneLayout->start();
    }
}