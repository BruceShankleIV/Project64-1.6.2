## Download Links

[The download link for the emulator](https://github.com/BruceShankleIV/Project64-1.6.2/releases).

[Download link for ROMhacks which the emulator has entires for in the Game config file, AKA optimized emulation settings and plugin notes](https://github.com/BruceShankleIV/Project64-1.6.2-ROMhacks-Collection).

A collection (possibly temporary) of texture packs is available [here](https://mega.nz/folder/xL5BwaaQ#2yLK6SfwElrnJQtphdqXKw) as well.

## About

This is an updated Project64 v1.6.1 that's semi-maintained for casual play of retro and ROMhack games with minimal system requirements. See User Guide or contact me for info and troubleshooting.

Contact Info -
My email: bruceiv.shankle@gmail.com
Report bugs: discord.gg/cHDxa9vzQM

I'm usually busy so be patient or try to resolve an issue yourself and post any solutions you find.

## Personal Favorite N64 Games From My Childhood

If you'd like to experience the games I grew up playing, here's a list of them and what version you'll want to use:

* The Legend of Zelda - Ocarina of Time (v1.0) (Version 1.0 for no censorship, most useful glitches to play with, and best game config entry)
* The Legend of Zelda - Majora's Mask (U Region for N64, not GC, Jabo's Direct3D8 has the least graphics issues with the pause menu on this version and has improved cheat support)
* Super Mario 64 (U version, has LOD fix and improved cheat support)
* Mario Kart 64 (U version, gets 60FPS support from the Game config entry)

## Project64 1.6.2 Plugin Usage Guidelines

To effectively make use of Project64 1.6.2 with all games, you will need to make use of the plugin system and review the plugin notes. When you open a ROM, if you are unable to play the game or there are bad graphical issues or there's a crash, it's possible that you need to change your plugins. From here, go to the ROM Notes tab. Here you may see a plugin note which tells you a specific plugin you need and/or suggest a specific plugin. If you are unable to open the ROM to access the ROM Notes tab in the settings, you can right click on the ROM in the ROM browser after selecting the directory where the ROM is and view the ROM Notes from there. However, there will be no ROM notes provided if the ROM isn't registered in a game entry from the Game config file (Game.ini).

The success of each plugin is highly PC and game dependent*.

Note: Do not recklessly tamper with the Plugin folder by adding dlls for other plugins, that can very easily result in misbehavior. You shouldn't need to add any more plugins to this application anyways.

## Aknowledgements

Thanks to the efforts between Zilmar & Jabo alongside their crafty team, they delivered the original Project64 which worked with most relevant games including many hacks that came afterwards.

Project64 1.6.1 surfaced in 2011 with a handful of improvements, although with some regression. The source code was then lost and only recovered years later by Jabo. Plugins made for the project were released including Azimer's HLE Audio 0.60 WIP 2 and Jabo's 1.6.1 Video/Sound/Input. Notably, Jabo's 1.6.1 Video managed to be the best option for users seeking enhanced graphics with native widescreen, high resolution, and superampling supported. His video plugin was also consistent, working across every device which made it an adequate default choice for enjoying retro titles. Azimer's audio plugin did not age so well though, but it was obsoleted by his later releases.

Zilmar then started making his own builds under a new team:
https://github.com/project64/project64/commit/f825b21de5e7e3cbc5722275754274eadb5497d0
https://drive.google.com/file/d/1SmM8inepLK9ZmrxTSOsKxBxc9nkO9xRE/view?usp=sharing

Project64 Legacy, an update of Project64 1.6 was revealed years later. Created by the former Project64 team, it has a high focus on accuracy to an unmodified N64, though this often came at the expense of ROMhacks it could support. Unfortunately, it has a lot of issues, so I was still struggling to have an ideal version of Project64 which worked for all ROMhacks, old and new, and could adapt based on the capabilities of my current windows PC with the least problems.

However, after about 3 years of frequent testing and tampering with the Project64 1.6.1 source code, various plugins, and extensive testing, Project64 1.6.2's Golden Master Edition is finally prepared! This is what hope looks like.

I thank Zilmar & Jabo for “Permission to use, copy, modify and distribute Project64 in both binary and source form, for non-commercial purposes”, and for indirectly giving me this opportunity to complete Project64 1.6.

To the former Project64 team, thanks for fixing the TLB ACE vulnerability and uploading a few accuracy improvements which don't compromise ROMhack support via [Project64 Legacy](https://github.com/pj64team/Project64-Legacy).

To Zilmar, thanks for the spirit you put into the original Project64 and uploading a couple fixes via your build (link will not be provided to his project here due to history of malicious software bundling and nagware).

To my friends, thank you for testing the application along the way and providing feedback, reporting issues to solve, and translating to other languages.

To idc5580, your patching tool is wonderful to apply lots of patches to a baseROM to quickly generate lots of ROMhacks. Thanks so much for your efforts!

And thank you, the user, for benefitting from all this work. May you have a good experience with this application and the games you will play.

See Section#8 Credits of the user guide for Former Project64 Team, code, plugin, testers/helpers/translators, database, and tools used credits.