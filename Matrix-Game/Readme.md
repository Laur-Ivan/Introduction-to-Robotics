# Space Invaders

## Game Concept

As the name of the game implies, it's the oldie (but goldie) Space Invaders game, with some twists (power-ups, bosnus/hidden levels, highscores etc).

## Gameplay mechanics and features

The game will have the basic elements of the original one - player on the bottom of the 'screen' which shoots enemies located at the top of the screen (of course, who also shoot back).
Player will have lives, score, highscore (saved in EEPROM) and the game will have progression (levels, higher the level -> higher the number of the enemies/faster their shoot rate/etc).

The new elements are(still IN PROGRESS :( ):

### Power-ups 
Enemies will shoot, randomly, various bonuses for the player to pick up, and a few of them are: lives, bonus score, bonus time limit(if the level has a time limit).

### Hidden levels
If the player finishes a level in a short time, the next level will be a secret/bonus one, where he can get a really good bonus score or power-ups.

### Animations
Each time the player beats its highscore, a nice little animation will pop up.

And hopefuly other features.

## Hardware to be used

1. 8x8 LED matrix (bicolor matrix if any Romanian store will bring one into their stock in useful time)
2. LCD display for the menu
3. Joystick
4. MAX7219 Driver
6. 10μF Capacitor
7. 0.1μF Capacitor
8. Breadboard
9. Cables (lots of them)

## How to play:
The first thing you notice when you start up the game is the loading screen with a link to this repo. After 5 seconds, you are taken to an old school Main Menu with the options of Play, Settings (where you can choose the difficulty: easy/hard), and Info, where you have some very small details about the game and its creator (spoilers: It's me).
The gameplay itself is almost the same (you move the player with the joystick's X axis and with the push button you shoot), but with 2 noticeable twists. First, the enemies aren't coming after you anymore, they are smart and wait at the top of the matrix, and secondly, you cannot move while they are shooting, so be careful and play a little bit smart.
Also, the progression is infinite, there isn't any level cap or anything like that.
Have fun! <3


## Link to video demo: https://drive.google.com/file/d/1HRuNuPvmkk_0818p13q-bctouRYEhc6B/view

## Image with the whole assembled hardware: https://drive.google.com/file/d/1HVGYoEvkDc9XcntWm2yH34Y7QUUAoEyU/view
