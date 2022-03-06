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
#include "sead/math/seadVector.h"
#include "sead/prim/seadSafeString.h"
#include "util.h"
#include <string>

static bool showMenu = false;
static bool isInGame = false;
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
        "Random Testing\n"
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


        // curGlobalSequence = curSequence;
        // al::Projection *CameraProjection = al::getProjection(curGlobalSequence->curScene, 0);

        //Header
        gTextWriter->printf("Twitch Controls - Debug Menu\nPage %i/3\n\n", debugPage+1);
        //Scene Info
        switch(debugPage){
            case 0:
                gTextWriter->printf("Generic Information:\n");
                //Stage / Scene Type stuff
                gTextWriter->printf("Stage Name: %s\n", stageScene->mHolder->getCurrentStageName());
                gTextWriter->printf("Scenario: %i\n", GameDataFunction::getScenarioNo(player));
                gTextWriter->printf("Total Shines in Kingdom: %i\n", GameDataFunction::getWorldTotalShineNum(GameData, GameDataFunction::getCurrentWorldId(GameData)));

                //Camera information
                gTextWriter->printf("\nCamera FOV: %f\n", CamProject->getFovy());

                //Game flow information
                gTextWriter->printf("\nIn game? %s\n", isInGame ? "true" : "false");
                gTextWriter->printf("Is paused? %s\n", amy::getGlobalStageScene()->isPause() ? "true" : "false");
                gTextWriter->printf("Is dead? %s\n", PlayerFunction::isPlayerDeadStatus(player) ? "true" : "false");

                //Save file info stuff
                gTextWriter->printf("\nLanguage: %s\n", amy::getGlobalStageScene()->mHolder->getLanguage());
                break;
            case 1:
                gTextWriter->printf("Twitch Integration Values:\n");
                gTextWriter->printf("Reject Redeems: %s\n", !amy::getRedeemInfo().isRedeemsValid ? "true" : "false");
                gTextWriter->printf("Invalid Stage: %s\n", amy::getRedeemInfo().isInvalidStage ? "true" : "false");
                gTextWriter->printf("Gravity Timer: %i\n", amy::getRedeemInfo().gravityTimer);
                gTextWriter->printf("Invis Timer: %i\n", amy::getRedeemInfo().invisTimer);
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
                            ChangeStageInfo stageInfo(stageScene->mHolder, "start", "CapWorldHomeStage", false, -1, ChangeStageInfo::SubScenarioType::UNK);
                            amy::getGlobalStageScene()->mHolder->changeNextStage(&stageInfo, 0);
                            break;
                    }
                break;
        }

        isInGame = false;
    }

    gTextWriter->endDraw();

    al::executeDraw(curSequence->mLytKit, "２Ｄバック（メイン画面）");
}

void stageInitHook(StageScene *initStageScene, al::SceneInitInfo *sceneInitInfo)
{
    __asm("MOV X19, X0");
    __asm("LDR X24, [X1, #0x18]");

    amy::getRedeemInfo().isRedeemsValid = false;
    
    __asm("MOV X1, X24");
}

ulong threadInit()
{ // hook for initializing any threads we need
    __asm("STR X21, [X19,#0x208]");

    return 0x20;
}

void stageSceneHook()
{

    __asm("MOV X19, X0");

    StageScene *stageScene;
    __asm("MOV %[result], X0"
          : [result] "=r"(stageScene));

    al::PlayerHolder *pHolder = al::getScenePlayerHolder(stageScene);
    PlayerActorHakoniwa *player = al::tryGetPlayerActor(pHolder, 0);
    amy::getGlobalStageScene() = stageScene;
    GameDataHolderAccessor GameData = *stageScene->mHolder;
    al::IUseCamera *UseCamera = stageScene;
    al::Projection *CamProject = al::getProjection(UseCamera, 0);
    GameDataHolderWriter holder = *stageScene->mHolder;

    //ADD A VALUE THAT UPDATES EVERY FRAME SO THAT LOADING ZONES CAN BE FOUND AND NOT CRASH PLZ THANKS
    //Some way to update invalidStage when in Home Ship cutscenes

    //Gravity timer updater
    if(amy::getRedeemInfo().gravityTimer <= 0)
        al::setGravity(player, sead::Vector3f{0, -1, 0});
    else if(!stageScene->isPause() && !PlayerFunction::isPlayerDeadStatus(player))
        amy::getRedeemInfo().gravityTimer--;

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
    // if(al::isPadHoldZR(-1) && al::isPadTriggerLeft(-1)){

    // }

    __asm("MOV X0, %[input]"
          : [input] "=r"(stageScene));
}

void seadPrintHook(const char *fmt, ...) // hook for replacing sead::system::print with our custom logger
{
    va_list args;
    va_start(args, fmt);

    va_end(args);
}