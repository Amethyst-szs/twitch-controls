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

    al::LiveActor* actorList[7];
};

ActorController& getActorController();

void setupActorListArray();
void trySummonActor(al::LiveActor* actor, float playerDistance);
bool isCaptureActorController(al::LiveActor* actor);

} // namespace amy