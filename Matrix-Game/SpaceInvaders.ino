#include <SPI.h>
#include <TFT.h>
#include "LedControl.h"

//TFT screen pins
const int CS = 10;
const int DC = 9;
const int RESET = 8;

//MAX7219 driver pins
const int DIN = 7;
const int CLK = 6;
const int LOAD = 5;
const int numberOfDrivers = 1;

//button pin
const int buttonPin = 2;

//JOYSTICK pins
const int joyXPin = A0;
const int joyYPin = A1;

//read joystick values
int joyXValue = 0;
int joyYValue = 0;

//joystick movement control values
int joyMovedX = 0;
int joyMovedY = 0;

int minThreshold = 400;
int maxThreshold = 600;

int counter = 0;
char printout[4];

TFT myScreen = TFT(CS, DC, RESET);

LedControl lc(DIN, CLK, LOAD, numberOfDrivers);

int currentMenuCursorPosition = 0;

enum menus {MAIN, PLAY, OPTIONS, INFO, GAME_OVER, LEVEL_FINISHED};

menus currentMenu;

//DEBOUNCE
int lastButtonState = 0;
int lastDebounce = 0;
int debounceInterval = 10;
int buttonValue = 0;
int buttonState = 0;

int cursorMenuPositions[3] = {20, 50, 80};
int cursorGamePositions[3] = {60, 90, 80};

unsigned short int currentYPos = 6;

unsigned short int currentXPos = 0;
unsigned short int previousXPos = 0;

const int matrixSize = 8;
const int bulletSpeed = 60;
const int numberOfBulletsAllowed = 5;
unsigned short int bulletsUsed = 0;

struct bullet {
  bool isActive;

  short int bulletXPosition;
  short int bulletYPosition;
};

bullet myBullets[numberOfBulletsAllowed];

unsigned long int lastBulletUpdate;

short int numberOfRows = 0;
short int minNumberOfRows = 1;
short int maxNumberOfRows = 4;

short int numberOfEnemiesOnEachRow = 0;
short int minNumberOfEnemiesOnEachRow = 1;
short int maxNumberOfEnemiesOnEachRow = 4;

const int enemiesMatrixSize = 4;
byte enemiesMatrix[enemiesMatrixSize][matrixSize];

short int initialTimeBetweenEnemyBullets;
short int initialEnemyBulletSpeed;
short int timeBetweenEnemyBullets = 1000;
short int enemyBulletSpeed = 150;

unsigned long int lastEnemyBulletSpeedUpdate = 0;
unsigned long int lastEnemyBulletUpdate = 0;

short int initialMinNumberOfRows;
short int initialMinNumberOfEnemiesOnEachRow;
short int initialMaxNumberOfEnemiesOnEachRow;

const int loadingScreenTime = 5000;

bool loadingScreenFinished = false;
bool showLoadingScreen = true;

int loadingScreenStartTime;

struct enemy {
  bool isActive;

  short int enemyXPosition;
  short int enemyYPosition;
};

enemy myEnemies[32];

short int currentNumberOfEnemies = 0;

bool difficultyIsHard = false;

int score = 0;
char scoreText[3];

const int lives = 3;
short int currentLives = lives;
char livesText[3];

short int level = 0;
char levelText[3];

void setup() {
  initializeTFT();
  initializeLC();
  initializeJoystick();

  pinMode(buttonPin, INPUT_PULLUP);

  //activateMainMenu();

  backupVariables();

  loadingScreenStartTime = millis();

  randomSeed(analogRead(A3));
}

void loop() {
  activateLoadingScreen();
  switch (currentMenu) {
    case MAIN:
      handleMainMenu();
      break;
    case PLAY:
      handlePlayMenu();
      handleGameplay();
      break;
    case OPTIONS:
      handleOptionsMenu();
      break;
    case INFO:
      handleInfoMenu();
      break;
    case GAME_OVER:
      handleGameover();
      break;
    case LEVEL_FINISHED:
      handleLevelFinished();
      break;
  }
  checkButtonPressed();
}


