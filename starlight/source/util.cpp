#include "util.h"
#include "sead/container/seadListImpl.h"
#include "sead/math/seadVector.h"
#include "sead/random/seadGlobalRandom.h"

StageScene*& amy::getGlobalStageScene(){
    static StageScene* stageScene;
    return stageScene;
}

amy::RedeemInfo& amy::getRedeemInfo(){
    static RedeemInfo redeemInfo;
    return redeemInfo;
}

sead::Vector3f amy::RedeemInfo::getRandomGravity(){
    sead::Vector3f VectorOptions[] = {
        {-1,0,0},
        {1,0,0},
        {0,0,-1},
        {0,0,1},
        {0,1,0},
    };
    return VectorOptions[sead::GlobalRandom::instance()->getU32()%(sizeof(VectorOptions)/sizeof(sead::Vector3f))];
}