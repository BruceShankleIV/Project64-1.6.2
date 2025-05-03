I was also going to include my fork of Azimer's Audio plugin with aglab2/LINK config fix. But ultimately,
it was scrapped due to volume being a bit lower than it should be and being the cause of some RSP errors.
However, it is fully functional so you may give it a try. Just don't complain if the game crashes on you!
Okay it turns out it is also unstable and prone to causing crashes like Jabo's DirectSound. DON'T USE IT!
^ Maybe it can be used once we fix audio timing in the emulator though?

RSP.dll scrapped because Mupen64Plus outclasses it, quite simply. Only reason to use RSP.dll is if you're
using it in tandem with the Shankle_Audio, that will be a little more compatible with it than Mupen Plus,
of if you are playing australian/european versions of banjo tooie which don't work at all with Mupen RSP.

Jabo_Direct3D8_1.7.0.53_Feb__9_2007.dll scrapped because it isn't compatible with the 1.6 plugin spec, so
I decided to fork an older version of Direct3D8 1.7 and include that in V1.6.2 instead for texture packs.
But if someone manages to get 1.7.0.53 supported then it should replace the included 1.7.0.47 gfx plugin
after being properly updated so that it works well in Project64 1.6.2 (requires extensive testing).

Additional OpenGL video plugins were ultimately scrapped due to numerous bugs in 'em, mainly crash-prone.