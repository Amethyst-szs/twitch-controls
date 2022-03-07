#include "main.hpp"
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
#include "game/Player/PlayerActorHakoniwa.h"
#include "game/Player/PlayerFunction.h"
#include "rs/util.hpp"
#include "sead/math/seadVector.h"
#include "sead/prim/seadSafeString.h"
#include "util.h"
#include <string>

static bool showMenu = false;
static bool isInGame = false;
static bool prevFrameInvalidScene = true;
static HakoniwaSequence *curGlobalSequence;

DebugWarpPoint warpPoints[40];

int listCount = 0;
int curWarpPoint = 0;

static int debugPage = 0;
static int debugSel = 0;
static int debugMax = 2;
static const char* page2Options[] {
        "Connect to Server\n",
        "Disconnect from Server\n",
        "Kill Player\n",
        "End Puppetable\n",
        "Plus 100 Coins\n",
        "OutPacket 4 - False\n",
        "OutPacket 4 - True\n",
        "Yell\n"
    };
static int page2Len = *(&page2Options + 1) - page2Options;

void drawBackground(agl::DrawContext *context)
{

    sead::Vector3<float> p1; // top left
    p1.x = -1.0;
    p1.y = 0.3;
    p1.z = 0.0;
    sead::Vector3<float> p2; // top right
    p2.x = -0.2;
    p2.y = 0.3;
    p2.z = 0.0;
    sead::Vector3<float> p3; // bottom left
    p3.x = -1.0;
    p3.y = -1.0;
    p3.z = 0.0;
    sead::Vector3<float> p4; // bottom right
    p4.x = -0.2;
    p4.y = -1.0;
    p4.z = 0.0;

    sead::Color4f c;
    c.r = 0.1;
    c.g = 0.1;
    c.b = 0.1;
    c.a = 0.9;

    agl::utl::DevTools::beginDrawImm(context, sead::Matrix34<float>::ident, sead::Matrix44<float>::ident);
    agl::utl::DevTools::drawTriangleImm(context, p1, p2, p3, c);
    agl::utl::DevTools::drawTriangleImm(context, p3, p4, p2, c);
}

// ------------- Hooks -------------

al::StageInfo *initDebugListHook(const al::Scene *curScene)
{

    // hook that gets all objects put in DebugList and adds their coordinates to a warp point array

    al::StageInfo *info = al::getStageInfoMap(curScene, 0);

    al::PlacementInfo rootInfo = al::PlacementInfo();

    al::tryGetPlacementInfoAndCount(&rootInfo, &listCount, info, "DebugList");

    if (listCount > 0)
    {
        for (size_t i = 0; i < listCount; i++)
        {
            al::PlacementInfo objInfo = al::PlacementInfo();

            al::getPlacementInfoByIndex(&objInfo, rootInfo, i);

            const char *displayName = "";
            al::tryGetDisplayName(&displayName, objInfo);

            strcpy(warpPoints[i].pointName, displayName);

            al::tryGetTrans(&warpPoints[i].warpPos, objInfo);
        }
    }

    return info;
}

