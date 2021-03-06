#include "main.hpp"
#include "al/LiveActor/LiveActor.h"
#include "al/PlayerHolder/PlayerHolder.h"
#include "al/actor/Placement.h"
#include "al/area/AreaObjGroup.h"
#include "al/area/ChangeStageInfo.h"
#include "al/camera/CameraDirector.h"
#include "al/camera/Projection.h"
#include "al/factory/ActorFactory.h"
#include "al/scene/Scene.h"
#include "al/scene/SceneObjHolder.h"
#include "al/util.hpp"
#include "al/util/AudioUtil.h"
#include "debugMenu.hpp"
#include "fl/packet.h"
#include "fl/server.h"
#include "game/GameData/GameDataFile.h"
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
#include "sead/math/seadMathCalcCommon.h"
#include <string>
#include <typeinfo>

static bool showMenu = false;
static bool isInGame = false;
static bool prevFrameInvalidScene = true;
static bool isPause = false;
static HakoniwaSequence* curGlobalSequence;

DebugWarpPoint warpPoints[40];

int listCount = 0;
int curWarpPoint = 0;

static int debugPage = 0;
static int debugSel = 0;
static int debugMax = 3;
static const char* page2Options[] {
    "Force Reconnection Screen\n",
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
    amy::RedeemInfo::windState& wind = amy::getWindState();

    // Increase the frame counter since the last ping
    smo::Layouts& layouts = smo::getLayouts();
    layouts.pingFrames++;

    // Check for ping frame freezes
    if (layouts.pingFrames == layouts.pingPreviousFrame)
        layouts.pingFrameStuckFrames++;
    else
        layouts.pingFrameStuckFrames = 0;

    if (layouts.pingFrameStuckFrames == 120) {
        smo::Server::instance().dcPacket();
        layouts.pingFrames = 400;
    }

    if (layouts.pingFrames >= 400 && layouts.firstBoot) {
        // In this case, the game is fairly sure the server is lost and it should go idle
        if (!layouts.mConnectionWait->mIsAlive) {
            layouts.mConnectionWait->appear();
            layouts.mConnectionWait->playLoop();
            ri.rejectionID = 0;
        }

        // Every 60 frames, try a reconnection!
        if (((layouts.pingFrames + 3) % 60 == 1) && !smo::Server::instance().isKicked)
            al::isPadHoldUp(-1) ? smo::Server::instance().connect(smo::getServerIp(true)) : smo::Server::instance().connect(smo::getServerIp(false));

        // Every 10 seconds, display a new fun fact!
        if (((layouts.pingFrames + 403) % 600 == 1) && !smo::Server::instance().isKicked)
            layouts.mConnectionWait->setTxtMessage(smo::getFunFact());
        else if (smo::Server::instance().isKicked)
            layouts.mConnectionWait->setTxtMessage(u"You have been kicked from the server!");

    } else if (layouts.mConnectionWait->mIsAlive && layouts.firstBoot) {
        // Server is okay! Perform usual code
        layouts.mConnectionWait->exeEnd();
        layouts.mConnectionWait->setTxtMessage(u"Connection to Twitch lost! One moment!");
    }

    layouts.pingPreviousFrame = layouts.pingFrames;

    al::Scene* curScene = curSequence->curScene;

    // Update invalid stage
    StageScene* stageScene = amy::getGlobalStageScene();
    ri.isInvalidStage = al::isEqualSubString(curSequence->mGameDataHolder->getCurrentStageName(), "Demo");

    // If the stage exists and the game is loading a new save file this frame, update the trackers of loads and langs
    if (curScene && isInGame && ri.isSaveLoad == 0 && !prevFrameInvalidScene && isPause)
        ri.isSaveLoad = stageScene->isLoadData() * 200;
    if (curScene && isInGame && ri.isLangChange == 0 && !prevFrameInvalidScene && isPause)
        ri.isLangChange = stageScene->isChangeLanguage() * 200;

    if (ri.isSaveLoad > 0 || ri.isLangChange > 0 || ri.isSceneKill > 0) {
        ri.isInvalidStage = true;
        ri.isTransition = true;
        isInGame = false;
    }

    // If the stage switched from an invalid stage to valid or vise versa
    if (ri.isInvalidStage != prevFrameInvalidScene) {
        amy::updateServerDemoState();
        isInGame = false;
        prevFrameInvalidScene = ri.isInvalidStage;
    }

    if (!showMenu && amy::getDancePartyState().timer <= 0 && ri.sayTimer <= 0) {
        al::executeDraw(curSequence->mLytKit, "????????????????????????????????????");
        return;
    }

    int dispWidth = al::getLayoutDisplayWidth();
    int dispHeight = al::getLayoutDisplayHeight();

    gTextWriter->mViewport = viewport;
    gTextWriter->mColor = sead::Color4f(1.f, 1.f, 1.f, 0.8f);

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
        gTextWriter->printf(dp.textDisplay);

        // End
        gTextWriter->endDraw();
        al::executeDraw(curSequence->mLytKit, "????????????????????????????????????");
        return;
    }

    // Say message overlay
    if (curScene && ri.sayTimer >= 0 && ri.sayText) {
        ri.sayTimer--;
        if (ri.sayAnimator > 0)
            ri.sayAnimator--;

        amy::drawBackground((agl::DrawContext*)drawContext,
            sead::Vector2f { -0.75f, -1.f },
            sead::Vector2f { 1.5f, 0.09f + ((ceil(strlen(ri.sayText) / 92.f) - 1.f) * 0.06f) + (ri.sayAnimator / 20.f / 4.f) },
            sead::Vector2f { 0.f, 0.f },
            sead::Color4f { 0.1f + (ri.sayAnimator / 30.f), 0.1f + (ri.sayAnimator / 30.f), 0.1f, 0.95f });

        // Draws to the screen
        gTextWriter->beginDraw();
        gTextWriter->setCursorFromTopLeft(sead::Vector2f(dispWidth / 7.5f, dispHeight - 25.f - (ceil(strlen(ri.sayText) / 92.f) - 1.f) * 20.f - (ri.sayAnimator * 4.f)));
        gTextWriter->setScaleFromFontHeight(20.f);
        gTextWriter->printf("%s", ri.sayText);

        // End
        gTextWriter->endDraw();
        al::executeDraw(curSequence->mLytKit, "????????????????????????????????????");
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
        gTextWriter->setScaleFromFontHeight(15.f);

        al::PlayerHolder* pHolder = al::getScenePlayerHolder(stageScene);
        PlayerActorHakoniwa* player = al::tryGetPlayerActor(pHolder, 0);
        sead::Vector3f pFront;
        al::calcFrontDir(&pFront, player);

        al::LiveActor* capture = player->getPlayerHackKeeper()->currentHackActor;
        GameDataHolderAccessor GameData = *stageScene->mHolder;
        GameDataHolderWriter holder = *stageScene->mHolder;

        bool lockValue = true;

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

            gTextWriter->printf("\nPlayer Front: %fx %fy %fz\n", pFront.x, pFront.y, pFront.z);

            gTextWriter->printf("\nWorld ID: %i\n", stageScene->mHolder->mGameDataFile->mCurWorldID);
            gTextWriter->printf("Is Unlocked This World: %s\n", GameDataFunction::isUnlockedCurrentWorld(GameData) ? "true" : "false");
            gTextWriter->printf("Collected Shine World Count: %i\n", stageScene->mHolder->mGameDataFile->getShineNum());
            gTextWriter->printf("World Shine Total: %i\n", stageScene->mHolder->mGameDataFile->getWorldTotalShineNum(stageScene->mHolder->mGameDataFile->mCurWorldID));
            gTextWriter->printf("Lock Number: %i\n", stageScene->mHolder->mGameDataFile->findUnlockShineNumCurrentWorld(&lockValue));
            gTextWriter->printf("Got Enough Shines: %i\n", rs::checkGetEnoughShine(player, stageScene->mHolder->mGameDataFile->mCurWorldID));

            gTextWriter->printf("\nLanguage: %s\n", amy::getGlobalStageScene()->mHolder->getLanguage());
            gTextWriter->printf("Animation: %s\n", player->mPlayerAnimator->mAnimFrameCtrl->getActionName());
            gTextWriter->printf("Left Stick: %fx %fy\n", al::getLeftStick(-1)->x, al::getLeftStick(-1)->y);
            // Game flow information
            gTextWriter->printf("In game? %s\n", isInGame ? "true" : "false");
            gTextWriter->printf("Is paused? %s\n", amy::getGlobalStageScene()->isPause() ? "true" : "false");
            gTextWriter->printf("Is dead? %s\n", PlayerFunction::isPlayerDeadStatus(player) ? "true" : "false");
            break;
        case 1:
            gTextWriter->printf("Capture Information:\n");
            if (capture == nullptr) {
                gTextWriter->printf("Capture something for information");
            } else {
                gTextWriter->printf("Current Capture: %s\n", player->getPlayerHackKeeper()->getCurrentHackName());
                gTextWriter->printf("Is actorController actor? %s\n", amy::isCaptureActorController(capture) ? "True" : "False");
                gTextWriter->printf("Capture Trans:\n  %fx\n  %fy\n  %fz\n", al::getTrans(capture)->x, al::getTrans(capture)->y, al::getTrans(capture)->z);
            }
            break;
        case 2:
            gTextWriter->printf("Twitch Integration Values:\n");
            gTextWriter->printf("Server Connected: %s\n", smo::Server::instance().isConnected() ? "true" : "false");
            gTextWriter->printf("Reject Redeems: %s\n", !ri.isRedeemsValid ? "true" : "false");
            gTextWriter->printf("Invalid Stage: %s\n", ri.isInvalidStage ? "true" : "false");
            gTextWriter->printf("Music Enabled: %s\n", ri.isMusic ? "true" : "false");
            gTextWriter->printf("Rejection ID: %i\n", ri.rejectionID);
            gTextWriter->printf("Restriction Tier: %i\n", ri.restrictionTier);
            gTextWriter->printf("Ping Frames: %i\n", layouts.pingFrames);
            gTextWriter->printf("Prev Ping Frame: %i\n", layouts.pingPreviousFrame);
            gTextWriter->printf("Freeze Frames: %i\n", layouts.pingFrameStuckFrames);
            gTextWriter->printf("Ping Calls: %i\n", ri.pingCalls);
            gTextWriter->printf("Save Swap Frames: %i\n", ri.isSaveLoad);
            gTextWriter->printf("Lang Swap Frames: %i\n", ri.isLangChange);
            gTextWriter->printf("Scene Swap Frames: %i\n", ri.isSceneKill);
            gTextWriter->printf("Bubble Frames: %i\n", ri.isRecoverBubble);
            gTextWriter->printf("\nGravity Timer: %f\n", amy::getGravityState().timer);
            gTextWriter->printf("Wind Timer: %f\n", amy::getWindState().timer);
            gTextWriter->printf("Wind Vect: %fx %fy %fz\n", wind.vect.x, wind.vect.y, wind.vect.z);
            gTextWriter->printf("Coin Tick Rate: %f\n", amy::getCoinTickState().speed);
            gTextWriter->printf("Hot Floor Timer: %f\n", amy::getHotFloorState().timer);
            gTextWriter->printf("Stick Inversion Timer: %f\n", amy::getStickInverState().timer);
            gTextWriter->printf("Water Area Timer: %f\n", amy::getWaterAreaState().timer);
            gTextWriter->printf("Dance Party Timer: %f\n", amy::getDancePartyState().timer);
            gTextWriter->printf("Shine Warp: %i\n", amy::getShineWarpState().isWarp);
            break;
        case 3:
            gTextWriter->printf("Quick Functions:\n");

            // Draw the page
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
                    layouts.pingFrames = 400;
                    break;
                case 1:
                    smo::Server::instance().dcPacket();
                    break;
                case 2:
                    GameDataFunction::killPlayer(*stageScene->mHolder);
                    break;
                case 3:
                    player->endDemoPuppetable();
                    break;
                case 4:
                    GameDataFunction::addPayShine(holder, 30);
                    break;
                case 5:
                    stageScene->mHolder->mGameDataFile->addCoin(100);
                    break;
                }
            break;
        }

        isInGame = false;
    }

    gTextWriter->endDraw();

    al::executeDraw(curSequence->mLytKit, "????????????????????????????????????");
}

