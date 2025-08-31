## Project Status

August 31st 2025 EDIT: I finished working on Project64 with most games working with no issues or little issues at all. Not only that, I got french translation from one of my TAS dev associates and friends, Andzura. Not only that, but I got a ton more ROMhacks and cool features done too. However, I only finished yesterday and still trying to juggle this project and my fulltime job. So please give me a few days or so to find the time to prepare the release of WIP 10, which I am pretty confident will be the last changes from me to the source. Oh, and yeah, WIP 10 will have GLideN64! Not only that, I updated it to have no tearing, and in case GLideN64 don't work good on your PC or for your game, I made some more improvements to the plugin dynamic app behavior for Icepir8's LLE. And if you have a lower end GPU, I made a tiny more Jabo D3D8 improvements, I also got sound working really good in the private WIP 10, Jabo's DirectSound 1.6.2 sounds and works good with a lot of games which is important because the Azimer plugins do not work on some PC's. Not just that, I got a Jabo's DirectInput7 1.6.2 with the updated input spec and added setup mode, and all this syncs directly in the app, all the registries in the same folder, heck I put so much detail into this next build you can even see I updated it down to the file properties, detailed copyright and descriptions and stuff. So just bear with me, I want to demonstrate our incredible effectiveness against other versions of Project64, then I will be hanging up the towel and moving on to other projects after fulfilling myself and hopefully you too with this one. :)

I've decided to take a break from the project with WIP 09, will return later with WIP 10. EDIT: Been doing more work on this recently, think we will be getting french translation too perhaps. Should be ready sometime in August or before then. So what I'm doing is I take some time to brainstorm what I will work on next and come up with ideas, then work on it for some time and occasionally backup my work. Once I get all of my thoughts taken care of, I commit everything and release a new build with all the latest changes for you to benefit from. So the current build you see is already out of date and has already been obsoleted a while ago, but I'm still not quite finished preparing the new build either. So it requires some patience. But I really do think sometime this month, maybe next month, we can have next build ready and commit the latest changes.
If you want to make your own project from this one or continue it in some capacity, press the "Fork" button to clone the repository and begin making changes. That's what the fork button is for! 🙂

