# Flappy 2048 in portable C

Play the addictive game, Flappy 2048 with no browser overhead. 

Just to wanted to expriment with graphical game-making in C and it seemed easy to code. 

the original: http://hczhcz.github.io/flappy-2048
# Dependencies
* Allegro 5
* A TTF font (optional)

# Installation
``` sh
git clone https://github.com/untakenstupidnick/flappy-2048-nobrowser
export PREFIX = /usr/games
make install

flappy-2048 #run the game
```

Either put the provided font at ./clearsans.ttf or set FONT_ADDRESS at config.h to the font address you want before compile-time.

# Caution

The game requires that you keep your eyes on a small portion of the screen for a long time
If you like your eyes, take a break now and then when you play it.
