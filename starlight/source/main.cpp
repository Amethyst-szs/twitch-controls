#include "main.hpp"
#include "al/LiveActor/LiveActor.h"
#include "al/PlayerHolder/PlayerHolder.h"
#include "al/area/AreaObjGroup.h"
#include "al/area/ChangeStageInfo.h"
#include "al/camera/CameraDirector.h"
#include "al/camera/Projection.h"
#include "al/scene/Scene.h"
#include "al/scene/SceneObjHolder.h"
#include "al/util.hpp"
#include "debugMenu.hpp"
#include "fl/server.h"
#include "game/GameData/GameDataFunction.h"
#include "game/GameData/GameDataHolderAccessor.h"
#include "game/GameData/GameDataHolderBase.h"
#include "game/GameData/GameDataHolderWriter.h"
#include "game/Info/ShineInfo.h"
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerFunction.h"
#include "game/Player/PlayerInput.h"
#include "layouts.hpp"
#include "rs/util.hpp"
#include "sead/math/seadVector.h"
#include "sead/prim/seadSafeString.h"
#include "util.h"
#include <string>
#include <typeinfo>

static bool showMenu = false;
static bool isInGame = false;
static bool prevFrameInvalidScene = true;
static HakoniwaSequence* curGlobalSequence;

DebugWarpPoint warpPoints[40];

int listCount = 0;
int curWarpPoint = 0;

static int debugPage = 0;
static int debugSel = 0;
static int debugMax = 2;
static const char* page2Options[] {
    "Disconnect from server\n",
    "Kill player\n",
    "End puppetable\n",
    "Complete kingdom (Glitch central)\n",
    "Plus 100 coins\n"
};
static int page2Len = *(&page2Options + 1) - page2Options;

// ------------- Hooks -------------

al::StageInfo* initDebugListHook(const al::Scene* curScene)
{

    // hook that gets all objects put in DebugList and adds their coordinates to a warp point array

    al::StageInfo* info = al::getStageInfoMap(curScene, 0);

    al::PlacementInfo rootInfo = al::PlacementInfo();

    al::tryGetPlacementInfoAndCount(&rootInfo, &listCount, info, "DebugList");

    if (listCount > 0) {
        for (size_t i = 0; i < listCount; i++) {
            al::PlacementInfo objInfo = al::PlacementInfo();

            al::getPlacementInfoByIndex(&objInfo, rootInfo, i);

            const char* displayName = "";
            al::tryGetDisplayName(&displayName, objInfo);

            strcpy(warpPoints[i].pointName, displayName);

            al::tryGetTrans(&warpPoints[i].warpPos, objInfo);
        }
    }

    return info;
}

