#include "layouts.hpp"

smo::Layouts& getLayouts()
{
    static smo::Layouts i;
    return i;
}

void smo::Layouts::init(al::LayoutInitInfo const& initInfo)
{
    // mConnectionWait = new al::WindowConfirmWait("ServerWaitConnect", "WindowConfirmWait", initInfo);

    // mConnectionWait->setTxtMessage(u"Connecting to Server.");

    // mConnectionWait->tryEndForce();
}