There is a discord server chatroom for Project64 1.6.2, intended for bug reports. You can chat about the project or report any issues you have in it [here](https://discord.com/invite/cHDxa9vzQM).

## About

Project64 1.6.2 is a Windows N64 software emulator derivative of Project64 1.6 developed as a fork of the 1.6 Plus offshoot of Project64 1.6.1. This emulator is highly capable, running primarily on an extremely powerful Recompiler CPU Core backed with multiple additional enhancements in tandem with a well-tested plugin library which should allow you to be able to run all Nintendo 64 games on this emulator, including ROMhacks. Additionally, this emulator project focuses on the following things:

* Prioritize gameplayability over hardware limitations (avoid emulating issues in N64's hardware).
* Prioritize the casual users' experience. No advanced features, just simple and decent emulation.
* Prioritize emulation and plugins which make full use of the capabilities in low & high-end PC's.
* Prioritize native 60FPS support whenever possible.
* Do not prioritize obscure irrelevant types of ROMs like Prototype, LodgeNet, N64DD, and PD.

You might like Project64 1.6.2 if you like these standards. Otherwise, please check out a different N64 emulator or fork the repo and develop it to be however you like.

## Setup

⚠You may want to see the Installation Disclaimer.txt before installing this application or you may encounter unexpected issues!⚠

Project64 1.6.2 is easy to setup. All you have to do is unpack the Project64 1.6.2 folder from the provided Project64 ZIP file to the desktop and launch the executable.

After you have the emulator installed, you should setup your controls. If using a controller, you may want to enable XInput mode in the N-Rage controller configuration.
Or you may want to use the SDL2 input plugin instead if you are playing with a controller and have issues with N-Rage in XInput mode. If using keyboard, just use N-Rage
without XInput mode enabled and remember to use F and G keys to control analog range for stuff like aiming. Once controls are setup, you might want to select a graphics
plugin depending on the game you're trying to play. The default graphics plugin is low-end HLE designed to maximize compatibility with PC hardware so it's guaranteed to
work with your PC regardless of your specs. But if you have a good enough PC, Icepir8's LLE is suggested for use if it's compatible with the game you're trying to play.
Next, you just have to legally obtain an N64 ROM file and open it in the emulator, then voila! You shall be able to play the game and hopefully without too many issues.
However, you may need to change your plugins around in order for the game to boot at all or look acceptable. Remember to check the ROM Notes tab in the settings to see!

## N64 Games to Play

The games listed below are expected to work well with the default settings so you won't have to worry about messing with them goddang plugins! 😉


Retail games (you cannot download these from the repository):

* The Legend of Zelda - Ocarina of Time (U/J 1.0/1.1/1.2 only)
* The Legend of Zelda - Majora's Mask (U suggested)
* Super Mario 64
* Pokémon Stadium
* Pokémon Stadium 2
* Super Smash Bros.
* Kirby 64: The Crystal Shards
* Bomberman 64
* Bomberman 64 - The Second Attack!
* Donkey Kong 64


Retail games with FPS unlocked (smooth in motion) (I cannot guarantee there won't be speed issues but if you find issues then please report it!):

* Armorines (U only)
* Banjo Kazooie (U 1.0 only)
* Banjo Tooie (U only)
* Extreme-G XG2 (U only)
* Goldeneye
* Mario Kart 64 (U only)
* Nagano Winter Olympics '98 (U only)
* Nightmare Creatures (U only)
* Penny Racers (U only)
* Turok - Dinosaur Hunter (U 1.0 only)


ROMhacks (you should download these from the repository) (some ROMhacks may NOT be suitable for kids/has dark themes/images of blood and stuff!):

* Majora's Mask Randomizer
* Pumkin Tower For D3D8 (Broken Cheats)
* Zelda Revival
* Puzzling
* Dawn & Dusk (this ROMhack is safe for kids 🙂)
* Master of Time
* Time Lost
* Sands of Time
* The Missing Link (this ROMhack is safe for kids 🙂)
* SM64 Sapphire (this ROMhack is safe for kids 🙂)
* SUPER MARIO Star Road (this ROMhack is safe for kids 🙂)
* ZTAR ATTACK 2 A Blast to the Past

## N64 Games to Play With Icepir8's LLE suggested

The games listed below are expected to work well with the the Icepir8's LLE video plugin and you will have some graphical issues if not using it!


ROMhacks (you should download these from the repository) (all of these ROMhacks AREN'T suitable for kids/has dark themes/has weird stuff in it!):

* The Sealed Palace - Play this game if you love Ocarina of Time.
* Ultimate Trial - Play this game if you love Zelda64 combat.
* Pumkin Tower - Play this game if you're a hungry pumkin! 😉


Retail games with FPS unlocked (smooth in motion) (I cannot guarantee there won't be speed issues but if you find issues then please report it!):

* Mickey's Speedway USA (U only)
* Perfect Dark
* Pokémon Snap (U only)
* Roadsters Trophy

## Project64 1.6.2 Plugin Usage Guidelines

To effectively make use of Project64 1.6.2 with all games, you will need to make use of the plugin system and review the plugin notes. When you open a ROM, if you are unable to play the game or there are bad graphical issues or there's a crash, it's possible that you need to change your plugins. From here, end emulation from the file tab, and then go to the ROM Notes tab. Here you may see a plugin note which tells you a specific plugin you need and/or suggest a specific plugin. If you are unable to open the ROM to access the ROM Notes tab in the settings, you can right click on the ROM in the ROM browser after selecting the directory where the ROM is and view the ROM Notes from there. However, there will be no ROM notes provided if the ROM is not logged in the ROM Database file (PJ64.rdb).

HOWEVER, in spite of all that, the success of each plugin is highly PC dependent. This is in-part why the default video plugin is a low-end one (Jabo's Direct3D8) which should work across all PC's.

## Special Thanks & Shoutouts

These are my special thanks and shoutouts to awesome people for their work, assistance, and/or kindness:

[@z64me](https://github.com/z64me) - For being supportive and addressing issues during the project's primitive stages when I started.

[@aglab2](https://github.com/aglab2) - I'm so glad that you researched and documented about the vulnerability inside of N64 emulators.
	 Without you, this project wouldn't have ever existed and I cannot thank people like you enough.
	 Not only that, but your Legacy fork was referenced for Project64 V1.6.2 to get improvements in.
	 Thank you for contributing valuable code which has been used to improve everyone's experiences.

gaudy-twinned0z@icloud.com / [@clickdevin](https://github.com/clickdevin) - Thanks so much for developing the Octomino SDL plugin, and for
					  giving me the tip I needed for building. You really rock dude!

[@TrulyNan](https://github.com/TrulyNaN) - "1.6 Shankle's Final Cut Gigachad Version will save us all." I love that enthusiasm of yours!

[@GlitchyDorime](https://github.com/GlitchyDorime) - I really appreciate your warm comments about this project. 10/10?!? I love hearing that!
		And you're a very good translator. Thanks for being there every step of the way with me.

[@MrsLilyCat](https://github.com/MrsLilyCat) - You're a very kind person and I especially thank you for making an awesome translation! 💖 :)

[@kev4cards](https://github.com/kev4cards) - Thanks for helping me with my credits. You have given good advice which is much appreciated.

[@satannintendo](https://github.com/SatanNintendo) - You have had a good attitude ever since we met, and thanks so much for your translation!

Bruce Shankle III - For helping me learn to use Visual Studio when I first started this project, and for
                    helping  me obtain an Uninterruptible Power Supply which has helped me tremendously.
                    Later on, he helped hook me up with high speed internet to help with my thousands of
                    GitHub commits that I was pushing to my private repositories. You're a great father!

Emmanuel Sturdivant - He gave me candy he found at the bottom of a vending machine. A heart of kindness.

l0lgamez - You were a great tester!

## This Project And Myself Have Been Slandered

In addition to all this, in spite of the childish behavior on behalf of the former Project64 Team lead by Gent, this project would not exist without their contributions
to Project64 up to Version 1.6.1. Understand that you have them to thank for previous versions. Thank you former Project64 Team. But what do I mean by childish behavior
exactly? You can see the Gent.rar file to see the raw details. But in short: this project has been slandered multiple times over by them. They refuse to explain why but
there are some theories about this in the user guide. I believe they are elitist as in they think they're special and that this gives them the right to be inconsiderate
and lie about others when their ego feels threatened, when they begin to feel a lack of self importance. The other reason is that I think they want to maintain control,
as they don't have a copyright over the code they developed off of based on the licensing in the source files, with PJ64's copyright belonging to users Zilmar and Jabo.
By misleading people about the terms of Project64's licensing and framing people who choose to develop Project64 the way they prefer it in acoordance with the licensing
they setup a situation where people may be hesitant to consider taking such action. This section is here to show you an example of the slandering made against Ver1.6.2.

https://www.project64-legacy.com/?post=warning-about-forked-repos-and-unofficial-exe-and-project-releases

there are unofficial packages/binaries being distributed by third party forks of MiB64, Project64 Legacy and Project64 1.6 Plus (So Called incorrectly Project64 1.6.2) -
First of all, all of the derivatives described here fit the definition of "unofficial packages/binaries" because they were not developed by the project64 team, which
could be decided by the founder and copyright holder of the code who calls himself Zilmar. Zilmar has created his own Project64 team for Project64 versions 1.7 and up.
Telling users that others builds based on their builds are "unofficial" as such is misleading because their builds are unofficial (what're they really trying to say?).

Second of all, Project64 1.6.2 is aptly named because its source code is based off of Project64 1.6.1's source code (and a handful of its plugins). Gent discussed PJ64
1.6.1 with me in DM's. He told me that Jabo had the source code for Project64 1.6.1 from the year 2012, and then said this during one of his voice messages:
"If we do manage to fix the vulnerability, uh, then we'll probably name it something like, uh, *inaudible* or 1.6 Extra or something like that and then make it source
available. If someone wants to further development, it's up to them."

So is what's essentially an updated Project64 1.6.1 incorrect to name it Project64 1.6.2? No, I think there's really no better name. And should someone make an update
of this Project64 1.6.2, I think the only fitting name would be Project64 1.6.3. Couldn't make any more sense. Not like it says somewhere in the license that you must
rename your project to something that makes less sense. See Gent.rar for DM's.

remove credits -
No credits removed, credits moved to the new credits section under the user guide as a cut to file size and to help make the source easier to compile
(skips afxres header requirement).

bundle unstable plugins - The plugins have been tested to be stable. In fact there had to be several modifications made to some plugins and to the emulator solely for
the purpose of getting them to be stable. However, if you use the wrong combination of plugins with some games you can get a bad bootup or random crash so be careful!
Remember to always refer to the plugin note for a ROM if given so that you can have a good experience! 😉

they violate the license - The license grants "Permission to use, copy, modify and distribute Project64 in both binary and source form, for non-commercial purposes"
under the condition that "this license information and copyright notice appear with all copies and any derived work." "Commercial users should seek permission of the
copyright holders first. Commercial use includes charging money for Project64 or software derived from Project64."
This Licensing is in the source files and included in all releases inside the Licensing folder.

they masquerade as an official distribution by including a document file with a supposed list of changes done by them - This is an unofficial build and has never at any
point in time ever "masqueraded" as an official build. In fact, no release after 1.6 is technically official. Official project64 builds will be found from www.pj64.net.
In addition to this, there is a list of notable changes which list changes between Project64 Version 1.6.1 and Project64 Version 1.6.2 in the user guide. In the initial
christmas release of this build, there wasn't enough time to get everything prepared as the point was to give people a present by gifting them with a modded Version 1.6
which had improvements made to it. Eventually, I got more free time to work on it and sorted out the gigantic list of changes between the 1.6 Plus edition of PJ64 1.6.1
although I still don't really have a full list of changes introduced in this 1.6.2 because there was so many changes to the source and everything that it's overwhelming
for me to try listing everything. If someone wants to do that I will just leave it up to them and say have at it because it sounds like a nightmare to review all of it.

crediting themselves instead of us - No, the former Project64 team people always has been and will be credited in the credits section for stuff they have done. This is
an outright lie. However, I will make very clear that this fork is developed by myself (E. Bruce Shankle IV), NOT them! So that if a user finds an issue they will know
who to go to so that the issue can be addressed and hopefully fixed in my build, and so that they may consider subscribing to my YouTube channel to show their support.

a fake license agreement which isn't found in any of the official distributions that boils down to "I'm allowed to do anything I want". We find this dangerously
misleading. Due to this we can only assume they have no good intentions, and we suspect it might have been done to try and confuse people and harm the reputation of our
team - These people call PJ64's own license agreement "fake" and lie about what it says. No it's not an open license that says you can do anything you want. The license
is not misleading at all, it's incredibly clear and I have no issue with it. Their assumptions and suspicions are unfounded as the license is not fake as they want you
to believe. To know what to believe, actually LOOK at the code yourself and see!

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Cpu.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Dma.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Eeprom.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Exception.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/FlashRam.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Interpreter%20CPU.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Interpreter%20Ops.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Main.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Memory.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Mempak.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Pif.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Plugin.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Recompiler%20CPU.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Recompiler%20Fpu%20Ops.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Recompiler%20Ops.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Registers.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Rom.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/RomBrowser.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Settings%20Api.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Settings.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Sram.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Tlb.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/Win32Timer.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/X86.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/r4300i%20Commands.c

github.com/BruceShankleIV/Project64-1.6.2/blob/db3623c13b70759f83bddbf08bea731592c24351/x86%20fpu.c


This is their 1.6 plus edition of 1.6.1 as they had left it when this project was forked, look at it with your own two eyes if you are in disbelief. Look at the license
even being in PJ64 1.7 - https://github.com/project64/project64/blob/f148d1b94ce20d55f752e8b3332db5ce96f40e05/Source/Project64/N64%20System/C%20Core/r4300i%20Commands.c
This license has also been seen in every derivative of Project64 1.6 in the source code. So it's not fair to assume but 100% fact that this is not a "fake license". The
people who want to mislead you is the former Project64 team, without a doubt. It's clear now that it's not just Zilamr who is scummy people but these people too. Who is
the main perpetrator? It's this guy who calls himself [@Gent](https://github.com/TheGent), that's who it is. Who is Gent? He is the cheats person who worked on the PJ64
Legacy edition of PJ64 1.6 who is now working on the MiB64 edition of the PJ64 Legacy edition of PJ64 1.6. If you look at the 1.6 Plus repository, he's currently trying
to employ underhanded tactics by telling folk they can't fork their repository, make changes without asking permission, and make releases. This is incredibly misleading
because not only is this not stated in the Project64 license that comes with the code, GitHub's own license says the following:

"GitHub Terms of Service Section 5. License Grant to Other Users

Any User-Generated Content you post publicly, including issues, comments, and
contributions to other Users' repositories, may be viewed by others. By setting
your repositories to be viewed publicly, you agree to allow others to view and
"fork" your repositories (this means that others may make their own copies of
Content from your repositories in repositories they control).

If you set your pages and repositories to be viewed publicly, you grant each User
of GitHub a nonexclusive, worldwide license to use, display, and perform Your
Content through the GitHub Service and to reproduce Your Content solely on GitHub
as permitted through GitHub's functionality (for example, through forking). You
may grant further rights if you adopt a license. If you are uploading Content you
did not create or own, you are responsible for ensuring that the Content you
upload is licensed under terms that grant these permissions to other GitHub Users."

Due to this, it's reasonable to assume they have malicious intentions. Please beware of the MiB/Former PJ64 Team and try to stay clear of Gent and any of his henchemen.
And remember to beware of the user Zilmar who has abused his authority over the Project64 emulator to traffic malware onto the computers of many for his self-profit! 💀

https://github.com/project64/project64/commit/f825b21de5e7e3cbc5722275754274eadb5497d0

https://github.com/project64/project64/commit/47b1456c8d46df9085a29d3eac1f08b38f77de47#diff-6de1381e0808ade83bbee235a50a864dbf7d1c635acde812440c69ea2810564c

## A Manifest Against Project64 3.0-N

We're gonna try to play some games in Project64 3.0-N, and then we're gonna try to play some games in Project64 1.6. And we will see if Project64 3.0-N is "superior".🤔

Project64 3.0-N VS Project64 1.6: Is it really "SUPERIOR" to 1.6 like the author has claimed it to be on their website?


Game 1: Pokémon Stadium (U)

Project64 1.6: Game boots with some RSP warnings if Limit FPS is off but works great.

Project64 3.0-N: Game crashes right after the N64 logo.


Game 2: Mario Tennis (U)

Project64 1.6: Game boots with some graphical issues but it's playable.

Project64 3.0-N: Game does not even boot.


Game 3: Paper Mario (U)

Project64 1.6: Game boots and runs with some graphical issues but it's playable.

Project64 3.0-N: Game does not boot and the emulator throws multiple errors and crashes completely.


Game 4: Donkey Kong 64 (U)

Project64 1.6: Game boots, seems to look alright.

Project64 3.0-N: Game boots, but the title screen demo segment is completely screwed up. Donkey Kong is walking into invisible walls and everything looks wrong.


Game 5: Yoshi's Story (U)

Project64 1.6: Game boots and runs with some graphical issues but it's playable, especially with Direct3D6 plugin instead.

Project64 3.0-N: Game crashes right after the Nintendo logo.

That was 5 retail games and this person's Project64 which they claim to be better than 1.6 is either crashing on boot, doesn't boot, or doesn't emulate the game right.
Not only that, the 5 games listed above work in the official Project64 V3.0 which means their fork does not improve upon, but makes Project64 worse than it already is.
While the author didn't specify which select set of games their fork focuses on functionality and improved general gameplay experience of, I will assume it is ROMhacks
they're referring to so let's try some of those and see how they compare against Project64 1.6 with 8MB default memory size.

Game 6: Crystal Clocks

Project64 1.6: Subscreen has delay but the game is fully playable otherwise. The subscreen delay was later fixed in 1.6.2 by including the LLE video plugin.

Project64 3.0-N: Game does not even boot.


Game 7: Indigo: Chapter 2 - v0.4.6

Project64 1.6: Requires Direct3D6 if THE LEGEND OF ZELDA internal name is not hex edited out. Player must use states due to a save issue. This was later fixed in 1.6.2.

Project64 3.0-N: Game does not even boot.


Game 8: Fierce Deity Mask in OoT

Project64 1.6: Requires Direct3D6 if THE LEGEND OF ZELDA internal name is not hex edited out. Works good besides some graphic issues.

Project64 3.0-N: Pausing in the game can just crash everything.


Game 9: Izou Zelda Patch 1.0

Project64 1.6: No issues at all.

Project64 3.0-N: I tried to boot the game and then the emulator froze on the "plugin initializing". I had to close it, and when I tried again the game didn't even boot.


Game 10: Zelda's Birthday (there was a "console compatible" version of this one but it introduced regressions and still wasn't fully console compatible unfortunately)

Project64 1.6: Works perfect with Use TLB unchecked in the ROM settings first.

Project64 3.0: Appears to work with TLB unchecked first, but the game crashes when entering kokiri forest which makes it impossible to beat the game normally.


And of course, all ROMhacks of the first 5 games mentioned in the list probably don't work in the emulator either, which means there's even more ROMhacks out there that
are not going to work in Project64 3.0-N but will work in Project64 1.6 (or some of them will even work in the official Project64 V3.0!). So I think that this perfectly
demonstrates the true quality of this emulator. A version of Project64 which claims to be better than 1.6 but fails and is worse than its parent. Not only that, while I
was testing with it, the .exe would continue to run in the background frequently and the program froze on me at several points on a "plugin initializing" window. Yikes!
But probably the most annoying thing is the exe running in the background as then if you try to boot the app up again, then you will have to click through an option box
to close the Project64 which is secretly running in the background every single time it does that. And Project64 3.0-N does that very frequently, so it's very annoying.

So now you understand the true craftsmanship that goes into N64 emulators like Project64 3.0-N. An emulator which by all accounts is inferior to Project64 1.6 as an emu
in many respects. I would say other Project64 Versions, especially those based on Version 1.6, easily surpass it right now due to the extremely poor game compatibility.
Clearly the author of Project64 3.0-N is just an egotistical fool as their fork cannot substitute for 1.6 in any meaningful capacity whatsoever. The creator blows a lot
of hot air with making bold claims like saying "On early 2023, Project64 3.0.1-N-v1 was released, and it would eventually evolve into what we know today as Luna's
Project64. As of today, I am confident in saying that Luna's Project64 is superior to 1.6 as we knew it a few years ago, and the recent developments in 1.6's history
have only made it worse for 1.6 itself." This entire statement is part of a propaganda, as the recent developments in Project64 1.6's history have made it a more viable
emulator due to new safety improvements and enhancements, and Luna's Project64/Project64 3.0-N is obviously not superior to Version 1.6 or even its parent, Version 3.0.

## ⚠ Actions Which Indicate Unhealthy and Harmful ROMhacking Communities to Avoid ⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠

DISCLAIMER: THIS SECTION DISCUSSES SOME DISTURBING TOPICS INCLUDING HARASSMENT, HATRED, AND OBSCENE BEHAVIOR FOR EDUCATIONAL PURPOSES (to warn users about harmful behaviors to avoid).
OBSCENE BEHAVIOR IS DISCUSSED IN HISTORICAL CONTEXT AND FOR EDUCATIONAL PURPOSES ONLY AS ALLOWED BY GITHUB'S TERMS OF SERVICE.
[GitHub ToS](https://docs.github.com/en/site-policy/acceptable-use-policies/github-sexually-obscene-content)

IF THESE TOPICS WOULD MAKE YOU UNCOMFORTABLE, DON'T SCROLL ANY FURTHER DOWN THAN THIS!

⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠
⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠⚠

There is a reasonable chance you downloaded this emulator to play and have a great gaming experience with retail games and ROMhacks too. However, ROMhacking communities
could have a focus on people's superiority complexes, amassing and disseminating hatred against anyone who doesn't fit into their agenda, usually by employing unethical
tactics alike Slander (the action or crime of making a false spoken statement damaging to a person's reputation) to help amass and disseminate the aformentioned hatred.
The point of this section is to list different examples of this so that when you are seeking out ROMhacks to use in the emulator, you will know what to look out for and
be able to keep yourself from getting caught up in unhealthy communities which emphasize such twisted values. But what this section is not about is targeting any people
with the examples given soley to give you an idea of what to expect and be aware of so that you will be able to detect when you have involved yourself with a bad group.
I've tried to avoid naming users throughout this section for this reason as I want you to focus more on the actions and not the person/their aliases who did the action.
You might be able to lead a more healthy online life this way by avoiding focusing on hatred towards others and distancing you and your online persona from hate groups.

We will take a look at parts of the Super Mario ROMhacking community for this section. To start, I will look over the Romhacking.com website database for SM64 ROMhacks.

Below you will find a security advisory from the [Romhacking.com](https://romhacking.com/) website.

"Security Advisory
Romhacking.com recommends the usage of the Parallel Launcher emulator, using the ParallelN64 emulator core.

The popular third-party emulator Project 64 has been found to contain a vulnerability in versions older than 3.0 that allows for a malicious N64 rom file to execute
arbitrary code on your computer outside of the emulator. A similar vulnerability also exists in Bizhawk prior to version 2.10, as well as all current official
releases of Mupen64plus. For this reason, we strongly encourage all users to avoid using vulnerable versions of these emulators."

This sounds like bad news, until you realize that the most used version of Project64 (Version 1.6), has already had said vulnerability completely patched. So why aren't
they making people aware of that? If you look in the Discord server for RHDC, you will see a follow-up message suggesting users to use Project64 3.0-N or an application
called "Parallel Launcher". But Project64 3.0-N as we just tested is an incapable emulator which is incapable of obsoleting Project64 1.6 like the creator claims to be.

First of all, let's think about why they would do this, as I have contacted one of the administrators of this site before and directly told them about the vulnerability
being fixed already by the founder of Project64, Zilmar. I have archived Zilmar's vulnerability fixed Project64 1.6 [here](https://drive.google.com/file/d/19FnmX-75XrULetD1-Y0CwgF1wrtwtmvM/view?usp=sharing).
The Romhacking.com admin also had access to this link. The most obvious motive here is simple to identify as they are obviously not interested in giving the user a safe
emulator option while prioritizing the best ROMhack support as most ROMhacks were developed using Project64 1.6 due to later versions having incredibly severe problems.
With that understood we see that they are promoting other emulators in the background to substitute for Project64 1.6, one of which whose authors has also claimed to be
"superior" despite having way worse game and ROMhack compatibility than what it's claimed to be superior to. That, combined with having seen Parallel Launcher users who
have issues playing some ROMhacks on the Parallel Launcher application, with its users saying things like, "There was an issue with Parallel Launcher (I think it was at
least) that prevented me from using the owl since the game would crash 100% if I punched the tree." and "If you don't want to encounter any bug, you need to use
Project64 1.6 for this one" or "i seem to have problems with getting the game to run at all. parallel launcher with ParaLLEl does not boot at all, GlideN64 is at like 1
frame per minute and OGRE does the same. tried the other plugins and best result i got were angrylion which works perfectly but looks VERY pixelated and Rice which
works fine but all entities seem to be completely black with no texture, idk what is causing this but hope someone can help" or "Doesn't seem to be compatible with
Parallel Launcher. I get a black screen no matter what settings I use." or "P.S. This will not work on parallel launcher. Use PJ1.6" lets me know that Parallel Launcher
cannot substitute for Project64 1.6 either. Taking that all into account, the takeaway from recommending other applications to substitute for Project64 1.6 which cannot
actually substitute it combined with the decision to not announce the vulnerability being fixed in Project64 1.6 combined with the attitude coming from the guy who made
Project64 3.0-N where they claimed to be "superior" to 1.6 leads me to believe this: the website doesn't care about users having a good experience really, they're aware
that Project64 1.6 is a popular emulator and when they saw a single weakness in its design, they (the people part of the website) took advantage of their opportunity to
promote their emulators and obfuscate the truth about Project64 1.6's actual viability as a capable emulator eligible for use with ROMhacks using proper plugins for it.
This way, they can affirm their emulator as "superior" to Project64 1.6 despite not actually making enough effort to be better. An amazing example of superiority-minded
behavior fueled by propaganda.

************************************************************************************************************************************************************************
Additionally, Parallel Launcher is not even an emulator as it is just a launcher for the ParallelN64 and Mupen64Plus-Next emulators, which makes it a front-end program.
Due to this, it is semantically incorrect to say that Parallel Launcher is an actual emulator as it is just a GUI for other emulators, similarly to Rosalie's Mupen GUI.
Standalone N64 emulation projects like Mupen64Plus, Zilmar's Project64, or this Project64 1.6.2 are emulators.
************************************************************************************************************************************************************************

Now let's take a look at the About section of one of the Romhacking.com trusted users' profile and see what it has to say about themselves.

[Bio - "That one german guy that made the Star Revenge Series and remade vanilla SM64 to be finally a good game.
Been hacking SM64 since 2012 and still going at it.
If you wanna see me make these hacks, then check out my Twitch streams.
Also citified hater of extreme hacks, screw the level design in them.
And don't question the story in my SR hacks, it's made up as I went along making them."](https://romhacking.com/user/BroDute)

Another sign of the superiority complexes, "remade vanilla SM64 to be finally a good game". Most likely the author is referencing the ROMhack called SM64 Decades Later.
The about section of that ROMhack says:

["Ever wondered what SM64's levels would look like after a few years, maybe even 2 decades after the events the original when Bowser took over the Castle with now 150
new stars to collect? Well, this hack aims to recreate and reimagine vanilla SM64 levels from memory to an extent where some levels will feel familiar yet new and
others that have been turned into fully new levels. Stars or sections that felt too repetitive or just weren't fun in the eyes of the hack creator have been replaced
with newer stars or been reworked to play out differently. So in short, don't expect this to be a 1 to 1 remake of sm64. If you desire new stuff, the endgame close to
100% is filled with new original stages. Good luck and have fun!"](https://romhacking.com/hack/sm64--decades-later)

So once again, we see the same thing: another user claiming their creation is superior to something else and that Super Mario 64 was never even a good game whatsoever!

[On christmas of 2021, Super Mario Star Road Retooled would release on YouTube](https://www.youtube.com/watch?v=HdMHkK1MPO0). In the video, it appears to be a rerelease
of a ROMhack called Star Road originally created by a user who goes by the alias "Pyro Jay". The video opens with "Pyro Jay Presents" "Rebuilt from the ground up 10
years later..." "Star Road Retooled" And the video showcases lots of levels in this ROMhack rerelease before cutting to "Revisit a timeless classic..." "... with
improved visuals, bug fixes, gameplay features, and more!" "New areas to visit and redesigned stars to collect!" "Star Road Retooled" and the video fades to black, then
says "Hack by Pyro Jay" "Trailer by SpK" "Available Now!" "Link in the description". Strangely enough, several sound cracks are also heard while listening to the video.

But there's several things that are fishy about this and a very big catch. That catch being that this hack wasn't solely made by Pyro Jay and it doesn't belong to them,
and that this is an unauthorized rerelease which seems to have been created in an attempt to make a supposed superior version compared to [SKELUX's official mod release](https://www.youtube.com/watch?v=_JBdxcnyxeQ).
Super Mario Star Road is a ROMhack made by an extremely talented hacker named SKELUX who developed awesome tools which were then used by others to create Mario 64 mods.
In a similar manner to the Project64 3.0-N discussed earlier which also claimed to be superior to something else, Super Mario Star Road Retooled is really an unecessary
and buggy version which is less optimal to play than the official release. Upon the initial release of Super Mario Star Road Retooled, here's what people said about it:

(Chat taken from SKELUX's Discord Server)

START

"What is special about this? It looks like Star World but upscaled rendering and kinda buggy? I thought it was a PC port but it's emulated so what makes this different
from the normal Star Road? Doesn't really say anything about it in the description."

"No originality" "And it says currently broken in title"

"It added more bugs than it fixed tbf"

"apparently version 1.0 has impossible stars because pyro jay didn't even bother testing the whole thing"

"there's also a warp under the star that crashes the game"

SKELUX: "what have they done to my boy"

"they removed the cheese protection for bowser 2...
why?"

SKELUX: "lol its not even cheat protection, that is a really odd visual mistake to make
you can see the ground go inside the castle, it is not supposed to look like that"

"cant believe pyro made star road 😨"

"pyro jay the type of guy to copy and paste something and make it worse"

"looks like a copy and paste with everything upscaled"

"The trailer makes it look like something good
But once you experience it yourself you realize its so bad"

SKELUX: "at any rate, it is a curious existence"

"WOW THEY BROKE THIS DOOR LOL" <- This user shows a clip of skipping stars needed for the final boss due to Pyro's rerelease being more glitchy than the official build.

"collecting the final star in night hub, crashes the emu lol"

"its a joke lol
the playtester didnt test i guess"

"alright so i got 119 stars on retooled, and overall it is really mediocre
a lot of the replacement stars are lame, and most of the time they don't even replace the bad star of the level"

"Pyro Jay is the type of guy to copy and paste star road add a few s***ty stars create more bugs just to put his name with star road"

"secret ruined" "i guess pyro jay sucked too much so he ruined the secret easter egg" - This shows a clip of a special secret easter egg in Star Road which is no longer
secret in Star Road Retooled, ruining the whole concept.

END

Another user posts screenshots of messages from Pyro Jay where Pyro Jay told someone they shouldn't rush their ROMhack, and then less than 6 months later, Pyro Jay says
they rushed Super Mario Star Road Retooled.

START

"inconsistent bugfix at it's finest" - This shows a clip of mario being unable to obtain a star through an unintended method, but in the same level it allows the player
to jump on top of a sloped part of the level and walk right up it to easily obtain a star through unintended means.

"the hack is super buggy but honestly I don't think you're giving him enough credit. like the goal of this project was to recreate the entirety of star road in a
non-outdated tool, which includes remodeling and retexturing every area (original ones are broken after exporting)
and that takes some time, it's not like he copy pasted the whole thing and sprinkled in some extra bugs for flavor"

"none of that looks remodelled"

SKELUX: "y tho" "literally all you need to do to edit the original im pretty sure is remove the checksum again"

SKELUX: "does not really seem to warrant remaking the whole game" "just another way to get my name further down on the credits" "for a game i 100% made myself"

"the fact you're barely mentionned and only in the middle of the credits is really scummy imo"

SKELUX: "lol yeah hes got that totally wrong. it would be appropriate form to credit himself, and then me second"

SKELUX: "everyone trying to gradually remove me from the credits of my own game"

SKELUX: "next someone will make a hack of this, as is intended, and will just credit him for making "retooled" with no need to credit skelux"

SKELUX: "heres the thing, i wouldnt have a problem if people made a hack of my game or something" "but instead two people have now just directly attempted to hijack
my project in its semi-original form, thus diverging traffic away from my original release" "at a fraction of the effort i put into making the actual game"

"well at least the green comet hack has your name listed first"

SKELUX: "that sort of thing is pretty nice" "what i do not like are these hacks that try to render my original release obsolete" "just an outright hijack"

SKELUX: "star world is fully encrypted, file name table wiped, checksum protected, immune to gameshark, among various other things"

SKELUX: "intention is to prevent cheating to find the games biggest secrets, as well as protecting against my projects being hijacked" "which has happened to me a lot"

SKELUX: "when you put a lot of work into something, other people love to come along and put in a fraction of the work modifying it just so they can slap their name over
yours" - This message from SKELUX received 5 Upvotes.


SKELUX: "something i quickly learned when leaving the community, they feel entitled to appropriate and steal everything i've ever contributed"

END

Keep in mind that the "Star World" project mentioned above is SKELUX's current project which includes remastering Mario Star Road. If the user Pyro Jay really wanted to
make the Star Road video game better, they would've contacted SKELUX and offered to assist or support his work in making [the Star World project](https://www.youtube.com/watch?v=aTYLHRfFjzY).

It should be increasingly obvious by now that this isn't a superior version of the original game and is an unauthorized rerelease created to attribute credit to another
person and to appropriate the work as they like with little regard for the original design philosophy or concepts of SKELUX's jawdropping Super Mario Star Road ROMhack.

Now that you've seen several examples of what superiority-minded behavior looks like, I am going to show you some examples of how hate becomes disseminated and amassed.

On the Super Mario Star Road Retooled page on the Romhacking.com website, you will notice that the mod is granted an even higher rating than the official Star Road mod.
In the comments of the Retooled ROMhack, the comments include:

"My Game keeps crashing on Course 8 when I go into the hole in the tall grass. I'm using Parallel Launcher."

"I would say, this is the best way to experience Star Road as of now, but I wouldn't say it's the truly the definitive. This hack is just "What if Star Road was made in
2023?" and it's basically that, will say, it does remove the big amount of collision issues, adds quality of life features like Aglab's Cam and Infinite Lives as well
as the most annoying stars from the original, and replaces with completely new ones, sometimes with really nice looking sub-areas, my favorite detail for this hack, it
adds, but it doesn't feel out of place. Now why I think this could be better, there are some weird design choices, like replacing C12 entrance with a train, which is
very cute, but the window itself was replaced by a (personally) ugly looking mirador, or some of the stars in Courses that didn't need to be changed, but they were,
which removed most of their difficulty or were just weird (e.g: C5 Star 6, B1 Secret Star, C15 star 6, the OW Star in the Sewers section having a pretty useless VC
for no reason, and most of the Red Coin Stars being removed for Courses like 9, or just being way to close to each other making them way to bare-bones like in Course
10). I wouldn't consider this a downgrade, but I much preffer playing the Original over this Retooled version
By the way, I played this on Project64 1.6, and the game crashes in C6 when Mario gets hurt by the small exploding bombs, it happened twice when trying to go for the
Star and also after defeating B3 and reaching the OW at night, the trees that are there have the snow particle."

"Was a good time except being soft locked in the purgatory hole because I went there before 120 stars
Warp hole isn't there and got super soft locked because I couldn't get out of the water"

"I mean I literally playtested this hack so I'm super biased but it's a fun romp if not a little bit too little of a discernable hack from Star Road. Improvements
here and there but nothing that makes me go oh sh** this is the definitive version. But hey, it's still Star Road."

"Can't recommend this version for those who wants to get ALL stars. How are you supposed to get the Star Replica at MMT if you don't have a flyguy to bounce on?
Extremely frustrating."

"Good update to Star Road. It adds many improvements such as parallel cam and infinite lives and changes many of the existing stars (even adding entirely new
sub-areas). However, I question some of the choices here... Changes like parallel cam certainly make the game better and are more than welcomed, but changes like
removing or oversimplifying most of the red coin stars felt unnecessary to me.

So, is this worth playing? Yes!
Is this the definitive way to play Star Road? I don't think so... but maybe your opinion will differ depending on how much you enjoy the changes made for this
version."

It seems many people from Romhacking.com who have played the original game don't consider the retooled to be truly a superior version either. But now we will review the
section on Romhacking.com reserved for SKELUX's official Star Road Release (which probably is not authorized by SKELUX who has his own YT download video for Star Road).

First of all, the overall rating of the official release which people seem to like most is rated lower than Retooled which people hesitate to call better. Very strange.
The about section for the page says:

"The original star road was created by Skelux, and later optimized for play on an actual N64 console by Kaze. The console compatible version can be found in the other
versions of this hack page.

The recommended version(s) for play are 1.0, 1.01 (NOT 1.0 Fix 1), or Deluxe. 1.0 is the original full release. 1.01 sets the default camera to Mario cam. Deluxe has a
completely different camera and lighting system, with some fixed wall clips. Other versions are not recommended, and in some cases not permitted in speedruns."

It starts to become obvious that the website is just hosting SKELUX's work and telling people to download buggy versions of his ROMhack which will give the user a worse
experience than if they downloaded the Final version from SKELUX's YT download video. The page does not link to SKELUX's YT download video where a user would be able to
obtain the correct version of the game to have a proper experience. It additionally claims Star Road was optimized to play on N64 hardware but this is not true as their
supposed console compatible version still crashes on N64 hardware at some point in the game, meaning it is not a true port. Not only that, when Kaze ported Star Road to
be more compatible with N64 hardware, he was previously added as an author of Super Mario Star Road despite the mod being made by SKELUX, not Kaze who made an N64 port.
The N64 console compatible port may not actually be fully console compatible anyways? Although people do say it works for the most part.

"A masterpeice!  On console the game crashes in the boat star of the coral reef course. This pertains to the console optimized version."

Meanwhile, if we take a further look into the comments of the section for this unofficial page for SKELUX's Star Road, we eventually come across comments like...

"Pretty well-made hack overall
I feel like this hack focuses on graphics more than gameplay, which makes the experience suffer a bit.

Favourite stage: Colossal Candy Clutter (C11)
Least favourite stage: Large Leaf Forest (C8)

Sidenote: I couldn't get the boat to spawn in C6, but I managed to get star 3 by using a bounce off a nearby Fly Guy.
I have removed my piece about Skelux from the original comment since I want to separate drama/personal character stuff from entertainment
basically, Skelux is a person who has said some awful stuff and hurt some people + more"

"Still a really good hack that holds up today, it is a classic after all.

Also did the creator do or say something bad? This is in reference to the most recent comment."

"after a bit of digging he apparently has said some pretty f***** up things akin to a 14 year old /pol/ frequenter"

"Amazing hack, unfortunately I can't recommend it because of who it was made by. 7/10 (if it was made by someone else, 8/10)"

"Wtf bro why the name of the creator remove a point ???"

"It's nothing to do with his name, it's just that (in my opinion) skelux is an absolutely terrible person."

We're starting to see some hate here. Calling the author of this amazing hack an absolutely terrible person, claiming that he hurt some people and said messed up things
after a bit of digging. This will be looked at soon enough, but what I want you to realize is that the user SKELUX is actually a pretty normal guy who has got slandered
by people involved with SM64 ROMhacking and these are the results. Hatred is amassed, anything you've contributed is to be poorly received, people will steal your credit
from you or attribute others to your work just for making their own unauthorized rereleases and ports. This also explains why despite SKELUX being a lot more skilled and
talented than hijackers like Pyro Jay, his work appears to be somewhat rating bombed.

To give you more examples of the disemmination of hatred, let's revisit the ROMhack author of the Star Revenge series and Decades Later. What are his ROMhacks like?

[Star Revenge Redone 1.3 v2.0](https://romhacking.com/hack/star-revenge-redone) starts off with a sign which reads:

"Welcome back to old new courses but still old from 2013 amirite. Why would you play this anyway?
Credits:
SM64 hack made by ProDute
Original Game by Nintendo
BroDute - Levels, graphics
SMWCentral - level textures

Thanks to:
LinCrash - De-meming, text redaction and bunch of new changes
SMWCentral - music and textures
Frauber, Trashlux - obsolete tools
Aglab, Kaze, Pilzinsel - ASM patches, tweaks
Everyone else - for other cool stuffs

Now stop playing this trash!"

Take a look at the 2nd part of the sign text while giving thanks to those people. Frauber and Skelux, who made the original tools used to create Super Mario 64 ROMhacks
aren't even properly credited. What tools from them were used is not made clear, all they want you to know is that their tools have been obsoleted by SUPERIOR tools and
that SKELUX is TRASH by name-calling him TRASHLUX.

[Taking a look at Star Revenge 1.5 Star Takeover Redone 2.2](https://romhacking.com/hack/star-revenge-15-star-takeover-redone), the signpost at the beginning reads as follows:

"Star Revenge (1.5) - Star Takeover Redone Version 2.2
Hack made by BroDute
Original Game by Nintendo.

Levels - BroDute
Music - The cool guys from SMWCentral

Textures:
Enemys - BroDute
HUD - BroDute

Leveltextures - BroDute and SMWCentral
Tools - Frauber, VL-Tune, Trashlux"

Now we've seen it twice, persistent name-calling and hatred directed against SKELUX despite his work being used to develop their ROMhacks/mods. Imagine using somebody's
work to help create your work and then insulting them over and over. Are you starting to sense the hate yet? Let's move on to another Star Revenge and see what we find.

Take a look at [Star Revenge 4.5 The Kedama Takeover REWRITTEN v1.3](https://romhacking.com/hack/star-revenge-45-the-kedama-takeover-rewritten), the sign at the start of the game reads as follows:

"Star Revenge 4
The Kedama Takeover
- Rewritten -

Hack by BroDute
Original game by Nintendo

Levels by BroDute
Textures by BroDute and SMW Central
HUD and enemy Textures by BroDute
More objects patch objects by Kaze

Tools: VL-Tune, Frauber pilzinsel64... and the memetrash Skelux

Music: Read signs in front of level entrances! Read the sign to the right for the unlisted music

Beta Testers: Asbeth and MarioCrash

Special thanks: Pilzinsel64 for various things."

By now, perhaps you are starting to see how hate gets spread in this community: by making video games for people to be entertained with and inserting insults into them.
But in order to really understand how bad the hate is and how it even got amassed in the first place, let's examine more of the conversation in SKELUX's Discord server.

This conversation talks about how someone trash talked Skelux's work whose viewers also harassed him.

START

SKELUX: "dude has done damage to my work and legacy in ways that can not be undone, as have various others from the rom hacking community. the one thing i can do in
return is to not contribute anything to the attention they get"

"how that happened?"

"many people still use your softwares to romhack sm64"

SKELUX: "torched pretty much all my sm64 related projects"

SKELUX: "every time i made a editor update he would stream trash talking it and all his viewers would come harass me"

SKELUX: "as a result support for the editor was dropped"

SKELUX: "now it remains in a permanently buggy state, and you know who to thank for that"


SKELUX: "after year and years of spending countless hours contributing tools and documents to the community, i was banned from every rom hacking platform and still
harassed to this day all from one slightly alternate opinion i shared"

SKELUX: "net positive on my end, since no time is wasted on these people anymore"

SKELUX: "the opinion i shared was that gender neutral bathrooms were dangerous for women and trans people alike, and for this i was told to kill myself and banned
everywhere"

END

SKELUX has alleged that he...

* was villainized as a Nazi
* was called a terrible person
* was impersonated as part of a slander tactic
* was insulted in several popular ROMhacks
* got harassed by people
* was miscredited in derivatives of his best and most significant past work
* had all of his SM64 projects hijacked/appropriated without permission
* had his work trash talked
* had things made up about him so that his reputation could be further damaged
* ultimately suffered a great deal from what can best be described as mobilized hatred and slandering which all came from people involved with Mario 64 ROMhacking

I even decided to contact SKELUX about this through direct message, and asked if all of the things stated above are true. He said this:

"all true, though to be fair from my end, there are many occasions where ive deliberately further provoked these people out of amusement at their cult-like behavior.
despite the years of work i put into helping build the community, just one tiny percieved slight on their dumb life perspective was all it took to flip and try to
destroy me
i was already going through a bit of a tough time when this stuff was first happening, so i didnt really handle it all perfectly"

But what does he mean by "deliberately further provoked these people"? Well... he explains his Discord server icon change here:

START

SKELUX: "i changed it to a bone swastika for a while to trigger the people harassing me and it worked"

SKELUX: "they talk about it to this day" "call me a nazi and i will be the villain you desire"

SKELUX: "literally calling me nazi for weeks and months then act all outraged when i change the server to a bone swastika"

SKELUX: "i am not someone who backs downs and cowers and apologizes, so there is only one way for me to go"

END

In addition to all of this, SKELUX's experience with the SM64 ROMhacking community has contributed to a paranoia of his which has made him a lot more antisocial,
making him a lot more difficult to approach. Whenever somebody he does not know DM's him, even if they are saying to him they are a fan of his and being thankful
for his work, he assumes that they could be a harasser from the SM64 ROMhacking community and that they are out to get him and runs from the convo. It's a shame.

Not only that, but I had my own experience similar to SKELUX's which lead to at least one attempt to make me out like an extremely screwed up person like SKELUX.


My experience started when I was contacted by people from the SM64 ROMhacking community who warned me about a guy who was caught grooming a kid sometime in 2021.
I and a couple members from the community worked together to warn the SM64 ROMhacking community about this individual's pedophile-esque behavior in addition to a
[conspiracy which was created by one of the groomer's friends to convince people that the groomer was falsely accused of grooming a child](https://www.youtube.com/watch?v=EiJo5xC71f8).
(The video linked above commentated by the groomer is derived from a script written by one of their friends who had been trying to manipulate people into viewing
the groomer as someone who was "sexually harassed" by the 13 year old, despite no evidence to support this, as while the 13 year old in question was hypersexual,
the groomer groomed them by rewarding sexual drawings and sexual talk from the 13 year old with laughter, sending several sexual drawings, bringing up techniques
to provide sexual stimulation, speaking very sexually, and generally speaking would always respond positively to anything sexual to further promote this behavior
while telling the 13 year old that their, "rapidly declining mental health and failing grades" will get better without them trying to improve it, which indicates
that they don't really have any real interest in the 13 year old being cognitively functional and would rather have them continue to be hypersexual with them and
without anybody knowing about this in any capacity, as evident by the encouraging of secrecy which you will see if you scroll down just a little bit further. But
the groomer's friend tried to make people believe that they were lied to about the grooming to convince people to let the groomer to be part of a community which
"is full of young vulnerable queer teenagers" even though they have groomed a 13 year old by encouraging sexual activity and images from them and telling them to
lie to others to keep this a secret. And while trying to convince people the groomer is such a victim of supposed harassment from  the 13 year old, they tried to
frame the person who had been warning others about the grooming as a liar who falsely accused their friend of grooming.)

Messages about this were immedietly deleted without hesitation and then one of the Discord moderators from the SM64 ROMhacking community contacted me to tell me,
"do not share this anywhere else". We had warned people about how the groomer had normalized a purely sexual relationship with a cognitively disabled 13 year old
even though they clearly knew better, but were still caught doing it anyways and going so far as permitting said 13 year old to send sexual pictures of themself.

While they were getting groomed, the 13 year indicated to others about the kind of relationship they were apart of and the groomer responded to this by demanding
the 13 year old to [convince people that what indicated the kind of relationship the groomer was having with them was just a joke and to lie to everyone:](https://drive.google.com/file/d/147QjkVFJsEXtx5UWsJkmXZH-lHy2V6C9/view?usp=sharing)

START

[Groomer](https://romhacking.com/user/Pluto): "Tell them you were joking"

[Groomer](https://romhacking.com/user/Pluto): "tell them f***ing anything to convince them"

END

After this, the groomer immediately tried to justify their decisions, claiming the 13 year old wanted this and that they're also a minor so that
makes this okay, and was clearly very angry at the 13 year old for letting on to others about this kind of relationship that they were apart of.
How conveniently the change of words is, from previously saying there's "a pretty big gap" in their age which can used for "Taking advantage of"
people who are many years younger than they are and the groomer saying that acts of sex with kids under the age of consent is "not really good".

Later on, when talking to the kid about them indicating to others about the kind of relationship they were involved in, [the Groomer said this:](https://drive.google.com/file/d/1DmIOdP4g5G7t4DjdFL72wVvJ6XXQGuSB/view?usp=sharing)

START

[Groomer](https://romhacking.com/user/Pluto): "Dont do that again"

[Groomer](https://romhacking.com/user/Pluto): "Thats such a sh**ty thing to do"

[Groomer](https://romhacking.com/user/Pluto): "Im not saying you are a sh**ty person but you did something really f***ed up"

END

In the end, the result of the Groomer scolding the 13 year old for making people aware of their relationship and making them feel bad for it lead to saying...

START

The 13 year old: "I promise from now on I'll keep our stuff private"

END

Which seems to be exactly what they wanted the 13 year old to say. Grooming complete.

[The 6 Stages of Grooming: Preventing Child Sexual Abuse](https://mcasa.org/assets/files/Stages_of_Grooming_Fact_Sheet_11.2023.pdf)

This is stage 6:

"Those who commit child sexual abuse commonly use threats or guilt to maintain power over the child. The
offender wants the child to keep the abuse a secret and ensure the child's continued participation. The offender
will engage in emotional manipulation: "If you tell your dad about this, he is going to hate you;" "If you tell, I
won't be able to give you gifts anymore;" "If you tell, your mom is going to get hurt." The offender may
normalize the inappropriate behavior with the child, or even blame the child for letting the abuse happen."

The groomer also said "I mean i only saw you as a friend" after the 13 year old implied they had developed romantic feelings for the groomer. 💔 Poor kid.

Since I was told not to talk about this and make people aware by a Discord moderator of the SM64 ROMhacking community I decided to take up this grooming with its
[server owner](https://romhacking.com/user/SimpleFlips). He didn't take any action and called it "minor's romance" because the groomer said they are 16. However,
there's nothing romantic about relationships which are purely sexually focused and especially being encouraged to be kept in secret. But the kids abuse is called
as "Romance" and justified. [The cognitively disabled 13 year old who states that their mental health is "rapidly declining" to the clearly exploitative groomer](https://drive.google.com/file/d/1B9XmsareqAFi5Lad_rgjYC6IFqX4IDvG/view?usp=sharing)
and the groomer who states that there "is a pretty big cap" in their age which can used for "Taking advantage of" people who are many years younger than they are
and also stated to be aware that the 13 year old was below the age of consent of sexual activity were most definetly not part of any kind of so-called romance!!!
The groomer seems to have decided to take advantage when they realized how vulnerable this 13 year old person was and how they can normalize very sexual behavior
with and get inappropriate images from the 13 year old, and was confident in the controlling of the relationship should anything become public by guilting the 13
year old if they made it known with anybody and using them to cover it up. Nobody should have to suffer from such a so-called romance from this creep, especially
not little kids. And to any parents reading this, perhaps consider this question: [WHAT ARE YOUR KIDS LEARNING?!?!?](https://www.youtube.com/watch?v=5IDbbIsL07A)
Because I don't think any parent should allow their kids to be using the Discord platform in any capacity let alone to be involved in groups that allow grooming.

Contrast that reality to [the author of Project64 3.0-N's article on Project64 1.6](https://sites.google.com/view/shurislibrary/project64-16) as dissected below.

"Project64 1.6 should be left to die" - Project64 1.6 is an emulator with many flaws but with many advantages over others as well, just like all other emulators
out there. It has great PC hardware support and works really well with ROMhacks provided the right plugins/settings are used. It should not be abandoned because
it's a highly appreciated emulator and both one of the most stable and reliable Project64's compared to all the other official one's out there for many reasons.

Additionally, Project64 1.6 is a good emulator, just like how Super mario 64 is a good game regardless of what the Decades Later or Project64 3.0-N authors say.
And Luna's Project64 is not superior to Project64 1.6, as demonstrated previously it contains many regressions and there is no attractive reason to be using it.

"there is unlikely to be any good reason to allow Project64 1.6 but not Luna's Project64 in any given game" - There is obviously at least one very strong reason
not to allow Luna's Project64/Project64 3.0-N: some games don't even boot up at all. Additionally, since the video plugin isn't doing any framebuffer emulation,
several graphical effects will not show up at all, at least not without the user manually changing to a different video plugin or enabling framebuffer in GLide.

"Project64 2.2 inadvertently hosted its installer on a website that added malware to every file that it hosted." -
Early versions of Project64 2.0 series were bundled with malware inside the installer. The above information is false and potentially intended to deceive users,
such that they will redirect blame for the malware away from Zilmar who deliberately put the scummy stuff in the installer to illegally profit off of his users.
See [his commit](https://github.com/project64/project64/commit/f825b21de5e7e3cbc5722275754274eadb5497d0) for an example of 1 instance of Zilmar doing this (he's
done this multiple times and gotten away with it).

"the logical step forward is to take a release that is open source and completely fine except for one issue, and simply remove that issue. That being, the
nagware screen in 3.0. Just like that, we end up with a Project64 version that has lots of additional features and nearly no drawbacks when compared to 1.6" -
Project64 3.0-N doesn't have "nearly no drawbacks when compared to 1.6", it clearly has significant drawbacks because ROM and ROMhack compatibility is horrible.

"Load times are fully consistent between 1.6 and 3.0, and also accurate to console." - I know this is false because while testing the OoT x MM combo randomizer,
there is more than one second of load time difference between 1.6 and 3.0, with 3.0 having less load time than 1.6, which gives 3.0 an unfair advantage over 1.6
users.

"People who pick up Project64 1.6 with default settings will be taken straight into using the 3 Jabo plugins, and each one of them comes with its own set of
issues." - Project64 1.6 comes with 5 not 3 jabo plugins: Jabo's Direct3D8 1.6, Jabo's Direct3D6 1.5.2, Jabo's DirectSound 1.6, Jabo's DirectInput7 1.6, and
the RSP emulation Plugin. Each plugin has its own set of advantages as well which would be too detailed and complicated to quickly explain here.

"Jabo's Direct3D8, a very inaccurate plugin that doesn't even work as intended anymore due to discrete GPUs' deprecation of DirectX 8" - Jabo's Direct3D8 is the
default video plugin for Project64 1.6.2 precisely because it has always worked on everyone's computers. Discrete GPU's did not directly depreciate DirectX8, it
is just that rapid evolution pushed the industry toward newer APIs and DirectX8 became considered "outdated", although it still functions very well to this day.

"we have advanced decomp projects that rely on console accuracy in order to work" - There is no project out there which really requires truly accurate emulation
to work, it just needs a functioning/working emulation which is capable of emulating what is required for each game. And that's exactly what Project64 does.

"Jabo's DirectAudio was never even good in the first place. It was full of crackling and sync issues, and unlike Jabo graphics, its performance was abysmal." -
It's called Jabo's DirectSound, and it's good for people who have issues with sound on their computer who cannot use Azimer's Audio plugin at all.

"As I have previously stated, one of the main "selling" points of Project64 1.6 was the fact that it never changes.
Even just for that reason, changing it would defeat the whole point of using 1.6 in the first place." - No, one of the main reasons to use 1.6 is because of its
good game and ROMhack compatibility. As long that and PC hardware compatibility or user experience isn't compromised, there's no reason not to update PJ64 V1.6.
If this wasn't true, users wouldn't be trying to add additional dll plugin files like GLideN64 or Azimer's Audio plugins into the Project64 1.6 "Plugin" folder.

And now, we have perhaps the worst section of the entire article. The supposed "downfall" of Project64 1.6. Which is better described as a 2nd awakening as the
vulnerability being announced is what started my initiative to create a better version of Project64 based on Version 1.6 that retains hack support and is safe.

"On January 2nd 2024, the SM64 romhack "Mystery of Purple Switch" was released, showcasing a vulnerability found in Project64 1.6 that allows malicious ROMs to
execute arbitrary code on your PC. This was a massive security flaw that was announced as such in several major N64 servers, and people were urged to switch to
a different emulator as soon as possible. This served as a catalyst for many different events to occur, including but not limited to the development of
Project64 Legacy and Project64 Plus by a team of people that included some of the original Project64 1.6 developers." - Another incorrect piece of information
by the oh-so-talented author of Project64 3.0-N. First of all, the aformentioned ROMhack was released on February 2nd 2024, which means all ROMhacks files made
before that date should be safe to run. Second of all, this isn't a massive security flaw because the patch was incredibly simple to make. Third of all, Legacy
was not started because of a small vulnerability exploit. Project64 Legacy has been in development for a very long time even if it has little to show for that.
In Gent.rar, you will see Gent saying that Project64 Legacy began development in 2008 and had already been releasing beta builds before the announcement of the
vulnerability on [their GitHub repository](https://github.com/pj64team/Project64-Legacy/releases). Project64 Plus or Project64 1.6 Plus was however created due
to the vulnerability with the idea of retaining ROMhack compatibility while having the vulnerability fixed while Version 1.6.2 goes above and beyond that goal.

"Accuracy has never been a concern or a goal in either of these forks, so the same issues that used to haunt romhack developers are all still there (with the
exception of the TEQ crash)." Project64 3.0-N is especially an inaccurate emulator not doing any framebuffer emulation at all just to support older hacks and
some games don't even boot/has weird crashes meaning that being inaccurate is really meaningless in the face of ROMhacks. Functionality is what is important.
Additionally, Project64 Legacy is a WAY more accurate emulator than other Project64's and was of a very high concern judging by the tests I have run with it.


""It is targeted at the communities that still desire to use 1.6's ability to play Rom Hacks and a-like because of an incorect core that lets a lot of things
slide when it comes to compatibility."

Yes, those spelling mistakes are in the readme, but let's not judge someone who may be foreign or dyslexic... even though they have an entire group of people
and access to autocorrect tools to proofread and spellcheck.

Let's instead judge the claim that 1.6 has an "incorrect core" that has the exclusive ability to play romhacks.
Because that's an utter lie." - The author of Project64 3.0-N makes things up on the spot, claiming that the README file said it has the exclusive ability to
play ROMhacks. No, 1.6 has an ability to play ROMhacks and its ability is generally better than all other emulators while the right plugins are getting used.

"Older romhacks tend to have issues with accurate graphics plugins as they were built with Jabo graphics in mind, and that is no longer an issue thanks to
ANGLE GLideN64" - Wrong, many ROMhacks still require the use of Jabo's Direct3D8 to function properly. This is why in the overhead plugin guide of V1.6.2,
it says ANGLE GLideN64 is for "Most SM64 ROMhacks graphics", as some of the older one's still require Jabo's Direct3D8 to function properly, or with ANGLE
it will function to a mostly good degree but the graphics will still look better in Jabo's Direct3D8 overall due to the differences in graphics emulation.

"Every romhack that works on Project64 1.6 will also work on Luna's Project64, and there are currently no known exceptions to this statement." - Utter lies.

"I shall preface this section by saying that I have looked into Bruce Shankle's recent GitHub history related to N64 emulation, and the literal first two
things I found were... very interesting. The first one was a fork of ANGLE GLideN64 where he changed many of the default settings. The first setting I
immediately looked at enabling framebuffer emulation. ANGLE GLideN64 is a plugin designed for SM64 romhacks, and in that context, enabling framebuffer
emulation will do nothing other than adding 2 frames of input delay and breaking compatibility with old romhacks." - Another utter lie, enabling framebuffer
is required for most retail N64 games created to function without horribly jarring graphical effects such as for Majora's Mask Lens of Truth/motion blurring
effects/OoT pausescreen bg. I was considering that ANGLE GLideN64 could be a viable video plugin for Project64 1.6 and having framebuffer enabled by default
and disabled framebuffer in the per-game config for ROMhacks which require framebuffer off. But I scrapped the idea due to GLideN64's OpenGL implementation.

"The second was a commit to Project64 Plus improperly done from mobile updating the .cht file that had to immediately be reverted." - I didn't even have a PC
and I still tried to help contribute to the emulator because I desperately wanted it to be better. I was able to update the RDB, but when I tried to make an
update for the CHT (cheat) file, it didn't work right and needed to be reverted.

"Needless to say, the fact that someone that is making those sorts of changes is part of the team further extends my concerns regarding the team's apparent
incompetence and disregard for the community's needs and wishes." - I am NOT apart of the former Project64 team whatsoever nor do I wish to be! In addition
to this, the former Project64 team is not incompetent as they created Project64 Version 1.6 which is a great emulator for its era. I fully believe in their
abilities, I just think that they don't test their work enough and their recent work has suffered as a result and been unusually more sloppy than expected.
Regarding a community's needs and wishes, this is something the fork button is reserved for.

"Bruce Shankle has also recently reached out to an SM64 romhack community moderator asking for a public announcement to be made on how Project64 1.6's
vulnerability was fixed. This won't be done because:

It would be misleading and may lead people to believe that the original Project64 1.6 is safe now

The romhack community in general does not want Project64 1.6 to come back in any way due to the inaccuracies found in it." - It would not be misleading to
make people aware that the vulnerability is fixed, but it would be misleading for the vulnerability to be fixed and then not tell anyone about it and while
telling people there is a vulnerability in Project64 1.6. That's extremely misleading and it is irresponsible for a community moderator to choose not to
make people aware about the vulnerability having been fixed and continue to mislead people. And without the inaccuracies inside of Project64 V1.6, a lot
of ROMhacks cannot function at all so it would not make any sense why a ROMhacks community would be against an inaccurate emulator at all. Egocentric bias?

"Ultimately though, nothing is as concerning as the fact that, in the last several months, Bruce Shankle has repeatedly and unjustifiably spread false
pedophilia allegations against several community members, which even lead to one of the most loved SM64 romhack creators essentially being bullied off of
the internet." - I never alleged "several community members" of pedophilia, this is completely made-up. All I did was warn the community about the guy who
groomed a 13 year old along with one of their friends' conspiracies which was created to convince people to allow the groomer to be around a community with
many children. And after that happened, I got muted and my message deleted immedietly anyways. And for that, information is now made-up about me spreading,
"false pedophilia allegations against several community members". I also have no involvement with any bullying, nor do I know who is being even referred to
in this excerpt. Perhaps the harmful community decided to spread hatred and now I am the scapegoat.

"Even if the emulator was good and addressed the community's needs, which it seems to have no chance of doing, we wouldn't want our community to promote a
project that such a person is involved with." - Why would anyone care about an unhealthy community which allows grooming kids in the name of false romance
and emphasizes on superiority and spreading hatred? As the key takeaway from all these experiences is that unhealthy communities should be avoided because
of promoting harmful activities which should be discouraged and nobody should participate in. But now this also serves as a perfect example of how they'll
try to get you and hurt your life: they don't like something you say so they spin whatever story they can about you to make you look as worse as possible.
This section of their article also indicates that they are the same type of person as the aformentioned groomer's friend who lied to others about grooming
and tried to frame the person who warned them about grooming as a liar, as they have done the exact same thing that they did to them, but now doing to me.

"Simply switch to Luna's Project64, as you will get a better gameplay experience than with any 1.6 variant, and move on. You'll be doing a favor both to
yourself and to romhack creators." - Luna's Project64/Project64 3.0-N is, generally speaking, a downgraded Project64 3.0 which supports less games, less
ROMhacks, with minimal benefits over other options. If you want to play some ROMhacks, just use Project64 1.6.2 which is an actually great emulator that
supports all the relevant ROMhacks and games out there to some degree and comes with better plugins than the one's in 3.0-N and more optimized databases
and way more awesome content and optimizations. There is really not much reason to use 3.0-N over 1.6.2 due to Ver1.6.2 having a lot better of generally
everything. Probably the only major department where 3.0-N does hold an edge would be: audio slightly and linux support. As while I'm satisfied with the
audio support in 1.6.2,I think it would benefit from a Shankle_Audio with fixed audio timing from 1.7, and Project64 1.6.2 is windows only. But no other
advantages seem present, as the emulation quality present in Project64 3.0-N is even worse than the original 1.6 and the emulation in 1.6.2 has improved
upon 1.6's emulation in many different ways, which means 3.0-N is multiple generations behind in terms of emulation quality and game/hack compatibility.
Additionally, there are other programs out there which work on Linux like Parallel Launcher or Mupen64Plus forks and their emulator does not really have
much to offer compared to those linux compatible alternatives. And for the windows department, Ver 1.6.2 obsoletes it in almost every way besides audio.
So I personally don't see why you would use Project64 3.0-N, since other options like 1.6.2 or Parallel Launcher have improved compatibility with games.
I also think it's obvious now that my earlier explanation was correct about why they weren't talking about the fix to the vulnerability in Projec64 1.6,
that they were trying to get people to transition to their own emulator and do so by capitalizing off a single small weakness in its design, all because
they need to affirm to themselves and everyone else that they are "superior"! Oh look at me, I am better than 1.6! Shouldn't use those 1.6 emulators! 👿

Last but not least, the same type of situation that happened to SKELUX is happening to other SM64 ROMhack makers as well. When reaching out to the user,
"Christopher Rios Lillo" about the existence of two B3313 ROMhacks with one of them called "unabandoned", here's what he had to say...

START

"I am the creator and lead developer of b3313, at one point i worked with benedani aka chlorobyte and thegreatest roman, these 2 users ended up organizing
a raid to my discord server to take control, hijacked the rhdc pages, tried to hijack my music distributors as i'm a musician, and have been spreading
misinformation about me, and harrassing me and my mods ever since, its been 2 years and no one does anything, i would advise to stay away from these
people.

at some point the harrasement was so bad that i dropped version .9, that had a lot of issues, at the time assumed to be abandoned but i kept working on
it secretly to avoid drama

there was a dark era where everyone was just: RELEASE WHEN????? even with the ongoing drama"

Bruce Shankle IV: "so basically, people were so impatient and harassing you over waiting for you to finish your hobby hack that you were compelled to
tell people you had abandoned the project in hopes that people would quit demanding for a release, and instead of this they then hijacked your project
by taking all of your assets that they could from you and making their own game from it without your approval and then called it the "unabandoned"
edition?"

"yes"

Bruce Shankle IV: "and then after you revealed you continued development by publishing the actual game, what sequence of events followed, they made up
some drama about you and threatened to doxx you?"

"yes, and they have been harrasing me to this day"

Bruce Shankle IV: "in what form is their harassment, verbal over the internet?"

"yes"

Bruce Shankle IV: "do they name-call like typical kindergarten bullies?"

"pretty much"

Bruce Shankle IV: "okay. so that about sums it up right, in addition to romhackint.com choosing to host the hijacked build of your project similar to star
road retooled."

"yes

simple refuses to take it down because of "preservation""

END

Additionally, apparently one of the friends of the lead developer of the B3313 ROMhack has lost around $120USD to a fake fundraiser made by a user who has
hijacked the B3313 ROMhack project.

Moving forward, I hope that you shall be able to identify actions indicating an unhealthy community so you can keep others and yourself out of harm's way.
The things people will do to another person using the internet is nasty so hopefully you can be safer and better off by understanding in advance that some
communities are very twisted with allowing horrible things to happen and be promoted like grooming, superiorism, and hatred, and should always be avoided.

## If You Have Read This Far...

If you're part of these crazy groups, hate to say it, but there's nothing you can reasonably do to improve it. I have tried and tried and no matter what I
did or how hard I tried, I have been unable to make an impact towards improving the health of a community. Most recently, I requested a user to censor the
age-innapropriate content (encouraging people to have sex) in their Discord server which was open to minors. I was swiftly banned following this, and when
my friend joined to remind them that this is not legal they banned him too. This is just one example, but now I have finally come to terms after realizing
that I can make a difference towards some things a lot more than others. Ultimately, while I do not have the ability to do anything at all to stop further
grooming, stealing of work, dishonesty, or expressing of hate to others, I do have the ability to make a little bit of progress on an emulator, stay clear
of harmful communities, and work on other things that suit my interest which benefit others. And if you can, I think you should try following in a similar
approach if you want to lead a healthy life. This has worked out a lot better for me recently and I hope this will be the last time I feel the need to say
anything about these kinds of injustices on the internet with my sights set on pursuing more projects which can benefit others. With that, I only have one
request to those who have read the file this far, to resign from it. To always resign from any group which you are confident is harmful online or offline.
And if you insist on being apart of such a type of harmful group, you better be extra careful to not get groomed, stolen from, or become a center of hate.

This may or may not be the best advice, but it's the best advice I have from my circumstances. With that, thanks for reading this far and have a good day.