void drawMainHook(HakoniwaSequence* curSequence, sead::Viewport* viewport, sead::DrawContext* drawContext)
{
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();

    // Increase the frame counter since the last ping
    smo::Layouts& layouts = smo::getLayouts();
    layouts.pingFrames++;

    if (layouts.pingFrames >= 300 && layouts.firstBoot) {
        // In this case, the game is fairly sure the server is lost and it should go idle
        if (!layouts.mConnectionWait->mIsAlive) {
            layouts.mConnectionWait->appear();
            layouts.mConnectionWait->playLoop();
            ri.rejectionID = 0;
        }

        // Every 60 frames, try a reconnection!
        if ((layouts.pingFrames + 3) % 60 == 1)
            al::isPadHoldUp(-1) ? smo::Server::instance().connect(smo::getServerIp(true)) : smo::Server::instance().connect(smo::getServerIp(false));

        // Every 10 seconds, display a new fun fact!
        if ((layouts.pingFrames + 303) % 600 == 1)
            layouts.mConnectionWait->setTxtMessage(smo::getFunFact());

    } else if (layouts.mConnectionWait->mIsAlive && layouts.firstBoot) {
        // Server is okay! Perform usual code
        layouts.mConnectionWait->exeEnd();
        layouts.mConnectionWait->setTxtMessage(u"Connection to Twitch lost! One moment!");
    }

    // Update invalid stage
    ri.isInvalidStage = al::isEqualSubString(curSequence->mGameDataHolder->getCurrentStageName(), "Demo");

    // If the stage switched from an invalid stage to valid or vise versa
    if (ri.isInvalidStage != prevFrameInvalidScene) {
        amy::updateServerDemoState();
        prevFrameInvalidScene = ri.isInvalidStage;
    }

    if (!showMenu && amy::getDancePartyState().timer <= 0 && ri.sayTimer <= 0) {
        al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");
        return;
    }

    int dispWidth = al::getLayoutDisplayWidth();
    int dispHeight = al::getLayoutDisplayHeight();

    gTextWriter->mViewport = viewport;

    gTextWriter->mColor = sead::Color4f(
        1.f,
        1.f,
        1.f,
        0.8f);

    al::Scene* curScene = curSequence->curScene;

    // Dance party overlay
    if (curScene && amy::getDancePartyState().timer > 0) {
        amy::RedeemInfo::dancePartyState& dp = amy::getDancePartyState();

        // Update the max screen edges
        dp.overlayMaxPos.x = dispWidth - 440.f;
        dp.overlayMaxPos.y = dispHeight - 80.f;

        // Changes the direction if it goes off the edge
        if (dp.overlayPos.x >= dp.overlayMaxPos.x || dp.overlayPos.x < 0)
            dp.overlayDirecton.x *= -1;

        if (dp.overlayPos.y >= dp.overlayMaxPos.y || dp.overlayPos.y < 0)
            dp.overlayDirecton.y *= -1;

        // Move the current position based on the direction and speed
        dp.overlayPos.x += dp.overlayDirecton.x * dp.overlaySpeed;
        dp.overlayPos.y += dp.overlayDirecton.y * dp.overlaySpeed;

        // Draws to the screen
        gTextWriter->beginDraw();
        gTextWriter->setCursorFromTopLeft(dp.overlayPos);
        gTextWriter->setScaleFromFontHeight(80.f);
        gTextWriter->printf("DANCE PARTY!");

        // End
        gTextWriter->endDraw();
        al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");
        return;
    }

    // Say message overlay
    if (curScene && ri.sayTimer >= 0 && ri.sayText) {
        ri.sayTimer--;

        amy::drawBackground((agl::DrawContext*)drawContext,
            sead::Vector2f { -0.75f, -1.f },
            sead::Vector2f { 1.5f, 0.09f + ((ceil(strlen(ri.sayText) / 92.f) - 1.f) * 0.06f) },
            sead::Vector2f { 0.f, 0.f },
            sead::Color4f { 0.1f, 0.1f, 0.1f, 0.95f });

        // Draws to the screen
        gTextWriter->beginDraw();
        gTextWriter->setCursorFromTopLeft(sead::Vector2f(dispWidth / 7.5f, dispHeight - 25.f - (ceil(strlen(ri.sayText) / 92.f) - 1.f) * 20.f));
        gTextWriter->setScaleFromFontHeight(20.f);
        gTextWriter->printf("%s", ri.sayText);

        // End
        gTextWriter->endDraw();
        al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");
        return;
    }

    // Standard debug menu
    if (curScene && isInGame) {
        amy::drawBackground((agl::DrawContext*)drawContext,
            sead::Vector2f { -1.f, -1.f },
            sead::Vector2f { 0.6f, 1.3f },
            sead::Vector2f { 0.f, 0.f },
            sead::Color4f { 0.1f, 0.1f, 0.1f, 0.9f });

        gTextWriter->beginDraw();
        gTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, (dispHeight / 3) + 30.f));
        gTextWriter->setScaleFromFontHeight(20.f);

        StageScene* stageScene = amy::getGlobalStageScene();
        al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
        PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);
        GameDataHolderAccessor GameData = *stageScene->mHolder;
        GameDataHolderWriter holder = *stageScene->mHolder;

        // Header
        gTextWriter->printf("Twitch Controls - Debug Menu\nPage %i/%i\n\n", debugPage + 1, debugMax + 1);
        // Scene Info
        switch (debugPage) {
        case 0:
            gTextWriter->printf("Generic Information:\n");
            // Stage / Scene Type stuff
            gTextWriter->printf("Coin Counter: %i\n", GameDataFunction::getCoinNum(*stageScene->mHolder));
            gTextWriter->printf("Stage Name: %s\n", stageScene->mHolder->getCurrentStageName());
            gTextWriter->printf("Scenario: %i\n", GameDataFunction::getScenarioNo(player));
            gTextWriter->printf("Language: %s\n", amy::getGlobalStageScene()->mHolder->getLanguage());
            gTextWriter->printf("Total Shines in Kingdom: %i\n", GameDataFunction::getWorldTotalShineNum(GameData, GameDataFunction::getCurrentWorldId(GameData)));
            gTextWriter->printf("Animation: %s\n", player->mPlayerAnimator->mAnimFrameCtrl->getActionName());
            // Player Information
            if (player->getPlayerHackKeeper()->getCurrentHackName() != nullptr) {
                gTextWriter->printf("Current Capture: %s\n", player->getPlayerHackKeeper()->getCurrentHackName());
            }
            gTextWriter->printf("Left Stick: %fx %fy\n", al::getLeftStick(-1)->x, al::getLeftStick(-1)->y);
            // Game flow information
            gTextWriter->printf("In game? %s\n", isInGame ? "true" : "false");
            gTextWriter->printf("Is paused? %s\n", amy::getGlobalStageScene()->isPause() ? "true" : "false");
            gTextWriter->printf("Is dead? %s\n", PlayerFunction::isPlayerDeadStatus(player) ? "true" : "false");
            break;
        case 1:
            gTextWriter->printf("Twitch Integration Values:\n");
            gTextWriter->printf("Reject Redeems: %s\n", !ri.isRedeemsValid ? "true" : "false");
            gTextWriter->printf("Invalid Stage: %s\n", ri.isInvalidStage ? "true" : "false");
            gTextWriter->printf("Restriction Tier: %i\n", ri.restrictionTier);
            gTextWriter->printf("Ping Frames: %i\n", layouts.pingFrames);
            gTextWriter->printf("Overlay State: %s\n\n", layouts.mConnectionWait->mIsAlive ? "true" : "false");
            gTextWriter->printf("Gravity Timer: %f\n", amy::getGravityState().timer);
            gTextWriter->printf("Wind Timer: %f\n", amy::getWindState().timer);
            gTextWriter->printf("Coin Tick Rate: %f\n", amy::getCoinTickState().speed);
            gTextWriter->printf("Hot Floor Timer: %f\n", amy::getHotFloorState().timer);
            gTextWriter->printf("Stick Inversion Timer: %f\n", amy::getStickInverState().timer);
            gTextWriter->printf("Water Area Timer: %f\n", amy::getWaterAreaState().timer);
            gTextWriter->printf("Dance Party Timer: %f\n", amy::getDancePartyState().timer);
            gTextWriter->printf("Shine Warp: %s\n", amy::getShineWarpState().isWarp ? "true" : "false");
            gTextWriter->printf("Shine Warp Target: %i\n", amy::getShineWarpState().targetShineID);
            break;
        case 2:
            gTextWriter->printf("Quick Functions:\n");

            // Draw the page
            if (al::isPadTriggerDown(-1) && !al::isPadTriggerZL(-1))
                debugSel++;
            if (al::isPadTriggerUp(-1) && !al::isPadTriggerZL(-1))
                debugSel--;

            // If scrolled past end of menu, send to start
            if (debugSel > page2Len - 1)
                debugSel = 0;
            if (debugSel < 0)
                debugSel = page2Len - 1;

            // Draw screen
            for (int i = 0; i < page2Len; i++) {
                if (debugSel != i)
                    gTextWriter->printf("%s", page2Options[i]);
                else
                    gTextWriter->printf("> %s", page2Options[i]);
            };

            // Process a selection
            if (al::isPadTriggerRight(-1) && !al::isPadHoldZL(-1))
                switch (debugSel) {
                case 0:
                    amy::log("ClientDisconnect");
                    break;
                case 1:
                    GameDataFunction::killPlayer(*stageScene->mHolder);
                    break;
                case 2:
                    player->endDemoPuppetable();
                    break;
                case 3:
                    GameDataFunction::addPayShine(holder, 30);
                    break;
                case 4:
                    stageScene->mHolder->mGameDataFile->addCoin(100);
                    break;
                }
            break;
        }

        isInGame = false;
    }

    gTextWriter->endDraw();

    al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");
}

