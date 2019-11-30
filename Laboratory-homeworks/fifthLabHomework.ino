#include <LiquidCrystal.h>

const int RS = 2;
const int enable = 3;
const int d4 = 4;
const int d5 = 5;
const int d6 = 6;
const int d7 = 7;
//const int backLightPin = 9;

LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);

//Joystick pins
const int pinSW = 8; // digital pin connected to switch output

const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

//joystick movement variables
int xAxisValue = 0;
int yAxisValue = 0;

int minThreshold = 400;
int maxThreshold = 600;

bool buttonState = false;
bool joyMovedX = false;
bool joyMovedY = false;
bool lastButtonState = false;

int lastDebounce = 0;
int debounceInterval = 10;
int joySwValue = 0;

enum menus {MAIN, GAME, OPTIONS};

menus currentMenu;

int currentScore = 0;
int highscore = 0;

const int lives = 3;

int startingLevel = 1;
int currentLevel = startingLevel;

const int maxStartingLevel = 9;

const int timeAfterWhichToIncreaseLevelAndUpdateScore = 2000;
int timeElapsedAfterLevelAndScoreIncrease = 0;

const int scoreMultiplier = 2;

const int gameTimer = 10000;
int currentGameTimer = 0;

byte currentMenuCursorPosition = 0;

bool shownGameOver = false;

void setup() {
  initializeLcd();

  initializePins();

  activateMenuScreen();

  resetCursorPosition(1);

  Serial.begin(9600);
}

void loop() {
  switch (currentMenu) {
    case MAIN:
      handleMainMenu();
      break;
    case GAME:
      handleGameMenu();
      break;
    case OPTIONS:
      handleOptionsMenu();
      break;
  }

  checkButtonPressed();
}

void initializeLcd() {
  lcd.begin(16, 2);
  lcd.setCursor(0, 0);
}

void initializePins() {
  pinMode(pinSW, INPUT_PULLUP);
}

void resetCursorPosition(int cursorStartingRow) {
  lcd.setCursor(0, cursorStartingRow);
  lcd.print(">");

  currentMenuCursorPosition = 0;
}

void activateMenuScreen() {
  lcd.clear();

  resetCursorPosition(1);

  lcd.setCursor(4, 0);
  lcd.print("MainMenu"); //due to symmetry reasons I didn't leave a space between 'Main' and 'Menu'

  lcd.setCursor(1, 1);
  lcd.print("Start");

  lcd.setCursor(8, 1);
  lcd.print("Options");

  currentMenu = MAIN;
}

void activateGameScreen() {
  resetGameVariables();

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print("Score:" + String(currentScore));

  lcd.setCursor(0, 1);
  lcd.print("Level:" + String(currentLevel));

  lcd.setCursor(9, 0);
  lcd.print("Lives:" + String(lives));

  currentMenu = GAME;
}

void activateOptionsScreen() {
  lcd.clear();

  resetCursorPosition(0);

  lcd.setCursor(1, 0);
  lcd.print("Level:" + String(startingLevel));

  lcd.setCursor(12, 0);
  lcd.print("Back");

  lcd.setCursor(0,1);
  lcd.print("Highscore:" + String(highscore));

  currentMenu = OPTIONS;
}

void handleMainMenu() {
  xAxisValue = analogRead(A0);

  handleMoveLeft();
  handleMoveRight();
  handleCenter();
}

void handleGameMenu() {
  if (millis() - currentGameTimer > gameTimer && !shownGameOver) {
    gameOver();
    shownGameOver = true;
  }
  else {
    if (millis() - timeElapsedAfterLevelAndScoreIncrease > timeAfterWhichToIncreaseLevelAndUpdateScore && !shownGameOver) {
      currentLevel++;
      currentScore += currentLevel * scoreMultiplier;

      updateScoreAndLevelAfterTimeElapse();

      timeElapsedAfterLevelAndScoreIncrease = millis();
    }
  }
}

void handleOptionsMenu() {
  xAxisValue = analogRead(A0);
  yAxisValue = analogRead(A1);

  handleMoveLeft();
  handleMoveRight();
  handleCenter();

  handleMoveUp();
  handleMoveDown();
}