void initializeTFT() {
  myScreen.begin();
  myScreen.background(0, 0, 0);
  myScreen.stroke(255, 255, 255);
  myScreen.setTextSize(2);
}

void initializeLC() {
  lc.shutdown(0, false);
  lc.setIntensity(0, 1);
  lc.clearDisplay(0);
}

void initializeJoystick() {
  pinMode(joyXPin, INPUT);
  pinMode(joyYPin, INPUT);
}

void handleMoveLeft() {
  if (joyXValue > maxThreshold && joyMovedX == false && currentMenuCursorPosition == 0) {
    if (difficultyIsHard) {
      myScreen.setTextSize(1);

      myScreen.stroke(0, 0, 0);
      myScreen.text("Difficulty <Hard>", 25, 20);

      myScreen.stroke(255, 255, 255);
      myScreen.text("Difficulty <Easy>", 25, 20);

      difficultyIsHard = false;
    }
    else {
      myScreen.stroke(0, 0, 0);
      myScreen.text("Difficulty <Easy>", 25, 20);

      myScreen.stroke(255, 255, 255);
      myScreen.text("Difficulty <Hard>", 25, 20);

      difficultyIsHard = true;
    }

    joyMovedX = true;
  }
}

void activateLoadingScreen() {
  if (showLoadingScreen) {
    setupLoadingScreen();
    showLoadingScreen = false;
  }
  if (millis() - loadingScreenStartTime > loadingScreenTime && !loadingScreenFinished)
  {
    activateMainMenu();
    loadingScreenFinished = true;
  }
}

void setupLoadingScreen() {
  myScreen.textSize(2);
  myScreen.text("Repo at", 0, 0);

  myScreen.textSize(1);
  myScreen.text("https://bit.ly/2S7glpp", 0, 30);
}

void handleMoveRight() {
  if (joyXValue < minThreshold && joyMovedX == false && currentMenuCursorPosition == 0) {
    if (difficultyIsHard) {
      myScreen.setTextSize(1);

      myScreen.stroke(0, 0, 0);
      myScreen.text("Difficulty <Hard>", 25, 20);

      myScreen.stroke(255, 255, 255);
      myScreen.text("Difficulty <Easy>", 25, 20);

      difficultyIsHard = false;
    }
    else {
      myScreen.stroke(0, 0, 0);
      myScreen.text("Difficulty <Easy>", 25, 20);

      myScreen.stroke(255, 255, 255);
      myScreen.text("Difficulty <Hard>", 25, 20);

      difficultyIsHard = true;
    }

    joyMovedX = true;
  }
}

void handleMoveDown(int numberOfElementsOnMenu) {
  if (valueIsLowerThanThreshold(joyYValue, minThreshold) && valueIsFalse(joyMovedY)) {
    currentMenuCursorPosition++;
    if (currentMenuCursorPosition >= numberOfElementsOnMenu) {
      currentMenuCursorPosition = 0;
    }
    if (currentMenu == MAIN || currentMenu == OPTIONS) {
      moveCursorToNextPosition(numberOfElementsOnMenu, 1);
    }
    else {
      moveCursorToNextPosition(numberOfElementsOnMenu, 0);
    }
    joyMovedY = 1;
  }
}

void handleMoveUp(int numberOfElementsOnMenu) {
  if (valueIsHigherThanThreshold(joyYValue, maxThreshold) && valueIsFalse(joyMovedY)) {
    currentMenuCursorPosition--;
    if (currentMenuCursorPosition < 0) {
      currentMenuCursorPosition = numberOfElementsOnMenu - 1;
    }
    if (currentMenu == MAIN || currentMenu == OPTIONS) {
      moveCursorToNextPosition(numberOfElementsOnMenu, 1);
    }
    else {
      moveCursorToNextPosition(numberOfElementsOnMenu, 0);
    }
    joyMovedY = 1;
  }
}

