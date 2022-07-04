#include "al/LiveActor/LiveActor.h"

namespace amy {

struct ActorController {
    al::LiveActor* coinTest; // Coin
    al::LiveActor* Kuribo; // Goomba
    al::LiveActor* TRex; // T-Rex
    al::LiveActor* Senobi; // Uproot
    al::LiveActor* Statue; // Bowser's Kingdom statue
    al::LiveActor* Tsukkun; // Pokio
    al::LiveActor* Utsubo; // Eel hole
    al::LiveActor* Megane; // Moe eye
    al::LiveActor* Tank; // Tank
    al::LiveActor* Pot; // Luncheon Pot

    al::LiveActor* actorList[10];

    //0-9 Goomba
    //10-19 Trex
    //20-29 Uproot
    //30-39 Statue
    //40-49 Pokio
    //50-59 Eel
    //60-69 Moe eye
    //70-79 Tank
    al::LiveActor* massSpawnList[80];
};

ActorController& getActorController();

void setupActorListArray();
void trySummonActor(al::LiveActor* actor, float playerDistance);
bool isCaptureActorController(al::LiveActor* actor);

} // namespace amy