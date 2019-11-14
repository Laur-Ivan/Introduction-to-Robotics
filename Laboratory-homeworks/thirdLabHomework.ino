const int buttonPin = 2;
const int speakerBuzzerPin = 3;
const int knockDetectorBuzzerPin = A0;

int knockDetectorBuzzerValue = 0;
int knockDetectorThreshold = 1;

int knocked = 0;

unsigned int currentCounter = 0;
unsigned int previousCounter = 0;

int timeDelay = 5000;

int buttonState = 0;
int lastButtonState = 0;
int reading = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50; 

int buzzerTone = 1000;

void setup() {
  InitializePinModes();
  Serial.begin(9600);
}

void loop() {
  knockDetectorBuzzerValue = analogRead(knockDetectorBuzzerPin);

  reading = digitalRead(buttonPin);

  if(reading != lastButtonState){
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    buttonState = reading;

    if(buttonState == HIGH && knocked){
      knocked = 0;
      noTone(speakerBuzzerPin);
    }
  }

  lastButtonState = reading;

  if(knockDetectorBuzzerValue != 0){
    Serial.println(knockDetectorBuzzerValue);
  }
  
  if(knockDetectorBuzzerValue >= knockDetectorThreshold){ 
    if(knocked == 0){
      previousCounter = millis(); 
      Serial.println("knock"); 
    }
  }

  if(knocked == 1){
    Serial.println("CurrentCounter:");
    Serial.println(currentCounter);
    Serial.println("previousCounter:");
    Serial.println(previousCounter);
    
    if((previousCounter + timeDelay) < millis()){
     tone(speakerBuzzerPin, buzzerTone);
     Serial.println("got in if");
    }
  }
}

void InitializePinModes()
{
  pinMode(buttonPin, INPUT);
  pinMode(speakerBuzzerPin, OUTPUT);
  pinMode(knockDetectorBuzzerPin, INPUT);
}