void handleGameplay() {
  joyXValue = analogRead(joyXPin);

  handleMovePlayerLeft();
  handleMovePlayerRight();

  handleCenter();

  spawnEnemyBullets();
}

//yPos -> row
//xPos -> column

void handleMovePlayerLeft() {
  if (joyXValue > maxThreshold && joyMovedX == false) {
    previousXPos = currentXPos;
    currentXPos = (previousXPos + matrixSize - 1) % matrixSize;
    lc.setLed(0, currentYPos, currentXPos, true);
    lc.setLed(0, currentYPos + 1, currentXPos, true);
    lc.setLed(0, currentYPos, previousXPos, false);
    lc.setLed(0, currentYPos + 1, previousXPos, false);

    joyMovedX = true;
  }
}
void handleMovePlayerRight() {
  if (joyXValue < minThreshold && joyMovedX == false) {
    previousXPos = currentXPos;
    currentXPos = (previousXPos + 1) % matrixSize;
    lc.setLed(0, currentYPos, currentXPos, true);
    lc.setLed(0, currentYPos + 1, currentXPos, true);
    lc.setLed(0, currentYPos, previousXPos, false);
    lc.setLed(0, currentYPos + 1, previousXPos, false);

    joyMovedX = true;
  }
}

void initializeGameplay() {
  spawnPlayer();
  spawnEnemies();
}

void spawnPlayer() {
  currentXPos = random(8);

  lc.setLed(0, currentYPos, currentXPos, true);
  lc.setLed(0, currentYPos + 1, currentXPos, true);
}

void handleCenter() {
  if (joyXValue >= minThreshold && joyXValue <= maxThreshold) {
    joyMovedX = false;
  }

  if (joyYValue >= minThreshold && joyYValue <= maxThreshold) {
    joyMovedY = 0;
  }
}

void checkButtonPressed() {
  buttonValue = !digitalRead(buttonPin);

  if (buttonValue != lastButtonState) {
    lastDebounce = millis();
  }

  if ((millis() - lastDebounce) > debounceInterval) {
    if (buttonValue != buttonState) {
      buttonState = buttonValue;
      if (buttonState == HIGH) {
        handleCurrentMenu();
      }
    }
  }

  lastButtonState = buttonValue;
}

void handleCurrentMenu() {
  switch (currentMenu) {
    case MAIN:
      handleMainMenuCurrentCursorPosition();
      break;
    case PLAY:
      playerShoot();
      break;
    case OPTIONS:
      handleOptionsCurrentCursorPosition();
      break;
    case INFO:
      activateMainMenu();
      break;
    case GAME_OVER:
      handleGameOverCurrentCursorPosition();
      break;
    case LEVEL_FINISHED:
      handleLevelFinishedCurrentCursorPosition();
      break;
  }
}

void spawnEnemies() {
  numberOfRows = random(minNumberOfRows, maxNumberOfRows);
  for (int i = 0; i < numberOfRows; i++) {
    numberOfEnemiesOnEachRow = random(minNumberOfEnemiesOnEachRow, maxNumberOfEnemiesOnEachRow);
    for (int j = 0; j < numberOfEnemiesOnEachRow; j++) {
      lc.setLed(0, i, j, true);
      enemiesMatrix[i][j] = 1;

      //      row 0:    m[0][0] = 0
      //                m[0][1] = 1;
      //                m[0][2] = 2 => for row 1, i use j
      //                .
      //                .
      //                .
      //                m[0][7] = 7;
      //
      //      row 1:   m[1][0] = 8 means j+i+7 => j+i+SIZE
      //                m[1][1] = 9; means j+i+7
      //                m[1][2] = 10 means j+i+7
      //                .
      //                .
      //                .
      //                m[1][7] = 1 + 7+ 7= 15

      //      row 2:    m[2][0] = 16 is j+i+14
      //                m[2][1] = 17 is j+i+14
      //                m[2][2] = 18 is j+i+14

      // so for row 0 i can use j+i+(SIZE-1)*i, for 1 i can use j+i+(SIZE-1)*i, same for (j+i)+(SIZE-1)*i

      int indexToPosition = (j + i) + (matrixSize - 1) * i; //transforms the col and row positions into vectory index pos
      myEnemies[indexToPosition].isActive = true;
      myEnemies[indexToPosition].enemyXPosition = i + 1;//enemy bullet position
      myEnemies[indexToPosition].enemyYPosition = j;

      currentNumberOfEnemies++;
    }
  }
}