HOOK_ATTR
bool sceneKillHook(GameDataHolderAccessor value)
{
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();
    ri.isSceneKill = 20;

    return GameDataFunction::isMissEndPrevStageForSceneDead(value);
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

    layouts.mainSeq = mainSeq;
    al::initLayoutInitInfo(&lytInfo, mainSeq->mLytKit, 0, mainSeq->mAudioDirector, initInfo->mSystemInfo->mLayoutSys, initInfo->mSystemInfo->mMessageSys, initInfo->mSystemInfo->mGamePadSys);
    smo::layoutInit(lytInfo);

    return GameDataFunction::isPlayDemoOpening(*mainSeq->mGameDataHolder);
}

HOOK_ATTR
void stageInitHook(StageScene* initStageScene, al::SceneInitInfo* sceneInitInfo)
{
    __asm("MOV X19, X0");
    __asm("LDR X24, [X1, #0x18]");

    amy::getRedeemInfo().isTransition = true;
    isInGame = false;

    __asm("MOV X1, X24");
}

HOOK_ATTR
void initActorWithArchiveNameHook(al::LiveActor* actor, al::ActorInitInfo const& initInfo, sead::SafeStringBase<char> const& string, char const* anotherString)
{
    // Get actorController
    amy::ActorController& controller = amy::getActorController();

    // Create actors here
    controller.coinTest = al::createActorFunction<Coin>("Test");
    controller.Kuribo = al::createActorFunction<KuriboHack>("KuriboSpawn"); // Goomba
    controller.TRex = al::createActorFunction<TRex>("TRexSpawn"); // T-Rex
    controller.Senobi = al::createActorFunction<Senobi>("SenobiSpawn"); // Uproot
    controller.Statue = al::createActorFunction<Statue>("StatueSpawn"); // Bowser's Kingdom statue
    controller.Tsukkun = al::createActorFunction<Tsukkun>("TsukkunSpawn"); // Pokio
    controller.Utsubo = al::createActorFunction<Utsubo>("UtsuboSpawn"); // Eel hole
    controller.Megane = al::createActorFunction<Megane>("MeganeSpawn"); // Moe eye
    controller.Tank = al::createActorFunction<Tank>("TankSpawn"); // Tank
    controller.Pot = al::createActorFunction<LavaPan>("PotSpawn"); // Luncheon Pot

    // Init those actors here
    al::initCreateActorNoPlacementInfo(controller.coinTest, initInfo);
    controller.coinTest->makeActorDead();
    al::initCreateActorNoPlacementInfo(controller.Kuribo, initInfo);
    controller.Kuribo->makeActorDead();
    al::initCreateActorNoPlacementInfo(controller.TRex, initInfo);
    controller.TRex->makeActorDead();
    al::initCreateActorNoPlacementInfo(controller.Senobi, initInfo);
    controller.Senobi->makeActorDead();
    al::initCreateActorNoPlacementInfo(controller.Statue, initInfo);
    controller.Statue->makeActorDead();
    al::initCreateActorNoPlacementInfo(controller.Tsukkun, initInfo);
    controller.Tsukkun->makeActorDead();
    al::initCreateActorNoPlacementInfo(controller.Utsubo, initInfo);
    controller.Utsubo->makeActorDead();
    al::initCreateActorNoPlacementInfo(controller.Megane, initInfo);
    controller.Megane->makeActorDead();
    al::initCreateActorNoPlacementInfo(controller.Tank, initInfo);
    controller.Tank->makeActorDead();
    al::initCreateActorNoPlacementInfo(controller.Pot, initInfo);
    controller.Pot->makeActorDead();

    // Once all actors are ready, reload the all actor controller array
    amy::setupActorListArray();

    for(int i = 0; i < 80; i++)
    {
        switch(sead::MathCalcCommon<float>::floor(i/10.f)){
            case 0:
                controller.massSpawnList[i] = al::createActorFunction<KuriboHack>("KuriboSpawn"); // Goomba
                al::initCreateActorNoPlacementInfo(controller.massSpawnList[i], initInfo);
                controller.massSpawnList[i]->makeActorDead();
                break;
            case 1:
                controller.massSpawnList[i] = al::createActorFunction<TRex>("TRexSpawn"); // Goomba
                al::initCreateActorNoPlacementInfo(controller.massSpawnList[i], initInfo);
                controller.massSpawnList[i]->makeActorDead();
                break;
            case 2:
                controller.massSpawnList[i] = al::createActorFunction<Senobi>("SenobiSpawn"); // Goomba
                al::initCreateActorNoPlacementInfo(controller.massSpawnList[i], initInfo);
                controller.massSpawnList[i]->makeActorDead();
                break;
            case 3:
                controller.massSpawnList[i] = al::createActorFunction<Statue>("StatueSpawn"); // Goomba
                al::initCreateActorNoPlacementInfo(controller.massSpawnList[i], initInfo);
                controller.massSpawnList[i]->makeActorDead();
                break;
            case 4:
                controller.massSpawnList[i] = al::createActorFunction<Tsukkun>("TsukkunSpawn"); // Goomba
                al::initCreateActorNoPlacementInfo(controller.massSpawnList[i], initInfo);
                controller.massSpawnList[i]->makeActorDead();
                break;
            case 5:
                controller.massSpawnList[i] = al::createActorFunction<Utsubo>("UtsuboSpawn"); // Goomba
                al::initCreateActorNoPlacementInfo(controller.massSpawnList[i], initInfo);
                controller.massSpawnList[i]->makeActorDead();
                break;
            case 6:
                controller.massSpawnList[i] = al::createActorFunction<Megane>("MeganeSpawn"); // Goomba
                al::initCreateActorNoPlacementInfo(controller.massSpawnList[i], initInfo);
                controller.massSpawnList[i]->makeActorDead();
                break;
            case 7:
                controller.massSpawnList[i] = al::createActorFunction<Tank>("TankSpawn"); // Goomba
                al::initCreateActorNoPlacementInfo(controller.massSpawnList[i], initInfo);
                controller.massSpawnList[i]->makeActorDead();
                break;
        }
    }

    // Complete hooked functionn
    al::initActorWithArchiveName(actor, initInfo, string, anotherString);
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

    isPause = stageScene->isPause();
    bool isDemo = rs::isActiveDemo(player);
    bool isDead = PlayerFunction::isPlayerDeadStatus(player);
    bool isInterupted = isDead || isDemo || isPause || amy::getDancePartyState().timer > 0;

    // Disable in-game music
    if (al::isPlayingBgm(stageScene) && !ri.isMusic) {
        al::stopAllBgm(stageScene, 0);
    }

    // Frame timer based states
    if (ri.isSaveLoad > 0)
        ri.isSaveLoad--;
    if (ri.isLangChange > 0)
        ri.isLangChange--;
    if (ri.isSceneKill > 0)
        ri.isSceneKill--;
    if (ri.isRecoverBubble > 0)
        ri.isRecoverBubble--;

    // Calculate the restriction tiers if it's the first frame in the full scene
    if (ri.isTransition) {
        ri.restrictionTier = 0;
        amy::calcWorldTier(GameDataFunction::getCurrentWorldId(*stageScene->mHolder), holder.getCurrentStageName());
        // In the event that the restriction tier has changed from the last scene to this one, publish the change
        if (ri.restrictionTier != ri.lastPublishRestrictionTier) {
            smo::Server::instance().restrict();
            ri.lastPublishRestrictionTier = ri.restrictionTier;
        }
    }

    // Once the restriction test is done, make sure to set the transition state to false!
    ri.isTransition = false;

    // Handle all redeems that run on a timer!
    amy::handleTimerRedeems(isInterupted, stageScene, player);

    // Activate home ship yes
    GameDataFunction::enableCap(holder);
    if (GameData.mGameDataFile->mCurWorldID > 1) {
        GameDataFunction::activateHome(holder);
        holder.mGameDataFile->mProgressData->talkCapNearHomeInWaterfall();
    }

    // Handle if the game is active
    if (!isInGame || !isDead || !isDemo) {
        isInGame = true;
    } else {
        isInGame = false;
    }

    // Trigger kingdom leave if you reach a kingdom early and gather a lock amount of moons
    bool isTriggerFlee = ((GameData.mGameDataFile->getShineNum()) >= GameData.mGameDataFile->findUnlockShineNumCurrentWorld(&isDemo));

    if (!GameDataFunction::isUnlockedCurrentWorld(GameData)
        && rs::checkGetEnoughShine(player, holder.mGameDataFile->mCurWorldID)
        && (!isInterupted || ri.fleeFrames > -1)
        && ri.isSceneKill <= 0
        && !ri.fleeDisabled)
        amy::triggerKingdomFlee(stageScene, player);

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

    __asm("MOV X0, %[input]"
          : [input] "=r"(stageScene));
}

