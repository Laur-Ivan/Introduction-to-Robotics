//Joystick pins
const int pinSW = A2; // digital pin connected to switch output
const int pinX = A0; // A0 - analog pin connected to X output
const int pinY = A1; // A1 - analog pin connected to Y output

//joystick movement variables
int xAxisValue = 0;
int yAxisValue = 0;

int minThreshold = 400;
int maxThreshold = 600;

bool buttonState = false;
bool joyMoved = false;
bool lastButtonState = false;

int lastDebounce = 0;
int debounceInterval = 10;

//4-digit 7-segments pins
const int pinA = 12;
const int pinB = 8;
const int pinC = 5;
const int pinD = 3;
const int pinE = 2;
const int pinF = 11;
const int pinG = 6;
const int pinDP = 4;
const int pinD1 = 7;
const int pinD2 = 9;
const int pinD3 = 10;
const int pinD4 = 13;

//4-digit 7-segments mechanics
bool dpState = false;

const int segSize = 8;
const int noOfDisplays = 4;
const int noOfDigits = 10;

int segments[segSize] = {pinA, pinB, pinC, pinD, pinE, pinF, pinG, pinDP};
int digits[noOfDisplays] = {pinD1, pinD2, pinD3, pinD4};

int currentDigitsValues[noOfDisplays] = {0, 0, 0, 0};

int currentDigit = 0;
int resetInterval = 1;
int lastReset = 0;
int selectedDigit = noOfDisplays;
int blinkingDigit = noOfDisplays - 1;
int blinkingState = true;
int blinkingInterval = 500;
int lastBlink = 0;

byte digitMatrix[noOfDigits][segSize - 1] = {
// a  b  c  d  e  f  g
  {1, 1, 1, 1, 1, 1, 0}, // 0
  {0, 1, 1, 0, 0, 0, 0}, // 1
  {1, 1, 0, 1, 1, 0, 1}, // 2
  {1, 1, 1, 1, 0, 0, 1}, // 3
  {0, 1, 1, 0, 0, 1, 1}, // 4
  {1, 0, 1, 1, 0, 1, 1}, // 5
  {1, 0, 1, 1, 1, 1, 1}, // 6
  {1, 1, 1, 0, 0, 0, 0}, // 7
  {1, 1, 1, 1, 1, 1, 1}, // 8
  {1, 1, 1, 1, 0, 1, 1}  // 9
};

void setup() {
  initializePins();
}

void loop() {
  //if no digit is selected (the button not pressed), the hovered one is blinking
  if (selectedDigit == noOfDisplays) {
    xAxisValue = analogRead(pinX);

    handleMoveLeft();
    handleMoveRight();
    handleCenter();
    handleBlink();

    resetDisplay();
  }
  //if we pressed the button, we want to change the digit hovered
  else {
    yAxisValue = analogRead(pinY);

    handleMoveDown();
    handleMoveUp();
    handleDigitCenter();

    resetDigits();
  }

  //in order to do all the logic above, we need to check if the button is pressed (i.e. the variables have changed)
  checkButtonPressed();
}


//load functions
void initializePins() {
  pinMode(pinSW, INPUT_PULLUP);

  for (int i = 0; i < segSize; i++) {
    pinMode(segments[i], OUTPUT);
  }

  for (int i = 0; i < noOfDisplays; i++) {
    pinMode(digits[i], OUTPUT);
  }
}


//logic functions
void handleMoveLeft() {
  if (xAxisValue < minThreshold && joyMoved == false) {
    if (blinkingDigit > 0) {
      --blinkingDigit;
    }
    else {
      blinkingDigit = noOfDisplays - 1;
    }
    blinkingState = true;
    lastBlink = millis();
    joyMoved = true;
  }
}

void handleMoveRight() {
  if (xAxisValue > maxThreshold && joyMoved == false) {
    if (blinkingDigit < noOfDisplays - 1) {
      ++blinkingDigit;
    }
    else {
      blinkingDigit = 0;
    }
    blinkingState = true;
    lastBlink = millis();
    joyMoved = true;
  }
}

void handleCenter() {
  if (xAxisValue >= minThreshold && xAxisValue <= maxThreshold) {
    joyMoved = false;
  }
}

void handleBlink() {
  if (millis() - lastBlink >= blinkingInterval) {
    lastBlink = millis();
    blinkingState = !blinkingState;
  }
}

void handleMoveDown() {
  if (yAxisValue < minThreshold && joyMoved == false) {
    if (currentDigitsValues[selectedDigit] > 0) {
      --currentDigitsValues[selectedDigit];
    }
    else {
      currentDigitsValues[selectedDigit] = noOfDigits - 1;
    }
    joyMoved = true;
  }
}

void handleMoveUp() {
  if (yAxisValue > maxThreshold && joyMoved == false) {
    if (currentDigitsValues[selectedDigit] < noOfDigits - 1) {
      ++currentDigitsValues[selectedDigit];
    }
    else {
      currentDigitsValues[selectedDigit] = 0;
    }
    joyMoved = true;
  }
}

void handleDigitCenter() {
  if (yAxisValue >= minThreshold && yAxisValue <= maxThreshold) {
    joyMoved = false;
  }
}

void checkButtonPressed() {
  if (millis() - lastDebounce >= debounceInterval) {
    lastDebounce = millis();
    buttonState = !digitalRead(pinSW);
    if (buttonState != lastButtonState) {
      if (buttonState == true) {
        if (selectedDigit == noOfDisplays) {
          selectedDigit = blinkingDigit;
          blinkingDigit = noOfDisplays;
        }
        else {
          blinkingDigit = selectedDigit;
          selectedDigit = noOfDisplays;
          blinkingState = true;
          lastBlink = millis();
        }
      }
      lastButtonState = buttonState;
    }
  }
}

//display functions
void resetDisplay() {
  if (millis() - lastReset >= resetInterval) {
    if (currentDigit == blinkingDigit && blinkingState == false) {
      clearDigit(currentDigit);
    }
    else {
      displayDigit(currentDigit, currentDigitsValues[currentDigit], false);
    }
    lastReset = millis();
    if (currentDigit == noOfDisplays - 1) {
      currentDigit = 0;
    }
    else {
      ++currentDigit;
    }
  }
}

void resetDigits() {
  if (millis() - lastReset >= resetInterval) {
    if (currentDigit == selectedDigit) {
      displayDigit(currentDigit, currentDigitsValues[currentDigit], true);
    }
    else {
      displayDigit(currentDigit, currentDigitsValues[currentDigit], false);
    }
    lastReset = millis();
    if (currentDigit == noOfDisplays - 1) {
      currentDigit = 0;
    }
    else {
      ++currentDigit;
    }
  }
}

void displayDigit(int num, byte digit, byte decimalPoint) {
  showDigit(num);

  for (int i = 0; i < segSize - 1; i++) {
    digitalWrite(segments[i], digitMatrix[digit][i]);
  }

  digitalWrite(segments[segSize - 1], decimalPoint);
}

void showDigit(int num) {
  for (int i = 0; i < noOfDisplays; i++) {
    digitalWrite(digits[i], HIGH);
  }

  digitalWrite(digits[num], LOW);
}

void clearDigit(int num) {
  showDigit(num);

  for (int i = 0; i < segSize; ++i) {
    digitalWrite(segments[i], LOW);
  }
}