void spawnEnemyBullets() {
  //lastEnemyBulletUpdate = millis();

  if ((millis() - lastEnemyBulletUpdate) > timeBetweenEnemyBullets) {
    short int randomIndex = random(4 * numberOfRows);

    while (myEnemies[randomIndex].isActive == false) {
      randomIndex = random(4 * numberOfRows);
    }

    //TO DO
    //short int enemyBelowIndex = (myEnemies[randomIndex].enemyXPosition + myEnemies[randomIndex].enemyYPosition) + (matrixSize - 1) * myEnemies[randomIndex].enemyXPosition;
    short int enemyBelowIndex = randomIndex + matrixSize;
    if (myEnemies[enemyBelowIndex].isActive) {
      myEnemies[randomIndex].enemyXPosition += 1;
    }

    //without ^^^^^, the invaders which have in front of them another invaders will turn them off while shooting

    shootEnemyEgg(myEnemies[randomIndex].enemyYPosition, myEnemies[randomIndex].enemyXPosition);

    lastEnemyBulletUpdate = millis();
  }
}

void shootEnemyEgg(int xPosition, int yPosition) {
  //lastEnemyBulletSpeedUpdate = millis();
  while (yPosition <= 7) {
    if ((millis() - lastEnemyBulletSpeedUpdate) > enemyBulletSpeed) {

      lc.setLed(0, yPosition, xPosition, false);
      yPosition++;
      lc.setLed(0, yPosition, xPosition, true);

      if (yPosition == (currentYPos + 1) && xPosition == currentXPos) {
        updateLives();

        if (currentLives == 0) {
          activateGameOver();
        }
      }
      lastEnemyBulletSpeedUpdate = millis();
    }
  }

  lc.setLed(0, 7, xPosition, false);
}

void handleGameover() {
  joyYValue = analogRead(joyYPin);

  handleCenter();

  handleMoveUp(2);
  handleMoveDown(2);
}

void activateGameOver() {
  currentMenu = GAME_OVER;

  currentMenuCursorPosition = 0;

  myScreen.background(0, 0, 0);

  myScreen.setTextSize(2);
  myScreen.text("Game Over!", 35, 15);
  myScreen.text("Play Again", 35, 60);
  myScreen.text("Quit", 35, 90);

  myScreen.text("->", 0, 60);
}

void handleLevelFinished() {
  joyYValue = analogRead(joyYPin);

  handleCenter();

  handleMoveUp(2);
  handleMoveDown(2);
}


void handleGameOverCurrentCursorPosition() {
  switch (currentMenuCursorPosition) {
    case 0:
      //start again
      playAgain();
      break;
    case 1:
      //quit to main menu;
      lightOffMatrix();
      activateMainMenu();
      break;
  }
}

void handleLevelFinishedCurrentCursorPosition() {
  switch (currentMenuCursorPosition) {
    case 0:
      //continue to next level
      playNextLevel();
      break;
    case 1:
      //quit to main menu;
      activateMainMenu();
      break;
  }
}

void handleMainMenuCurrentCursorPosition() {
  switch (currentMenuCursorPosition) {
    case 0:
      level = 0;
      score = 0;
      resetLevelVariables();
      clearMatrixesAndValues();
      minNumberOfRows = initialMinNumberOfRows;
      minNumberOfEnemiesOnEachRow = initialMaxNumberOfEnemiesOnEachRow;
      maxNumberOfEnemiesOnEachRow = initialMaxNumberOfEnemiesOnEachRow;
      timeBetweenEnemyBullets = initialTimeBetweenEnemyBullets;
      enemyBulletSpeed = initialEnemyBulletSpeed;
      activatePlayMenu();
      initializeGameplay();
      break;
    case 1:
      activateOptionsMenu();
      break;
    case 2:
      activateInfoMenu();
      break;
  }
}