void drawMainHook(HakoniwaSequence *curSequence, sead::Viewport *viewport, sead::DrawContext *drawContext)
{
    //Update invalid stage
    amy::getRedeemInfo().isInvalidStage = al::isEqualSubString(curSequence->mGameDataHolder->getCurrentStageName(), "Demo");
    
    //If the stage switched from an invalid stage to valid or vise versa
    if(amy::getRedeemInfo().isInvalidStage != prevFrameInvalidScene){
        amy::log("Changing scene to: %s", curSequence->mGameDataHolder->getCurrentStageName());
        prevFrameInvalidScene = amy::getRedeemInfo().isInvalidStage;
    }

    if (!showMenu)
    {
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

    al::Scene *curScene = curSequence->curScene;

    if (curScene && isInGame)
    {
        drawBackground((agl::DrawContext *)drawContext);

        gTextWriter->beginDraw();
        gTextWriter->setCursorFromTopLeft(sead::Vector2f(10.f, (dispHeight / 3) + 30.f));
        gTextWriter->setScaleFromFontHeight(20.f);

        StageScene *stageScene = amy::getGlobalStageScene();
        al::PlayerHolder *pHolder = al::getScenePlayerHolder(stageScene);
        PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);
        GameDataHolderAccessor GameData = *stageScene->mHolder;
        al::IUseCamera *UseCamera = stageScene;
        al::Projection *CamProject = al::getProjection(UseCamera, 0);
        amy::RedeemInfo &ri = amy::getRedeemInfo();

        // sead::Vector2f *lStick = al::getLeftStick(-1);
        // sead::Vector2f *rStick = al::getRightStick(-1);

        // curGlobalSequence = curSequence;
        // al::Projection *CameraProjection = al::getProjection(curGlobalSequence->curScene, 0);

        //Header
        gTextWriter->printf("Twitch Controls - Debug Menu\nPage %i/3\n\n", debugPage+1);
        //Scene Info
        switch(debugPage){
            case 0:
                gTextWriter->printf("Generic Information:\n");
                //Stage / Scene Type stuff
                gTextWriter->printf("Coin Counter: %i\n", GameDataFunction::getCoinNum(*stageScene->mHolder));
                gTextWriter->printf("Stage Name: %s\n", stageScene->mHolder->getCurrentStageName());
                gTextWriter->printf("Scenario: %i\n", GameDataFunction::getScenarioNo(player));
                gTextWriter->printf("Total Shines in Kingdom: %i\n", GameDataFunction::getWorldTotalShineNum(GameData, GameDataFunction::getCurrentWorldId(GameData)));

                //Camera information
                gTextWriter->printf("\nCamera FOV: %f\n", CamProject->getFovy());

                //Player Information
                if(player->getPlayerHackKeeper()->getCurrentHackName() != nullptr){
                    gTextWriter->printf("\nCurrent Capture: %s\n", player->getPlayerHackKeeper()->getCurrentHackName());
                }

                //Game flow information
                gTextWriter->printf("\nIn game? %s\n", isInGame ? "true" : "false");
                gTextWriter->printf("Is paused? %s\n", amy::getGlobalStageScene()->isPause() ? "true" : "false");
                gTextWriter->printf("Is dead? %s\n", PlayerFunction::isPlayerDeadStatus(player) ? "true" : "false");

                //Control stick inputs
                // gTextWriter->printf("\nLeft Stick: %fx %fy\n", lStick->x, lStick->y);
                // gTextWriter->printf("Right Stick: %fx %fy\n", rStick->x, rStick->y);

                //Save file info stuff
                gTextWriter->printf("\nLanguage: %s\n", amy::getGlobalStageScene()->mHolder->getLanguage());
                break;
            case 1:
                gTextWriter->printf("Twitch Integration Values:\n");
                gTextWriter->printf("Reject Redeems: %s\n", !ri.isRedeemsValid ? "true" : "false");
                gTextWriter->printf("Invalid Stage: %s\n", ri.isInvalidStage ? "true" : "false");
                gTextWriter->printf("Scene Transition: %s\n", ri.isTransition ? "true" : "false");
                gTextWriter->printf("Gravity Timer: %i\n", ri.gravityTimer);
                gTextWriter->printf("Wind Timer: %i\n", ri.windTimer);
                gTextWriter->printf("Wind Vector: %fx %fy %fz\n", ri.windVect.x, ri.windVect.y, ri.windVect.z);
                gTextWriter->printf("Coin Tick Running: %i\n", ri.coinTickRunning);
                gTextWriter->printf("Coin Tick Current: %i\n", ri.coinTickCurrent);
                gTextWriter->printf("Coin Tick Rate: %f\n", ri.coinTickRate);
                break;
            case 2:
                gTextWriter->printf("Quick Functions:\n");

                //Draw the page
                if(al::isPadTriggerDown(-1) && !al::isPadTriggerZL(-1))
                    debugSel++;
                if(al::isPadTriggerUp(-1) && !al::isPadTriggerZL(-1))
                    debugSel--;

                //If scrolled past end of menu, send to start
                if(debugSel > page2Len-1)
                    debugSel = 0;
                if(debugSel < 0)
                    debugSel = page2Len-1;

                //Draw screen
                for(int i=0;i<page2Len;i++){
                    if(debugSel != i)
                        gTextWriter->printf("%s", page2Options[i]);
                    else
                        gTextWriter->printf("> %s", page2Options[i]);
                };

                //Process a selection
                if (al::isPadTriggerRight(-1) && !al::isPadHoldZL(-1))
                    switch(debugSel){
                        case 0:
                            smo::Server::instance().connect(smo::getServerIp().serverIp);
                            break;
                        case 1:
                            amy::log("ClientDisconnect");
                            break;
                        case 2:
                            GameDataFunction::killPlayer(*stageScene->mHolder);
                            break;
                        case 3:
                            player->endDemoPuppetable();
                            break;
                        case 4:
                            stageScene->mHolder->mGameDataFile->addCoin(100);
                            break;
                        // case 5:
                        //     amy::demoToggle(false);
                        //     break;
                        // case 6:
                        //     amy::demoToggle(true);
                        //     break;
                    }
                break;
        }

        isInGame = false;
    }

    gTextWriter->endDraw();

    al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");
}

