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
        u"Alright was this Amethyst, Twitch, or the power cord?",
        u"Your packet got stuck in public transport, expect 30 year delay...",
        u"Quick! Think of something to entertain chat!",
        u"I'm gonna try to get a new PB in fix server any%",
        u"It costs 465,999,480 USD to make a building out of 207,593 layers of paint",
        u"Code is the best spaghetti",
        u"Fun Fact: Goombas are a reference to the fictional plant known as chestnut",
        u"The characters in New Donk City are of a fictional species known as humans",
        u"That last enemy had a family. How do you sleep at night, knowing what you did?",
        u"When I said hot tub stream, I didn't mean pour water on the server omg",
        u"We have been trying to reach you about your Cappy's extended warranty",
        u"Hmm, try blowing on the cartridge to unfreeze the game",
        u"Fun Fact: Mario's blood type is 'edible!'",
        u"Fun Fact: Talkatoo is just a figment of your imagination",
        u"Person redeemed Resize!",
        u"Yoshi's tax evasion or something I'm running out of jokes",
        u"Sand kingdom's sand is a single grain of sand in every SMO cartridge",
        u"The deep woods are universally believed to be lame",
        u"Lake kingdom is more of a lagoon than a lake",
        u"Metro Kingdom has no metro",
        u"Tropical wigglers are very cute and Nintendo should make a plush of them",
        u"20 shiverians die during races every year...",
        u"Try the volleyball challenge with this mod, I dare you",
        u"Luncheon Kingdom's food is raw and missing lamb sauce",
        u"Ruined Kingdom gets its name from its ruined potential",
        u"Bowser's Kingdom is a testing ground for teleportation technology",
        u"Moon Kingdom is actually half cheese",
        u"Beat darker 2401 times and unlock a Aperture Science Dual Portal Device",
        u"Fun Fact: Wii Fit U is a game that exists",
        u"Fun Fact: The Wii U is actually good console",
        u"Fun Fact: Quagsire is at your door, look outside",
        u"Diagnosis : Skill issue"
    };

    return funFacts[sead::GlobalRandom::instance()->getU32() % (sizeof(funFacts) / sizeof(const char16_t*))];
};