void seadPrintHook(const char* fmt, ...) // hook for replacing sead::system::print with our custom logger
{
    va_list args;
    va_start(args, fmt);

    va_end(args);
}

HOOK_ATTR
bool recoverBubbleHook(al::LiveActor const* player)
{
    amy::RedeemInfo::state& ri = amy::getRedeemInfo();
    ri.isRecoverBubble = 180;
    return rs::isPlayer2D(player);
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
    // Set the target if there is no set target
    if (info.isWarp && !actor->isGot() && !rs::isMainShine(actor)) {
        actor->makeActorDead();
        info.isWarp = false;
    }

    // Return the original result
    return al::isInWaterPos(actor, location);
}

HOOK_ATTR
bool isOnDamageFire(PlayerTrigger* param_1)
{
    return amy::getfireDamageState().isDamage;
}

HOOK_ATTR
bool isCollisionCodeDamageFireGround(IUsePlayerCollision const* param_1)
{
    return amy::getfireDamageState().isDamage ? true : rs::isCollisionCodeDamageFireGround(param_1);
}

HOOK_ATTR
bool isTouchDamageFireCode(al::LiveActor const* param_1, IUsePlayerCollision const* param_2, IPlayerModelChanger const* param_3)
{
    return amy::getfireDamageState().isDamage ? true : rs::isTouchDamageFireCode(param_1, param_2, param_3);
}