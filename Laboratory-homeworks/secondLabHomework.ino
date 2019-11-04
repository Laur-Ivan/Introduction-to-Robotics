const int redLedPot = A5;
const int greenLedPot = A3;
const int blueLedPot = A1;

const int redLedPin = 3;
const int greenLedPin = 5;
const int blueLedPin = 6;

int readValueRedPot = 0;
int readValueGreenPot = 0;
int readValueBluePot = 0;

int mappedValueRedPot = 0;
int mappedValueGreenPot = 0;
int mappedValueBluePot = 0;

void setup() {
  SetPotPinMode();
  SetLedPinMode();
}

void loop() {
  ReadPotsValues();
  MapPotsValues();
  SetRGBColors(mappedValueRedPot, mappedValueGreenPot, mappedValueBluePot);
}

void SetPotPinMode(){
  pinMode(redLedPot, INPUT);
  pinMode(greenLedPot, INPUT);
  pinMode(blueLedPot, INPUT);
}

void SetLedPinMode(){
  pinMode(redLedPin, OUTPUT);
  pinMode(greenLedPin, OUTPUT);
  pinMode(blueLedPin, OUTPUT);
}

void SetRGBColors(int red, int green, int blue){
  analogWrite(redLedPin, red);
  analogWrite(greenLedPin, green);
  analogWrite(blueLedPin, blue);
}

void ReadPotsValues(){
  readValueRedPot = analogRead(redLedPot);
  readValueGreenPot = analogRead(greenLedPot);
  readValueBluePot = analogRead(blueLedPot);
}

void MapPotsValues(){
  mappedValueRedPot = map(readValueRedPot, 0, 1023, 0, 255);
  mappedValueGreenPot = map(readValueGreenPot, 0, 1023, 0, 255);
  mappedValueBluePot = map(readValueBluePot, 0, 1023, 0, 255);
}