void handleOptionsCurrentCursorPosition() {
  switch (currentMenuCursorPosition) {
    case 0:
      //nothing to do here(level difficulty chooser)
      break;
    case 1:
      //TODO reset highscore
      break;
    case 2:
      activateMainMenu();
      break;
  }
}

void lightOnMatrix() {
  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      lc.setLed(0, col, row, true);
      delay(50);
    }
  }
}

void lightOffMatrix() {
  for (int row = 0; row < 8; row++)
  {
    for (int col = 0; col < 8; col++)
    {
      lc.setLed(0, col, row, false);
    }
  }
}

void moveCursorToNextPosition(int numberOfElementsOnMenu, int isNotPlayMenu) { //isNotPlayMenu is true if it is a menu type(main menu/options menu/ game menu), 0 if it is a game menu
  if (valueIsTrue(isNotPlayMenu)) {
    ClearAllMenuCursors(numberOfElementsOnMenu);
    SetCursorAtCurrentMenuPosition();
  }
  else {
    ClearAllGameCursors(numberOfElementsOnMenu);
    SetCursorAtCurrentGamePosition();
  }
}

void SetCursorAtCurrentMenuPosition() {
  myScreen.stroke(255, 255, 255);
  myScreen.text("->", 0, cursorMenuPositions[currentMenuCursorPosition]);
}

void SetCursorAtCurrentGamePosition() {
  myScreen.stroke(255, 255, 255);
  myScreen.text("->", 0, cursorGamePositions[currentMenuCursorPosition]);
}

void clearCursorAtPosition(int positionToClearTextAt) {
  myScreen.stroke(0, 0, 0);
  myScreen.text("->", 0, positionToClearTextAt);
}

void ClearAllMenuCursors(int numberOfElementsOnMenu) {
  for (int i = 0; i < numberOfElementsOnMenu; i++) {
    clearCursorAtPosition(cursorMenuPositions[i]);
  }
}


void ClearAllGameCursors(int numberOfElementsOnMenu) {
  for (int i = 0; i < numberOfElementsOnMenu; i++) {
    clearCursorAtPosition(cursorGamePositions[i]);
  }
}

void handlePlayMenu() {

}

void handleInfoMenu() {
  checkButtonPressed();
}

void handleOptionsMenu() {
  joyXValue = analogRead(joyXPin);
  joyYValue = analogRead(joyYPin);

  handleMoveLeft();
  handleMoveRight();
  handleCenter();

  handleMoveUp(3);
  handleMoveDown(3);
}

void handleMainMenu() {
  joyYValue = analogRead(joyYPin);

  handleMoveDown(3);
  handleMoveUp(3);
  handleCenter();
}

void activateOptionsMenu() {
  currentMenu = OPTIONS;

  currentMenuCursorPosition = 0;

  myScreen.background(0, 0, 0);

  myScreen.setTextSize(1);
  myScreen.text("Difficulty <Easy>", 25, 20);
  myScreen.text("Reset score", 25, 50);
  myScreen.text("Back", 25, 80);

  myScreen.text("->", 0, 20);
}

void activateInfoMenu() {
  currentMenu = INFO;

  currentMenuCursorPosition = 0;

  myScreen.background(0, 0, 0);

  myScreen.setTextSize(2);
  myScreen.text("Space Groove", 0, 0);

  myScreen.setTextSize(1);
  myScreen.text("Made(with <3) by:", 0, 22);

  myScreen.setTextSize(2);
  myScreen.text("Laur Ivan", 0, 35);
  myScreen.text("@Unibuc", 10, 57);
  myScreen.text("Robotics", 10, 78);
  myScreen.text("Back", 30, 100);

  myScreen.text("->", 0, 100);
}

