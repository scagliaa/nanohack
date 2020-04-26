Written by Constantine "NanoCat" Shablya.
I hereby place this project and its code in the public domain.

Special thanks to
Leystryku
styles
Willox
Paradoxxx
kelse
LuaStoned
blue kirby
TheRussianPootis
everybody else i forgot to put here

Check INDEX.txt to find out what files of cheat mean.

Videos:
http://youtu.be/3g1IrPwvosI - first, barebones version
http://youtu.be/izXoK-E2xAo
http://youtu.be/liLeIZF4MiU - css autowall demo
http://youtu.be/xMWdwfPQxsQ - gmod shake nospread demo
http://youtu.be/F03KhcrAilA
http://youtu.be/CFlC4_HcEOY
http://youtu.be/JjUChDjp4pA - hvh against some cheat in gmod
http://youtu.be/vv4O7BLw1uM
-
http://youtu.be/uWcdg2o4tbI
http://youtu.be/hD59_c_7Wag
http://youtu.be/FfWhC7xZS48
http://youtu.be/OWgZo-qmXWs
http://youtu.be/pgYyUgMiRHU
http://youtu.be/gJFNvyrPrwY
http://youtu.be/bCvgOG1y4-E

Hi! You are looking at the README for NanoHack version 2.0/.1 source. This
is evolution of NanoHack since december of 2013 and until december 2015.
A lot of things have been changed since the beginning. Here is a list of
features you probably wouldn't find in many cheats out there (until the
date this was released) and what makes this cheat special:
 Cheat:
  . (!!!) compensational (shake) nospread for GMOD with no shot skipping
    that is, what is commonly done in Lua/C++ is now done exclusively
    in C++, other than that, the cheat doesn't skip one bullet in order
    to get actual spread of the weapon.

  . (!!!) SMAC anti namestealer bypass
    that you probably won't find anywhere. Is done by swapping
    latin letters with similar looking ones from cyrillic alphabet
    and vice versa. Though SMAC's 10 second limit still is a problem.
    Other than that, namestealer doesn't affect your "name" convar and
    also has a special mode to work in DarkRP.

  . chat jammer
    spams tons of newlines in chat and prevents users from communicating
    through it, while filtering this shit out of your chat so you can still
    read it finely.

  . SMAC aimbot detection bypass
    is done by smoothing out angles motion. I use this method because
    my laptop commonly had FPS drops below 60 which would fuck up method
    where you reset your angles to 0. If you have decent PC, you can
    easily bypass SMAC aimbot detector by setting your angles to 0 while
    you don't shoot.

  . SMAC autopistol detection bypass
  . SMAC seed detection bypass
  . SMAC bunnyhop detection bypass

 Other common things and notes:
  . use of so-called "engine prediction"
  . aimbot lookup algorithm is somewhat laggy
  . nicely looking menu thanks to my FORMS lib
  . shitty font used for menu