HOOK_ATTR
void stageInitHook(StageScene *initStageScene, al::SceneInitInfo *sceneInitInfo)
{
    __asm("MOV X19, X0");
    __asm("LDR X24, [X1, #0x18]");

    amy::getRedeemInfo().isTransition = true;
    
    __asm("MOV X1, X24");
}

ulong threadInit()
{ // hook for initializing any threads we need
    __asm("STR X21, [X19,#0x208]");

    return 0x20;
}

HOOK_ATTR
void stageSceneHook(StageScene* stageScene)
{
    __asm("MOV X19, X0");

    al::PlayerHolder *pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);
    al::LiveActor *curHack = player->getPlayerHackKeeper()->currentHackActor;
    amy::getGlobalStageScene() = stageScene;
    GameDataHolderAccessor GameData = *stageScene->mHolder;
    al::IUseCamera *UseCamera = stageScene;
    al::Projection *CamProject = al::getProjection(UseCamera, 0);
    GameDataHolderWriter holder = *stageScene->mHolder;
    amy::RedeemInfo &ri = amy::getRedeemInfo();

    ri.isTransition = false;

    //Gravity timer updater
    if(ri.gravityTimer <= 0)
        al::setGravity(player, sead::Vector3f{0, -1, 0});
    else if(!stageScene->isPause() && !PlayerFunction::isPlayerDeadStatus(player))
        ri.gravityTimer--;

    //Coin tick updater
    if(ri.coinTickRunning && !(stageScene->isPause()|| rs::isActiveDemo(player))){
        ri.coinTickCurrent++;

        //Tick the coin counter if the current rate is reached
        if(ri.coinTickCurrent >= ri.coinTickRate){
            stageScene->mHolder->mGameDataFile->addCoin(-1);
            ri.coinTickCurrent = 0;
        }

        //Kill the player if they hit zero coins
        if(GameDataFunction::getCoinNum(*stageScene->mHolder) <= 0){
            ri.coinTickRunning = false;
            ri.coinTickRate = 240.f;
            stageScene->mHolder->mGameDataFile->addCoin(70);
            GameDataFunction::killPlayer(holder);
            player->startDemoPuppetable();
            al::setVelocityZero(player);
            rs::faceToCamera(player);
        }
    }

    //Wind handler
    if(ri.windTimer >= 0 
    && !stageScene->isPause() 
    && !PlayerFunction::isPlayerDeadStatus(player) 
    && !rs::isActiveDemo(player)
    && !rs::isPlayerOnGround(player)){
        ri.windTimer--;

        al::addVelocity(player, ri.windVect);
    }
    //Activate home ship yes
    GameDataFunction::activateHome(holder);
    holder.mGameDataFile->mProgressData->talkCapNearHomeInWaterfall();
    GameDataFunction::repairHome(holder);
    GameDataFunction::enableCap(holder);

    if (!isInGame)
    {
        isInGame = true;
    }

    //DEBUG MENU HOTKEYS
    if (al::isPadHoldZL(-1) && al::isPadTriggerUp(-1)) // enables/disables debug menu
        showMenu = !showMenu;

    if(al::isPadHoldZL(-1) && (al::isPadTriggerRight(-1) || al::isPadTriggerLeft(-1))){
        //Pick the menu scroll direction
        if(al::isPadTriggerRight(-1))
            debugPage++;
        else if(al::isPadTriggerLeft(-1))
            debugPage--;

        //Handle over/underflowing
        if(debugPage > debugMax)
            debugPage = 0;
        if(debugPage < 0)
            debugPage = debugMax;
    }

    // TESTING FUNCTIONS
    if(al::isPadHoldZR(-1) && al::isPadTriggerLeft(-1)){
        // rs::appearCapMsgTutorial(stageScene, "YukimaruTutorial");
    }

    __asm("MOV X0, %[input]"
          : [input] "=r"(stageScene));
}

void seadPrintHook(const char *fmt, ...) // hook for replacing sead::system::print with our custom logger
{
    va_list args;
    va_start(args, fmt);

    va_end(args);
}