HOOK_ATTR
void stageInitHook(StageScene* initStageScene, al::SceneInitInfo* sceneInitInfo)
{
    __asm("MOV X19, X0");
    __asm("LDR X24, [X1, #0x18]");

    amy::getRedeemInfo().isTransition = true;

    __asm("MOV X1, X24");
}

al::SequenceInitInfo* initInfo;

HOOK_ATTR
ulong constructHook()
{ // hook for constructing anything we need to globally be accesible

    __asm("STR X21, [X19,#0x208]"); // stores WorldResourceLoader into HakoniwSequence

    __asm("MOV %[result], X20"
          : [result] "=r"(initInfo)); // Save our scenes init info to a gloabl ptr so we can access it later

    return 0x20;
}

HOOK_ATTR
bool threadInit(HakoniwaSequence* mainSeq)
{ // hook for initializing any threads we need
    al::LayoutInitInfo lytInfo = al::LayoutInitInfo();
    smo::Layouts& layouts = smo::getLayouts();

    al::initLayoutInitInfo(&lytInfo, mainSeq->mLytKit, 0, mainSeq->mAudioDirector, initInfo->mSystemInfo->mLayoutSys, initInfo->mSystemInfo->mMessageSys, initInfo->mSystemInfo->mGamePadSys);

    smo::layoutInit(lytInfo);

    return GameDataFunction::isPlayDemoOpening(*mainSeq->mGameDataHolder);
}

