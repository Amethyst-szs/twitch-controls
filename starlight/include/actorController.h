#include "al/LiveActor/LiveActor.h"

namespace amy {

struct ActorController {
    al::LiveActor* coinTest;
    al::LiveActor* Kuribo;

    al::LiveActor* actorList[2];
};

ActorController& getActorController();

void setupActorListArray();
void trySummonActor(al::LiveActor* actor, float playerDistance);
bool isCaptureActorController(al::LiveActor* actor);

} // namespace amy