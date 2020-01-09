# Space Invaders

## Game Concept

As the name of the game implies, it's the oldie (but goldie) Space Invaders game, but with a couple of power-ups and infinite levels.

## Gameplay mechanics and features

The game will have the basic elements of the original one - player on the bottom of the 'screen' which shoots enemies located at the top of the screen (of course, who also shoot back).
Player has lives, score, highscore (saved in EEPROM) and the game has progression (levels, higher the level -> higher the number of the enemies/faster their shoot rate/etc).

### Power-ups 

Every few enemies killed(6 at the moment), a power up spawns on a free column(a column where there are no enemies) represented by a blinking led. The player has to shoot it order to obtain the bonus. The power ups are: bonus score(10) and bonus lives(1).

## How to play:

The first thing you notice when you start up the game is the loading screen with a link to this repo. After 5 seconds, you are taken to an old school Main Menu with the options of Play, Settings (where you can choose the difficulty: easy/hard and reset the current highscore), and Info, where you have some very small details about the game and its creator (spoilers: It's me).
The gameplay itself is almost the same (you move the player with the joystick's X axis and with the push button you shoot), but with a couple of changes. First, the enemies aren't coming after you anymore, they are smart and wait at the top of the matrix. Secondly, there are power-ups which I've talked about above.
Also, the progression is infinite, there isn't any level cap or anything like that.

## Hardware to be used

1. 8x8 LED matrix 
2. LCD display for the menu
3. Joystick
4. MAX7219 Driver
6. 10μF Capacitor
7. 0.1μF Capacitor
8. Breadboard
9. Cables (lots of them)

Have fun! <3


## Link to video demo: https://drive.google.com/file/d/1HRuNuPvmkk_0818p13q-bctouRYEhc6B/view

## Image with the whole assembled hardware: https://drive.google.com/file/d/1HVGYoEvkDc9XcntWm2yH34Y7QUUAoEyU/view
