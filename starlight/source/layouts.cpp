#include "layouts.hpp"

smo::Layouts& smo::getLayouts()
{
    static smo::Layouts i;
    return i;
}

void smo::layoutInit(al::LayoutInitInfo const& initInfo)
{
    smo::Layouts& layouts = smo::getLayouts();

    // layouts.mConnectionWait = new al::WindowConfirmWait("ServerWaitConnect", "WindowConfirmWait", initInfo);

    // layouts.mConnectionWait->setTxtMessage(u"Connecting to Server.");

    // mConnectionWait->tryEndForce();
}