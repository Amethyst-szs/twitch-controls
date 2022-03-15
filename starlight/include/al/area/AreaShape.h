#pragma once
#include <sead/math/seadMatrix.h>
#include <sead/math/seadVector.h>

namespace al {

class AreaShape {
public:
    void setBaseMtxPtr(sead::Matrix34f const*);

    unsigned char padding_0[0x10];
    sead::Vector3f mScale;
};
};