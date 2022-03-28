#include "layouts.hpp"
#include "sead/random/seadGlobalRandom.h"

smo::Layouts& smo::getLayouts()
{
    static smo::Layouts i;
    return i;
}

void smo::layoutInit(al::LayoutInitInfo const& initInfo)
{
    smo::Layouts& layouts = smo::getLayouts();

    typedef void (smo::Layouts::*LayoutsThreadFunc)(void);

    layouts.mConnectionWait = new al::WindowConfirmWait("ServerWaitConnect", "WindowConfirmWait", initInfo);
    layouts.mConnectionWait->setTxtMessage(u"Connecting to Twitch Controls server!");

    // mConnectionWait->tryEndForce();
}

const char16_t* smo::getFunFact()
{
    constexpr static const char16_t* funFacts[] = {
        u"Did you know: Mario Odyssey's code calls Moons 'Shines'",
        u"Fun Fact: This mod took over a month to make!",
        u"Thank you for watching the stream!",
        u"Hopefully, Amethyst is working on fixing why the server died!",
        u"Fun Fact: Amethyst wants to buy a Virtual Boy for no reason",
        u"Good luck fixing the server Amethyst!",
        u"Fun Fact: Sleeping 8h per day is good! Buuut Amethyst hasn't been doing that",
        u"Fun Fact: France is a country that exists",
        u"Did you know: You're always growing to be the best you can be!",
        u"Did you know: Amethyst thinks you're awesome, keep being you!",
        u"You should follow amethyst_szs! She made this mod! (and this text)",
        u"Did you know: 3/4 of this code is probably copy pasted from smarter people",
        u"Fun Fact: This code was made with no caffeine!",
        u"At the time Amethyst is writing this message, it is 10:47 PM",
        u"Fun Fact: Every Switch update, people with modded switches are annoyed",
        u"Switch home themes are easier to make than this code",
        u"Fun Fact: An apple a day makes doctors get warped to the shadow realm",
        u"A joycon melts at about 240C/464F, that's a way to fix drift!",
        u"Did you know: SMO Lost Kingdoms is not a good mod",
        u"Fun fact: The water in Yoshi Star Galaxy looks terrible",
        u"Did you know: Amethyst wants dumplings right now",
        u"I feel like SMO needs more spooky captures",
        u"Fun Fact: Mokachii is a really good artist!",
        u"Fun Fact: Nut jumps are illegal under USA Flight Regulations",
        u"Did you know: A baby has more bones than you",
        u"SMO Twitch controls is sponsored by Technical Difficulties",
        u"If you take the amount of packets being successfully recieved, \nand multiply it by the number of commands the server is executing, \nyou get how amused Amethyst is right now.",
        u"Are you sure it's not a you problem?",
        u"Okay, which one of you tripped over the power cord?",
        u"At least your switch didn't crash I guess?",
        u"This could be a good time for a bathroom break",
        u"Fun Fact: The logo for this mod is not an NFT",
        u"Amethyst ran out of sandpaper to fix the imperfections in the code",
        u"Gosh, did Amethyst make ANOTHER typo?",
        u"If this crashes again, Amethyst is selling this code to Crafty",
        u"Programming is like a box of chocolates. Stuff like this happens",
        u"Epic Coding Fail",
        u"Alright was this Amethyst, Twitch, or the power cord?"
    };

    return funFacts[sead::GlobalRandom::instance()->getU32() % (sizeof(funFacts) / sizeof(const char16_t*))];
};