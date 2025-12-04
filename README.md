## Download Links

[The download link for the emulator](https://github.com/BruceShankleIV/Project64-1.6.2/releases).

Get ROMhacks from [here](https://github.com/BruceShankleIV/Project64-1.6.2-ROMhacks-Collection).
< If there is a ROMhack not found in this collection anywhere, let me know so it can be added.

Zelda64 Texture packs available [here](https://mega.nz/folder/hLB2jI5R#t71whplzD7iMbb3m_YfFSA).
< Feel free to message any not found here and I will add if it's not too big in file size.

Super Mario 64 texture available [here](https://mega.nz/folder/xL5BwaaQ#2yLK6SfwElrnJQtphdqXKw).
< Feel free to message any not found here and I will add if it's not too big in file size.

Note: If a texture pack is big in file size, then it needs to be split up into a multi-volume
archive, such as .001 and .002 and so forth.

These links are also in the User Guide header as you will be informed on first boot of the app.
This way, you always have access to these resources with every copy of the emulator you download.

## Project Status

I said it was pretty much done with v11, but I wanted to do a tiny bit more, now v11.1 and probably done!

## What This Project Is About

This is not a complete rewrite of the original Project64 code (which would introduce regressions), but a small evolution with improved ROMhack support, additional plugins for users with high-end devices which can suport these plugins, more settings, removing dead code and useless settings or features, and overall making it better based on my own personal usage. It has been crafted from a custom build called Version 1.6.1, hence this name. Each change has been carefully tested to make sure no issues are introduced. Other developers have been reckless about their changes and compromised game compatibility and app stability in doing so, hence why this exists. If you want to load a retail game or supported hack with a high chance of success and you don't care about hardware limitations, this is the build for you.

## Suggested Games to Play (not including ROMhacks)

The games listed below are expected to work decently with the default settings so you won't have to worry about updating your plugin settings.

* The Legend of Zelda - Ocarina of Time (1.0/1.1/1.2)
* The Legend of Zelda - Majora's Mask (U)
* Super Mario 64
* Mario Kart 64 (runs at 60FPS on U version)
* Kirby 64: The Crystal Shards
* Banjo Kazooie
* Banjo Tooie
* Goldeneye (runs at 60FPS)


## Project64 1.6.2 Plugin Usage Guidelines

To effectively make use of Project64 1.6.2 with all games, you will need to make use of the plugin system and review the plugin notes. When you open a ROM, if you are unable to play the game or there are bad graphical issues or there's a crash, it's possible that you need to change your plugins. From here, go to the ROM Notes tab. Here you may see a plugin note which tells you a specific plugin you need and/or suggest a specific plugin. If you are unable to open the ROM to access the ROM Notes tab in the settings, you can right click on the ROM in the ROM browser after selecting the directory where the ROM is and view the ROM Notes from there. However, there will be no ROM notes provided if the ROM is not logged in the ROM Database file (Game.ini).

The success of each plugin is highly PC and game dependent.

Note: It's ill-advised to add any dll's into the Plugin folder. Doing so will result in unknown consequences.

## What's Next?

A friend of mine is working on cheat codes for Ocarina of Time 1.0 (U) version. Next build will have these if he's done with them by then.


## Aknowledgements, Credits, & Special Thanks (credits/thanks taken from the User Guide)

Thanks to the efforts between Zilmar & Jabo alongside their team, they delivered the original Project64 which worked with most relevant games including many hacks that came afterwards.

Project64 1.6.1 surfaced with a handful of improvements, but many regressions due to lack of testing. The source code was then lost and only recovered years later by Jabo. Plugins made for the project were released including Azimer's HLE Audio 0.60 WIP 2 and Jabo's 1.6.1 Video/Sound/Input. Notably, Jabo's 1.6.1 Video managed to be the best option for users seeking enhanced graphics with native widescreen, high resolution, and superampling supported. His video plugin was also consistent, working across every device which made it a great default choice. Azimer's audio plugin was ahead of Jabo's sound in some ways, but it still wasn't as stable as Jabo's sound due to its faulty AI implementation. Jabo's 1.6.1 input was overall worse than N-Rage's at the time, but did have the advantage of some XInput support (my x360 can be used with it) and would get a Setup mode added later for button mapping convenience.

Zilmar then ruined this wonderful project with malware bundling and adware. Actions like this have long since marked the end of soul in official releases of the project. Example shown below this line.
https://github.com/project64/project64/commit/f825b21de5e7e3cbc5722275754274eadb5497d0

Project64 Legacy, an update of Project64 1.6 is revealed years later. Created by some of the people who delivered the original Project64, it has a high focus on accuracy to an unmodified N64, though this often came at the expense of games it could support. Unfortunately, it too is plagued by various different types of issues (as of 1.6.4) due to making untested changes, while one of these developers was actually just a troll who impersonates themselves as a copyright holder for who knows why. Said developer is Gent, the person behind the cheat codes of Project64, who decided to ignore the licensing of the project and dictate use of the Project64 code. He is not listed as a copyright holder of the code, if he mocks you like a bully would then you just have to laugh at how unreletingly petty he is and hope he moves on from whatever it is he's going through, and hope that he corrects the issues in Project64 Legacy by learning from this project.

Finally, after my efforts for about one year, alongside the testing of numerous friends, we have come to this. Project64 1.6.2, a mostly bug-free experience with support for ROMhacks, texture packs, all relevant plugins included, and working on all devices, with all features confirmed working. No malware, no BS, just a working emulator with high support made by sane people.

I thank Zilmar & Jabo for "Permission to use, copy, modify and distribute Project64 in both binary and source form, for non-commercial purposes", and for giving everyone this opportunity to continue Project64 by expanding its capabilities within this capacity.

To the developers behind Project64 Legacy, thanks for making valuable contributions in some ways which will benefit user experience with this application.

To Zilmar, thanks for the spirit you put into the original Project64 and meaningful improvements.

To my friends, thank you for testing the application along the way and providing feedback, reporting issues to solve, and translating to other languages.

To idc5580, your patching tool is wonderful to apply lots of patches to a baseROM to quickly generate lots of ROMhacks. Thanks so much for your efforts!

And thank you, the user, for benefitting from all this work. May you have a good experience with this application and the games you will play.

Section#8 Credits
--------------------------------------------------------------------------------------------------------
E. Bruce Shankle IV - The guy behind this awesome hacked build of Project64 aimed at high compatibility.
Compatibility = Works on all PC's, takes advantage of what your hardware can do, working with all games.

********************************************************************************************************
Formerly, the Project64 Team (1998-2024):
********************************************************************************************************
Zilmar (Tooie):		Founder. Core, Application programmer 1.0-1.4, 1.5, 1.6, copyright holder

Jabo:			Video, Audio, Input plugin programmer 1.0-1.6.1, copyright holder

Witten:			Application, Core programmer for 1.5-1.6.1, Bug tracking,
				testing, suggestions, end user support 1.0-1.6.1

RadeonUser:	   	Application, Core programmer for 1.6.1, Bug tracking,
				testing, suggestions, end user support 1.0-1.6.1

Gent:			Application, database and web development, Bug tracking,
				testing, suggestions & end user support 1.0-1.6.1

Smiff:			Bug tracking, testing, suggestions 1.0-1.5 rdb and
				end user support 1.0-1.6

Azimer:			Application, Core programmer, Audio plugin
				programmer for 1.4-1.6.1

Icepir8:		Application, Core programmer, Video plugin
				programmer for 1.6.1

Contributors:		Hacktarux, Nekokabu
********************************************************************************************************


////////////////////////////////////////////////////////////////////////////////////////////////////////
Code from other builds
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
Game image tearing fix in Registers.c
authored by Zilmar in...
https://github.com/project64/project64/commit/0e691d2e53ab4fb744a6b1f08d622c8ffb4da21d
https://github.com/project64/project64/commit/c7f895737ab3abc257931c4f9a7d65890a037f02
from oddMLan in...
https://github.com/pj64team/Project64-Legacy/commit/78a8a720074530b9ffff383b3d59a5017bbca167

Screen shaking fix in CPU.c, Registers.c, and Registers.h
authored by Zilmar in...
https://github.com/project64/project64/commit/7345ecd97e131907d2d1ce6de9438dcd60793d6c
https://github.com/project64/project64/commit/7909b1850600bf3bfb2673f10830a73d425693ad
from oddMLan in...
https://github.com/pj64team/Project64-Legacy/commit/371c029d0574f8916a9d149822be89debea3ee2d

PD Accuracy improvements in DMA.c
from Icepir8, authored by Icepir8 in...
https://github.com/pj64team/Project64-Legacy/commit/2e68aa6e1781fb26a2697197a162ad7e5b39f575

CIC Chip improvements in PIF.c and Registers.c
from RadeonUser, authored by RadeonUser in...
https://github.com/pj64team/Project64-Legacy/commit/1b4ae1635e1409e12c2c74e84308a2aff771f7b2

Internal AI Twisted edge Sound fix in CPU.c
from Project64 Legacy, authored by loganmc10 in...
https://github.com/mupen64plus/mupen64plus-core/commit/2170d5e0c9ed3c12e487eb1c5425798712b6dac8

Legacy AiLenChanged in CPU.c
Adapted from EmuAI fix authored by Azimer in...
https://github.com/pj64team/Project64-Legacy/commit/6da131efcc6a61360bc2e7a0828b3725fa168d6a

Recompiler TLB ACE Vulnerability fix in Recompiler CPU.c, Recompiler CPU.h, and Recompiler Ops.c
from hacktarux, authored by hacktarux in...
https://github.com/pj64team/Project64-Legacy/commit/d05e6faeedcbfb721f4bacfc25d22e4781988f50

Removed ROM Browser borders in BruceIV's Project64 Ver. 1.6.2.rc and ROM Browser.c
from oddMLan, authored by oddMLan in...
https://github.com/pj64team/Project64-Legacy/commit/c1eaf72b4558f66278cca05a7f1dd464a89a87c8

96kb .sra for Dezaemon 3D in SRAM.c
from Nekokabu, authored by Nekokabu in...
https://github.com/project64/project64/commit/cd2f3cf173e88c8c1e18108f9f21405c077352e8

N64 PIF Protection Scheme Algorithm by Azimer & Zilmar, adapted into source code by X-Scale
(see N64 CIC-NUS-6105 and 1.6 pif2 file).

If there was anything else used from Legacy not explicitly listed here, it's because there was no way
to determine the author of a change.


////////////////////////////////////////////////////////////////////////////////////////////////////////
aglab2 (Better load times and fixed flickering with CF0, SD space, slowdown boot fix, TLB safety checks)
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
https://github.com/aglab2/Project64-Legacy/commits/main/
https://github.com/pj64team/Project64-1.6-Plus/pull/23


////////////////////////////////////////////////////////////////////////////////////////////////////////
z64me
////////////////////////////////////////////////////////////////////////////////////////////////////////
* Drag and drop ROM opening support
* Fixed direct ROM openings from associated extension


////////////////////////////////////////////////////////////////////////////////////////////////////////
1.6.2 TESTERS/HELPERS/TRANSLATORS
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
woot - Beta Tester.
l0lgamez - Beta Tester.
NekoNeko - Beta Tester.
TrulyNaN - Beta Tester, found GPU specific issues, Brazilian Portuguese translation.
caseytopaz - Beta Tester.
grimmer420 - Beta Tester.
theluigidude2007 - Beta Tester.
idc5580 - Flips Auto Patcher 2.0, allows mass patching of ROMhacks. Beta tester.
Andzura - French translation support, D3D8 fullscreen resolution testing.
lilycatx - Beta Tester, dutch translation support.
nintendosatan - Beta Tester, russian translation support.
Glitchy/glitchydorime - Beta Tester, spanish translation support, Bomberman 64 testing.
idc8580 - Flips en masse patch applying (Alcaro made flips, idc made en masse patch applying).
pomboatomico_1443 - Beta Tester.
maddiedoesgames - Beta Tester, discovered issues with 60FPS hacks and hardware-specific plugin behavior,
Banjo Kazooie Nostalgia ROMhack compatibility testing, Harvest Moon testing.
Miro_Oku - Intel GPU testing.


////////////////////////////////////////////////////////////////////////////////////////////////////////
PLUGIN CREDITS
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
Glide64 - See the about. Some improvements to the default ini and hack support from Bruce IV/me.
GLideN64 - See the about. Some improvements to the default ini from Bruce IV/me. Based on glN64 RC2.
Jabo's Direct3D8 1.6.2 - Jabo for D3D8 1.6.1, 1.6.2 updated app-synced build by Bruce Shankle IV/me.
Jabo's Legacy Direct3D - Same as above, but with d3d8 to d3d9 by crosire.
Project64 Legacy Icepir8's LLE - Icepir8 for improvements to ata4's multithreaded fork of Angrylion RDP.
Rice's Video - By Rice x StrmnNrmn (Rice's video based on StrmnNrmn's code).
Jabo's DirectSound 1.6.2 - Jabo for 1.6.1, 1.6.2 updated app-synced build by Bruce Shankle IV/me.
No Audio plugin - Former Project64 Team.
Shankle's LINK's AziAudio [SEP2023] (1.0) - Azimer, aglab2, and Bruce Shankle IV/me.
Jabo's DirectInput7 1.6.2 - Jabo & Gent, 1.6.2 updated app-synced build by Bruce Shankle IV/me.

Shankle's N-Rage's DirectInput8 1.6.2 -

N-Rage 2.0 Credits:
N-Rage (the author) for the last official V1.83 release.
Rabiddeity (main author of the 2.0 rewrite) for tweaks and additions.
Azimer for help with MemPaks.
MadManMark for adding Transferpaks.
aTomIC, Harlay, NaSeR, Siskoo for translations.
Smiff, Bodie, Cyber, Hotshitu, Gannonboy, Harlay, squall_leonhart, Poobah, Legend for testing.
koolsmoky for message window patch.
kidkat for fixing potential crash and buffer overruns.
KrossX for fixing XInput Rumble and Deadzone.

Legacy N-Rage Changes from N-Rage 2.0 Credits:
Zilmar for customizations and bugfixes.
LuigiBlood for N64 Mouse support.

Shankle's N-Rage's DirectInput8 1.6.2 Changes from Legacy N-Rage Credits:
Bruce for several small changes including improvements for XInput mode and default plugin configuration.

Shankle's Octomino SDL2 Input 0.1.1 (1.0) -
Octomino/clickdevin for original plugin.
wermipls for configuration gui.
Bruce for stripping the logging which was causing a lot of annoying issues and for better configuration.

Project64 Legacy Icepir8's RSP - Icepir8, Angrylion RDP Plus developers.

RSP Emulation Plugin - Jabo & Zilmar, updated app-synced build from BruceIV.

Static Interpreter - by Iconoclast with Special thanks to Jabo, ziggy, and angrylion for RDP test and
		     bpoint, zilmar, and Ville Linde for RSP driver examples.


////////////////////////////////////////////////////////////////////////////////////////////////////////
DATABASE CREDITS
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
Game.ini (ROM Database) - Original by Smiff, updated by Nekokabu + MASA + Jelta, and updated by BruceIV.
SM64 U and OoT 1.0 U/J LOD fixes by theboy181.
FIFA Soccer 64 audio fix by Gent.
Mario Kart 64 (U) 60FPS Hack - theboy181
Original Mario 64 Press L to Fly cheat - Unknown.

Cheat.ini (Cheat Database) - Original by Gent, further modified/tweaked by Bruce IV.

Spanish by Glitchydorime, Dutch by lilycatx, Russian by SatanNintendo, French by Andzura.

gamecontrollerdb - https://github.com/mdqinc/SDL_GameControllerDB


////////////////////////////////////////////////////////////////////////////////////////////////////////
APP ICON CREDITS
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
Stanley Sy (starvingartist)
stan@stanleysy.com
http://stanleysy.deviantart.com
http://www.stanleysy.com
(From the Antiseptic suite)


////////////////////////////////////////////////////////////////////////////////////////////////////////
TOOLS USED
\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
MSYS2 + MINGW32 + GCC + SDL 2.0.12
v120_xp Toolset in VS 2022
v141_xp Toolset in VS 2019
Resource hacker
HxD
Notepad++
ChatGPT
GitHub Copilot
GitHub Desktop
redketchup.io
listdiff.com
LGCY, renamed from d3d8.dll - https://github.com/crosire/d3d8to9/releases/tag/v1.9.2
--------------------------------------------------------------------------------------------------------

Section#9 Special Thanks & Shoutouts
--------------------------------------------------------------------------------------------------------
These are my special thanks and shoutouts to awesome people for their work, assistance, and/or kindness:

z64me - For being supportive and addressing issues during the project's primitive stages when I started.

aglab2 - I'm so glad that you researched and documented about the vulnerability inside of N64 emulators.
	 Thanks to your amazing discovery, hacktarux made a fix to the vulnerability in Project64 V 1.6.
	 Not only that, but your Legacy fork was referenced for Project64 V1.6.2 to get improvements in.
	 Thank you for contributing valuable code which has been used to improve everyone's experiences.

gaudy-twinned0z@icloud.com / clickdevin - Thanks so much for developing the Octomino SDL plugin, and for
					  giving me the tip I needed for building. You really rock dude!

TrulyNan - "1.6 Shankle's Final Cut Gigachad Version will save us all." I love that enthusiasm of yours!

GlitchyDorime - I really appreciate your warm comments about this project. 10/10?!? I love hearing that!
		And you're a very good translator. Thanks for being there every step of the way with me.

lilycatx - You're a very kind person and I especially thank you for making an awesome translation! ♥ :)

kev4cards - Thanks for helping me with my credits. You have given good advice which is much appreciated.

satannintendo - You have had a good attitude ever since we met, and thanks so much for your translation!

Brian W. - For remembering the day I turned 21. Thanks for wishing happy birthday, you're a good friend!

Bruce Shankle III - For helping me learn to use Visual Studio when I first started this project, and for
                    helping me obtain an Uninterruptible Power Supply which has helped out tremendously.
                    Later on, he setup high speed internet to help me manage code and develop Project64.
		    Not just that, but he cleaned up my living space after it became a wasteland because
		    I was focusing so much on this project. He is a great father to me.

Emmanuel Sturdivant - He gave me candy he found at the bottom of a vending machine. A heart of kindness.

l0lgamez - You were a great tester!

MTA - Thanks for checking out the project and finding a bug! :)

TheGent & Smiff - Although both of your work on the original Database support files has been obsolete by
		  now, I can't go without saying how you put a lot of time into it which is respectable.
		  Good job to you two for investing so much into your original works.

Zilmar - You programmed the core in Project64 1.6 from what I can understand. Thanks to your significant
	 work on that, this project can exist and enable so many other game projects to remain playable.
	 Thank you, for allowing this to be possible. And good luck with your builds, even if your users
	 cannot trust you anymore after you back-stabbed them.

Jabo - Thanks for making such an outstanding video plugin with very little drawbacks compared to others.
--------------------------------------------------------------------------------------------------------

## Should Probably Stay Away From Those ROMhackers Though - Arrogance, Content Stealing (and a lot of it), Financial Scamming, Reports of Grooming. Things seem pretty bad out there folks. Stay indoors!

Yeah you saw that title correctly. People behind these fun fangames seem to be involved in the most dirty and messed up things. I encourage you to play the ROMhacks and texture packs
with this program, but at a safe distance away from the people involved in these works if they are a stranger to you. Below I talk about some of this to show you what you're missing.


[Bio - "That one german guy that made the Star Revenge Series and remade vanilla SM64 to be finally a good game. Been hacking SM64 since 2012 and still going at it. If you wanna see me make these hacks, then check out my Twitch streams. Also citified hater of extreme hacks, screw the level design in them. And don't question the story in my SR hacks, it's made up as I went along making them."](https://romhacking.com/user/BroDute)

Above, A user states they have redone Super Mario 64 to be "finally a good game", implying that Mario 64 was never a good game and that their work is "actually good", a case of redoing something not because you liked the original work, but because you didn't like it and wish to devalue it by putting your work above its base. It would be like if Mario 64 DS wasn't made by Nintendo and then these DS developers told everyone that Mario 64 sucked and that they should play Mario 64 DS instead.

[On christmas of 2021, Super Mario Star Road Retooled would release on YouTube](https://www.youtube.com/watch?v=HdMHkK1MPO0). In the video, it appears to be a rerelease
of a ROMhack called Star Road originally created by a user who goes by the alias "Pyro Jay". The video opens with "Pyro Jay Presents" "Rebuilt from the ground up 10
years later..." "Star Road Retooled" And the video showcases lots of levels in this ROMhack rerelease before cutting to "Revisit a timeless classic..." "... with
improved visuals, bug fixes, gameplay features, and more!" "New areas to visit and redesigned stars to collect!" "Star Road Retooled" and the video fades to black, then
says "Hack by Pyro Jay" "Trailer by SpK" "Available Now!" "Link in the description".

But this hack wasn't solely made by Pyro Jay and it doesn't belong to them. This is actually an infringing hackjob of the original made to undermine [the true author's official release](https://www.youtube.com/watch?v=_JBdxcnyxeQ).
Similar to the previous example, it's an attempt to undermine the work it is based off of, and not a very sucessful one judging by the broken graphics with all video plugins and users' bad experiences with the game.

When reaching out to the user, "Christopher Rios Lillo" about the existence of two B3313 ROMhacks with one of them called "unabandoned", here's what he had to say...

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
something dramatically untrue about you and threatened to doxx you?"

"yes, and they have been harrasing me to this day"

Bruce Shankle IV: "in what form is their harassment, verbal over the internet?"

"yes"

Bruce Shankle IV: "okay. so that about sums it up right, in addition to romhacking.com choosing to host the hijacked build of your project similar to star
road retooled."

"yes"

"simpleflips refuses to take it down because of 'preservation'"

END

Another case of people taking advantage of somebody else's contributions because they are so compelled to by their own malice.

Additionally, a user has lost around $120USD to a fake fundraiser made by one of the people involved in stealing this guy's project.


[The author of Project64 3.0-N wrote an article claiming they have surpassed Project64 1.6 with Project64 3.0-N](https://sites.google.com/view/shurislibrary/project64-16)
despite their emulator not working with many games that work in 1.6. They also purpotedly claimed project64 accidentally had malware included in the installer
in it, even though it has been deliberately bundled in previous installers. They also claimed there was "nearly no drawbacks" with Project64 3.0 compared to 1.6
which is another lie, as all the releases of Project64 after 1.6 are plagued with strange regressions which negatively affect the program's effectiveness. They
also lied about plugins, claiming Jabo's Direct3D8 doesn't work on PC's anymore even though it does and also lied about emulation accuracy requirements,
claiming that some ROMhack projects "rely on console accuracy in order to work" which is completely false. Because if that was true, they wouldn't work on Project64,
and they work on Project64. Project64 is and has not ever been, and probably never will be accurate to an N64 at all. Even if it was, it's unlikely any common
computer system would be able to run it due to high requirements. Look at Ares for example. It doesn't even work on my PC, which is already powerful by today's
standards. They lied about Jabo's sound plugin, not even naming it correctly and failing to mention that the crackles heard when using it can be eliminated by
activating the Sync Game to Audio option, which is now integrated directly into the emulator in Project64 1.6.2 so that it can be set in the ROM Database file.
They also lied about Jabo's video plugin, claiming it can be replaced by a buggy and horrible fork of GLideN64 called ANGLE GLideN64, which appears to be a bad
version of GLideN64 which breaks graphics in a very large number of games, many of which display properly in Jabo's Direct3D8 but not in ANGLE GLideN64. This
applies to a lot of retail games and some ROMhacks. They also don't aknowledge framebuffer in a true context of N64 emulation. Most games rely on a framebuffer
and sometimes very heavily. The correct way to handle this is how it is done in Jabo's Direct3D8, on a per-game/entry system. Since Jabo's Direct3D8's source
code is not available, I will briefly explain what I have observed from it based on extensive testing and viewing it inside of a hexadecimal editor.

Jabo's Direct3D8 checks a ROM's header data including its Internal name like "THE LEGEND OF ZELDA" for Ocarina of Time or "SUPER MARIO 64" for mario 64, as well
as it's CRC. GLideN64 works exactly the same way with one of it's ini files applying settings on a per-name basis. If a certain name or CRC is detected, Jabo's
video plugin will emulate framebuffer graphical effects and more. It's method also doesn't induce any input lag or screen tearing, those issues are related to
GLideN64's implementation of framebuffer emulation being completely different from Jabo's Direct3D8, for better and for worse. This is the correct way to emulate
important effects which can compromise an experience on a different game, on a per-game basis. This is why framebuffer emulation is supposed to be on by default,
and turned off on a per-game basis for games which don't benefit from it to get the most overall effectiveness out of the program, as game graphics won't work at
all if you emulate with the framebuffer off. Therefore, it's best to have it on if it's not known if a game works with it off and emulate with it off if you can.

In addition to this, I mentioned the ACE vulnerability which has been being used as a scare tactic has been patched right after it was found and to distirbute
the patch for users that benefit from Project64 1.6. Instead, they continue to lie to people and omit this critical detail because that wouldn't help them control
people's decisions through fear and to lure them into using less effective and regressive programs. Meanwhile, users on the internet behave like pedophiles by
enabling statutory rapists and manipulators to do what they do best: abusing lonely kids and burying the evidence.

In Early march of 2021, a 13 year old boy was talking to his peers about the special relationship he had been involved in.
The groomer said to [convince people that they were joking about the sexual image they received from them:](https://drive.google.com/file/d/147QjkVFJsEXtx5UWsJkmXZH-lHy2V6C9/view?usp=sharing)

START

Groomer: "Tell them you were joking"

Groomer: "tell them f***ing anything to convince them"

END

After this, the groomer claimed the 13 year old wanted this and that they're also a minor so that
makes these sexual acts acceptable all while scolding the boy for making people aware about this.
Since the groomer built an attachment with the kid, this made the kid very upset and wanting to feel better [as described by somebody involved with him](https://drive.google.com/file/d/1gdytSj2K68Xw4UHdlrAGTNi_ipEMKJlx/view?usp=sharing).

[Chat](https://drive.google.com/file/d/1DmIOdP4g5G7t4DjdFL72wVvJ6XXQGuSB/view?usp=sharing)

START

Groomer: "Dont do that again"

Groomer: "Thats such a sh**ty thing to do"

Groomer: "Im not saying you are a sh**ty person but you did something really f***ed up"

END

START

The 13 year old: "I promise from now on I'll keep our stuff private"

Groomer: "Dont do it again though"

Groomer: "thanks"

END

[The groomer shifted their tone once the boy promised to preserve the secrecy of their sexual relationship](https://drive.google.com/file/d/1_krQui6ILfdEOsb6ct6ifK0kkuInCcgs/view?usp=sharing).

"Those who commit child sexual abuse commonly use threats or guilt to maintain power over the child. The
offender wants the child to keep the abuse a secret and ensure the child's continued participation. The offender
will engage in emotional manipulation: "If you tell your dad about this, he is going to hate you;" "If you tell, I
won't be able to give you gifts anymore;" "If you tell, your mom is going to get hurt." The offender may
normalize the inappropriate behavior with the child, or even blame the child for letting the abuse happen."

After this, the manager of the so-called ROMhacking community who calls himself SimpleFlips tried to downplay all this to me as "Minor's Romance" and disregarded
this child sexual abuse coming from one of the members and server moderators of his so-called "community". Despite knowing [this cognitively impaired 13
year old boy stated their mental health is "rapidly declining" to the groomer](https://drive.google.com/file/d/1B9XmsareqAFi5Lad_rgjYC6IFqX4IDvG/view?usp=sharing)
and the groomer stated there's "a pretty big gap" in their age which can used for "Taking advantage" and they stated the 13 year old was below the age of consent.



This Kind of Screwed Crap is NOT Just Limited to One Place on the Internet Either:

From my own interactions with multiple N64 Content creators/people who record videos of ROMhacks, they don't impose any restrictions on innapropriate content
being posted by perverts and see no reason to. Seohaine from OverKart64 uploads other people's content, claims it as his own, then monetizes it and impersonates
himself as the original creator of the content. CDi-Fails in Hylian Modding hijacked what was supposed to be a fair competition to see who can make the best
ROMhack, disqualifying the best ROMhack and denying the makers the $500 USD cash reward he promised.

[Seohaine](https://www.youtube.com/post/UgkxCFRZmG3RAwJJX0HiPEM1G0K-hn1dwy62)

[OverKart64 ROMhacking server owner plays dumb after being told what Seohaine has been up to and lets Seohaine get away with scamming the viewers of his videos, I would assume they are being paid somehow](https://drive.google.com/file/d/1HJ7h5Xy20f_jgepbXvYPUd4IFzmDFYgH/view?usp=sharing)

[CDi-Fails hijacks monetary-rewards ROMhacking competition, lies about submissions on the HM website](https://drive.google.com/file/d/18h2PhHUB7PbkNaP5omi8jj7qmdkVI7Ms/view?usp=sharing)

[Zelda Source Code Decompilation effort is completely corrupt and Fig02 lies about a contributor's intentions while being negligent to deal with pirates like Saurean who pirated leaked code and referenced it for their work](https://drive.google.com/file/d/1C8OjxAEJ4oees6GsZanxBwJ1imqpByZL/view)

[The author of Project64 3.0-N finds Hitler's mass genocide respectable. Not as a joke, but seriously. "Technically Hitler would've solved overpopulation" "I don't like what he did" "But there's ultimately no way to explain why what he did was objectively bad"](https://drive.google.com/file/d/1xIDOLn7r58ppRXfB_2n-b7S1IbD3h3Bc/view?usp=sharing)

Yeah, this is crazy. Just stay away from this and you will avoid so much trouble.