void activateMainMenu() {
  currentMenu = MAIN;

  currentMenuCursorPosition = 0;

  myScreen.background(0, 0, 0);

  myScreen.setTextSize(2);
  myScreen.text("Play", 50, 20);
  myScreen.text("Options", 50, 50);
  myScreen.text("Info", 50, 80);

  myScreen.text("->", 0, 20);
}

void activatePlayMenu() {

  currentMenu = PLAY;

  myScreen.background(0, 0, 0);

  String myScoreText = String(score);
  myScoreText.toCharArray(scoreText, 3);

  String myLivesText = String(lives);
  myLivesText.toCharArray(livesText, 3);

  myScreen.setTextSize(2);
  myScreen.text("Lives: ", 0, 0);
  myScreen.text(livesText, 73, 0);
  myScreen.text("Score:", 0, 55);
  myScreen.text(scoreText, 73, 55);
  myScreen.text("Level: ", 40, 100);

  String myLevelText = String(level);
  myLevelText.toCharArray(levelText, 3);
  myScreen.stroke(0, 0, 0);
  myScreen.text(levelText, 115, 100);

  level++;

  myLevelText = String(level);
  myLevelText.toCharArray(levelText, 3);
  myScreen.stroke(255, 255, 255);
  myScreen.text(levelText, 115, 100);
}

void updateScore() {
  myScreen.setTextSize(2);

  String myScoreText = String(score);
  myScoreText.toCharArray(scoreText, 3);

  myScreen.stroke(0, 0, 0);
  myScreen.text(scoreText, 73, 55);

  score++;

  myScoreText = String(score);
  myScoreText.toCharArray(scoreText, 3);

  myScreen.stroke(255, 255, 255);
  myScreen.text(scoreText, 73, 55);
}

void updateLives() {
  myScreen.setTextSize(2);

  String myLivesText = String(currentLives);
  myLivesText.toCharArray(livesText, 3);

  myScreen.stroke(0, 0, 0);
  myScreen.text(livesText, 73, 0);

  currentLives--;

  myLivesText = String(currentLives);
  myLivesText.toCharArray(livesText, 3);

  myScreen.stroke(255, 255, 255);
  myScreen.text(livesText, 73, 0);
}

void playerShoot() {
  spawnBullet();
  lastBulletUpdate = millis();
}

void spawnBullet() {
  int activeBulletIndex = 0;

  for (int i = bulletsUsed; i < numberOfBulletsAllowed; i++) {
    if (!myBullets[i].isActive) {
      myBullets[i].isActive = true;

      myBullets[i].bulletXPosition = currentXPos;
      myBullets[i].bulletYPosition = currentYPos - 1;

      lc.setLed(0, currentYPos - 1, currentXPos, true);
      activeBulletIndex = i;
      moveBullet(myBullets[i].bulletXPosition, i);

      break;
    }

    bulletsUsed++;
  }
}

void moveBullet(int xPosition, int bulletIndex) {
  int i = 5;//xPos -> column, i-> row
  bool destroyedEnemy = false;

  while (i >= 0) {
    if ((millis() - lastBulletUpdate) > bulletSpeed) {
      //check if bullet 'touches' enemy
      if (i < enemiesMatrixSize) {
        if (enemiesMatrix[i][xPosition] == 1) {
          short int enemyIndexInVector = (xPosition + i) + (matrixSize - 1) * i;
          myEnemies[enemyIndexInVector].isActive = false;
          enemiesMatrix[i][xPosition] = 0;
          lc.setLed(0, i, xPosition, false);
          destroyedEnemy = true;
          break;
        }
      }

      //turn off led at previous bullet position
      lc.setLed(0, i, xPosition, false);
      i--;

      //turn on led at next bullet position
      lc.setLed(0, i, xPosition, true);

      lastBulletUpdate = millis();
    }
  }

  if (!destroyedEnemy) {
    lc.setLed(0, 0, xPosition, false);
  }
  else {
    updateScore();

    currentNumberOfEnemies--;

    if (currentNumberOfEnemies == 0) {
      activateLevelFinished();
    }
  }

  bulletsUsed--;
  myBullets[bulletIndex].isActive = false;
}

