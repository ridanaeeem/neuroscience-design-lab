typedef enum{
  validLeft,
  invalidLeft,
  validRight,
  invalidRight
} state_defs;
state_defs cueType;

// stimuli (2 LEDs)
const int leftStimulus = 2;  
const int rightStimulus = 13;
// endogenous visual cues (2 LEDs)
const int ledLeftCue = 7; 
const int ledRightCue = 10;

// buttons
int leftButton = 4;
int rightButton = 11;

// Variables will change:
int ledState = LOW;  // ledState used to set the LED
int buttonState = 0;  // variable for reading the pushbutton status
int buttonState2 = 0;  // variable for reading the pushbutton status


// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;  // will store last time LED was updated

// constants won't change:
const long interval = 1000;  // interval at which to blink (milliseconds)

void setup() {
  Serial.begin(9600);
  pinMode(leftStimulus, OUTPUT);
  pinMode(rightStimulus, OUTPUT);
  pinMode(ledLeftCue, OUTPUT);
  pinMode(ledRightCue, OUTPUT);
}

void loop() {
  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    // save the last time you blinked the LED
    previousMillis = currentMillis;

    // if the LED is off turn it on and vice-versa:
    if (ledState == LOW) {
      ledState = HIGH;
    } else {
      ledState = LOW;
    }

    // set the LED with the ledState of the variable:
    digitalWrite(leftStimulus, ledState);
    digitalWrite(rightStimulus, ledState);
    digitalWrite(ledLeftCue, ledState);
    digitalWrite(ledRightCue, ledState);
  }

  buttonState = digitalRead(leftButton);
  buttonState2 = digitalRead(rightButton);


  // check if the pushbutton is pressed. If it is, the buttonState is HIGH:
  if (buttonState == HIGH) {
    // turn LED on:
    Serial.println("leftClick");
  } else if (buttonState2 == HIGH) {
    // turn LED off:
    Serial.println("rightClick");
  }

  switch(cueType){
    case validLeft:
      // Serial.println("valid left");
    break;
    case validRight:
      // Serial.println("valid right");
    break;
    case invalidLeft:
      // Serial.println("invalid left");
    break;
    case invalidRight:
      // Serial.println("invalid right");
    break;
  }
}