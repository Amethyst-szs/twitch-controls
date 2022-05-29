#include "al/LiveActor/LiveActor.h"

namespace amy {

struct ActorController {
    al::LiveActor* coinTest;
    al::LiveActor* Kuribo;
};

ActorController& getActorController();

void trySummonActor(al::LiveActor* actor, float playerDistance);

} // namespace amy