void activateLevelFinished() {
  currentMenu = LEVEL_FINISHED;

  currentMenuCursorPosition = 0;

  myScreen.background(0, 0, 0);

  myScreen.setTextSize(2);
  myScreen.text("Good Job!", 35, 15);
  myScreen.text("Continue", 35, 60);
  myScreen.text("Quit", 35, 90);

  myScreen.text("->", 0, 60);
}

void backupVariables() {
  initialMinNumberOfRows = minNumberOfRows;
  initialMinNumberOfEnemiesOnEachRow = minNumberOfEnemiesOnEachRow;
  initialMaxNumberOfEnemiesOnEachRow = maxNumberOfEnemiesOnEachRow;
  initialTimeBetweenEnemyBullets = timeBetweenEnemyBullets;
  initialEnemyBulletSpeed = enemyBulletSpeed;
}

void resetGameVariables() {
  minNumberOfRows = initialMinNumberOfRows;
  minNumberOfEnemiesOnEachRow = initialMaxNumberOfEnemiesOnEachRow;
  maxNumberOfEnemiesOnEachRow = initialMaxNumberOfEnemiesOnEachRow;
  timeBetweenEnemyBullets = initialTimeBetweenEnemyBullets;
  enemyBulletSpeed = initialEnemyBulletSpeed;

  //reset and hide the score on display
  String myScoreText = String(score);
  myScoreText.toCharArray(scoreText, 3);
  myScreen.stroke(0, 0, 0);
  myScreen.text(scoreText, 73, 55);
  score = 0;
}

void resetLevelVariables() {
  currentNumberOfEnemies = 0;

  currentLives = lives;
}

void clearMatrixesAndValues() {
  lightOffMatrix();

  for (int i = 0; i < enemiesMatrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      enemiesMatrix[i][j] = 0;
    }
  }

  for (int i = 0; i < 32; i++) {
    myEnemies[i].isActive = false;
    myEnemies[i].enemyXPosition = 0;
    myEnemies[i].enemyYPosition = 0;
  }
}

void playNextLevel() {
  clearMatrixesAndValues();

  resetLevelVariables();

  activatePlayMenu();

  if (difficultyIsHard) {
    if (minNumberOfRows < 2) {
      minNumberOfRows++;
    }

    if (minNumberOfEnemiesOnEachRow < 5) {
      minNumberOfEnemiesOnEachRow++;
    }

    if (maxNumberOfEnemiesOnEachRow < 7) {
      maxNumberOfEnemiesOnEachRow++;
    }

    if (timeBetweenEnemyBullets > 60) {
      timeBetweenEnemyBullets -= 50;
    }

    if (enemyBulletSpeed > 50) {
      enemyBulletSpeed -= 10;
    }
  }
  else
  {
    if (minNumberOfEnemiesOnEachRow < 3) {
      minNumberOfEnemiesOnEachRow++;
    }

    if (maxNumberOfEnemiesOnEachRow < 7) {
      maxNumberOfEnemiesOnEachRow++;
    }

    if (timeBetweenEnemyBullets > 60) {
      timeBetweenEnemyBullets -= 25;
    }

    if (enemyBulletSpeed > 50) {
      enemyBulletSpeed -= 5;
    }
  }

  initializeGameplay();
}

void playAgain() {
  level = 0;

  activatePlayMenu();

  clearMatrixesAndValues();

  resetGameVariables();

  resetLevelVariables();

  initializeGameplay();
}

int valueIsTrue(int valueToCheck) {
  return valueToCheck == 1;
}

int valueIsFalse(int valueToCheck) {
  return valueToCheck == 0;
}

int valueIsLowerThanThreshold(int valueToCheck, int threshold) {
  return valueToCheck < threshold;
}

int valueIsHigherThanThreshold(int valueToCheck, int threshold) {
  return valueToCheck > threshold;
}
