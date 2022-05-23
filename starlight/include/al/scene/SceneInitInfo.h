#pragma once

namespace al {
struct SceneInitInfo {
    struct GameSystemInfo* gameSysInfo;
    void* gameDataHolder;
    char field_0x10[8];
    const char* initStageName;
    unsigned int scenarioNo;
};
}