void handleMoveLeft() {
  if (xAxisValue < minThreshold && joyMovedX == false) {
    if (currentMenuCursorPosition) {//cursor is on the option on the right, we move it to the left
      currentMenuCursorPosition = 0;
    }
    else {
      currentMenuCursorPosition = 1;
    }
    activateOptionCursor();
    joyMovedX = true;
  }
}

void handleMoveRight() {
  if (xAxisValue > maxThreshold && joyMovedX == false) {
    if (currentMenuCursorPosition) {//cursor is on the option on the right, we move it to the left
      currentMenuCursorPosition = 0;
    }
    else {
      currentMenuCursorPosition = 1;
    }
    activateOptionCursor();
    joyMovedX = true;
  }
}

void handleMoveDown() {
  if (yAxisValue < minThreshold && joyMovedY == false && !currentMenuCursorPosition) {
    if (startingLevel > 1) {
      startingLevel--;
    }
    else {
      startingLevel = maxStartingLevel;
    }
    joyMovedY = true;
  }

  updateStartingLevelLcd();
}

void handleMoveUp() {
  if (yAxisValue > maxThreshold && joyMovedY == false && !currentMenuCursorPosition) {
    if (startingLevel < maxStartingLevel) {
      startingLevel++;
    }
    else {
      startingLevel = 1;
    }
    joyMovedY = true;
  }

  updateStartingLevelLcd();
}

void handleCenter() {
  if (xAxisValue >= minThreshold && xAxisValue <= maxThreshold) {
    joyMovedX = false;
  }

  if (yAxisValue >= minThreshold && yAxisValue <= maxThreshold) {
    joyMovedY = false;
  }
}

void activateOptionCursor()
{
  if (currentMenu == MAIN) {
    if (currentMenuCursorPosition == 0) { //if the cursor which we want to activate will be on the right option
      //clear previous cursor
      lcd.setCursor(7, 1);
      lcd.print(" ");

      lcd.setCursor(0, 1);
      lcd.print(">");
    }
    else {
      //clear previous cursor
      lcd.setCursor(0, 1);
      lcd.print(" ");

      lcd.setCursor(7, 1);
      lcd.print(">");
    }
  }
  else {
    if (currentMenuCursorPosition == 0) {
      //clear previous cursor
      lcd.setCursor(11, 0);
      lcd.print(" ");

      lcd.setCursor(0, 0);
      lcd.print(">");
    }
    else {
      //clear previous cursor
      lcd.setCursor(0, 0);
      lcd.print(" ");

      lcd.setCursor(11, 0);
      lcd.print(">");
    }
  }
}

void checkButtonPressed() {
  joySwValue = !digitalRead(pinSW);

  if (joySwValue != lastButtonState) {
    lastDebounce = millis();
  }

  if ((millis() - lastDebounce) > debounceInterval) {
    if (joySwValue != buttonState) {
      buttonState = joySwValue;

      if (buttonState == HIGH) {
        if (currentMenu == MAIN) {
          if (currentMenuCursorPosition == 0) {
            activateGameScreen();
          }
          else {
            activateOptionsScreen();
          }
        }
        if (currentMenu == GAME) {
          if (currentMenuCursorPosition == 0) {
            activateMenuScreen();
          }
          else {

          }
        }
        if (currentMenu == OPTIONS) {
          if (currentMenuCursorPosition == 0) {

          }
          else {
            activateMenuScreen();
          }
        }
      }
    }
  }

  lastButtonState = joySwValue;
}

void updateScoreAndLevelAfterTimeElapse() {
  lcd.setCursor(6, 0);
  lcd.print(currentScore);

  lcd.setCursor(6, 1);
  lcd.print(currentLevel);
}

void gameOver() {
  lcd.clear();
  lcd.setCursor(4, 0);

  lcd.print("Game Over!");

  lcd.setCursor(6, 1);
  lcd.print(">");

  lcd.setCursor(7, 1);
  lcd.print("Exit");

  currentMenuCursorPosition = 0;

  checkHighscore();
}

void resetGameVariables() {
  currentGameTimer = millis();
  timeElapsedAfterLevelAndScoreIncrease = millis();

  currentMenuCursorPosition = 1;

  shownGameOver = false;

  currentLevel = startingLevel;

  currentScore = 0;
}

void updateStartingLevelLcd() {
  lcd.setCursor(7, 0);
  lcd.print(startingLevel);
}

void checkHighscore(){
  if(currentScore > highscore){
    highscore = currentScore;
  }
}