HOOK_ATTR
void stageSceneHook(StageScene* stageScene)
{
    __asm("MOV X19, X0");

    al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
    if (!pHolder) {
        __asm("MOV X0, %[input]"
              : [input] "=r"(stageScene));
        return;
    }

    PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);
    if (!player) {
        __asm("MOV X0, %[input]"
              : [input] "=r"(stageScene));
        return;
    }

    al::LiveActor* curHack = player->getPlayerHackKeeper()->currentHackActor;
    amy::getGlobalStageScene() = stageScene;
    GameDataHolderAccessor GameData = *stageScene->mHolder;
    al::IUseCamera* UseCamera = stageScene;
    al::Projection* CamProject = al::getProjection(UseCamera, 0);
    GameDataHolderWriter holder = *stageScene->mHolder;
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();

    bool isPause = stageScene->isPause();
    bool isDemo = rs::isActiveDemo(player);
    bool isDead = PlayerFunction::isPlayerDeadStatus(player);
    bool isInterupted = isDead || isDemo || isPause || amy::getDancePartyState().timer > 0;

    // Calculate the restriction tiers if it's the first frame in the full scene
    if (ri.isTransition) {
        ri.restrictionTier = 0;
        amy::calcWorldTier(GameDataFunction::getCurrentWorldId(*stageScene->mHolder), holder.getCurrentStageName());
        // In the event that the restriction tier has changed from the last scene to this one, publish the change
        if (ri.restrictionTier != ri.lastPublishRestrictionTier) {
            amy::log("Restrict%u", ri.restrictionTier);
            ri.lastPublishRestrictionTier = ri.restrictionTier;
        }
    }

    // Once the restriction test is done, make sure to set the transition state to false!
    ri.isTransition = false;

    // Gravity timer updater
    if (amy::getGravityState().timer < 0 && !rs::isPlayer2D(player))
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
            GameDataFunction::killPlayer(holder);
            player->startDemoPuppetable();
            al::setVelocityZero(player);
            rs::faceToCamera(player);
        }
    }

    // Wind handler
    if (amy::getWindState().timer > 0 && !rs::isPlayerOnGround(player) && !isInterupted && player->getPlayerHackKeeper()->getCurrentHackName() == nullptr) {
        amy::getWindState().timer--;
        al::addVelocity(player, amy::getWindState().vect);
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
    if (dp.timer > 0 && !isPause && !isDead) {
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

    // Activate home ship yes
    GameDataFunction::activateHome(holder);
    holder.mGameDataFile->mProgressData->talkCapNearHomeInWaterfall();
    // GameDataFunction::repairHome(holder);
    GameDataFunction::enableCap(holder);

    if (!isInGame) {
        isInGame = true;
    }

    // DEBUG MENU HOTKEYS
    if (al::isPadHoldZL(-1) && al::isPadTriggerUp(-1)) // enables/disables debug menu
        showMenu = !showMenu;

    if (al::isPadHoldZL(-1) && (al::isPadTriggerRight(-1) || al::isPadTriggerLeft(-1))) {
        // Pick the menu scroll direction
        if (al::isPadTriggerRight(-1))
            debugPage++;
        else if (al::isPadTriggerLeft(-1))
            debugPage--;

        // Handle over/underflowing
        if (debugPage > debugMax)
            debugPage = 0;
        if (debugPage < 0)
            debugPage = debugMax;
    }

    // TESTING FUNCTIONS
    if (al::isPadHoldZR(-1) && al::isPadTriggerLeft(-1)) {
        // rs::appearCapMsgTutorial(stageScene, "YukimaruTutorial");
    }

    __asm("MOV X0, %[input]"
          : [input] "=r"(stageScene));
}

void seadPrintHook(const char* fmt, ...) // hook for replacing sead::system::print with our custom logger
{
    va_list args;
    va_start(args, fmt);

    va_end(args);
}

bool hotFloorHook(PlayerInput* input)
{
    return amy::getHotFloorState().timer > 0 ? true : input->isTriggerJump();
}

sead::Vector2f* stickInverHook(int port)
{
    sead::Vector2f* vec = al::getLeftStick(port);
    if (amy::getStickInverState().timer > 0) {
        vec->x = vec->x * -1;
        vec->y = vec->y * -1;
    }
    return vec;
}

bool waterAreaHook(al::LiveActor const* actor, sead::Vector3<float> const& vector)
{
    return amy::getWaterAreaState().timer > 0 ? true : al::isInWaterPos(actor, vector);
}

uint32_t shineInitHook(Shine* shine, al::ActorInitInfo* actorInitInfo)
{ // bryce code
    // al::ActorInitInfo* shineInitInfo;
    // shineInitInfo = actorInitInfo;

    // const char* stageName = shine->curShineInfo->stageName.cstr();

    // if (!shine || !stageName || !actorInitInfo) {
    //     return 0;
    // }
    // amy::log("Shine Init Hook Called HBUIDSFIBDHSUFDSFJBHK");
    // int shineID = shine->curShineInfo->shineId;
    // amy::log("%i", shineID);
    return 0;
    // return rs::getStageShineAnimFrame(shine, stageName);
};

HOOK_ATTR
bool shineControl(Shine* actor, sead::Vector3f const& location)
{
    amy::RedeemInfo::shineWarpState& info = amy::getShineWarpState();

    // Get 2D areas and check it isn't a 2D moon
    // al::AreaObjGroup* area2Ds = al::tryFindAreaObjGroup(player, "2DMoveArea");
    // if (area2Ds) {
    //     al::AreaObj* targetArea = area2Ds->getInVolumeAreaObj(location);
    //     if (targetArea)
    //         return al::isInWaterPos(actor, location);
    // }

    // Disable teleport if the target shine is collected
    if (info.targetShineID == actor->shineId && actor->isGot()) {
        amy::getShineWarpState().isWarp = false;
        info.targetShineID = -1;
    }

    // Set the target if there is no set target
    if (
        info.isWarp
        && !actor->isGot()
        && !rs::isMainShine(actor)
        && info.targetShineID == -1
        && !rs::isPlayer2D(rs::getPlayerActor(amy::getGlobalStageScene())))
        info.targetShineID = actor->shineId;

    // Check if an active shine is loaded
    if (info.targetShineID == actor->shineId) {
        // Get player
        StageScene* stageScene = amy::getGlobalStageScene();
        al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
        PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);

        // Calculate position
        sead::Vector3f target = location;
        target.y += 0.f;

        // Update
        if (player) {
            al::setTrans(actor, *al::getTrans(player));
        }
    }

    // Return the original result
    return al::isInWaterPos(actor, location);
}