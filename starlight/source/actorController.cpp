#include "actorController.h"
#include "al/LiveActor/LiveActor.h"
#include "al/util.hpp"
#include "game/StageScene/StageScene.h"
#include "sead/math/seadVector.h"
#include "util.h"

amy::ActorController& amy::getActorController()
{
    static ActorController i;
    return i;
}

void amy::trySummonActor(al::LiveActor* actor, float playerDistance)
{
    // Get scene and player
    StageScene* stageScene = amy::getGlobalStageScene();
    al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);
    al::LiveActor* hackReference = player->getPlayerHackKeeper()->currentHackActor;

    if (actor != hackReference) {
        // Actor positioning info
        sead::Vector3f actorPos = *al::getTrans(player);
        sead::Vector3f forwardVector;
        al::calcFrontDir(&forwardVector, player);

        actorPos.x += forwardVector.x * playerDistance;
        actorPos.y += forwardVector.y * (playerDistance * 0.25);
        actorPos.z += forwardVector.z * playerDistance;

        // Prepare actor
        actor->makeActorDead();
        al::setTrans(actor, actorPos);
        al::setScaleAll(actor, 1.f);
        actor->makeActorAlive();
        actor->appear();

        return;
    }
}