typedef enum{
  prompt,
  begin,
  validLeft,
  invalidLeft,
  validRight,
  invalidRight,
  waitLeftPress,
  waitRightPress
} state_defs;
state_defs programState;

// pin numbers
const int leftStimulus = 2;  
const int rightStimulus = 13;
const int ledLeftCue = 7; 
const int ledRightCue = 10;
int leftButton = 4;
int rightButton = 11;

// states of LEDS and buttons
int ledState = LOW;  // ledState used to set the LED
int leftButtonState = 0;  // variable for reading the pushbutton status
int rightButtonState = 0;  // variable for reading the pushbutton status

// time variables
// how long the light has been on
unsigned long lightOn = 0;
// how long it took the person to react
unsigned long reactionTime = 0;
// last time light was turned on
unsigned long previousMillis = 0;
// how long (ms) until stimulus should turn on
unsigned long stimulusDelay; 
// what time the cue started (when begin ends)
unsigned long cueTurnOn = 0;
// how long the cue stays on
unsigned long cueLength = 500;
// when did the last trial end
unsigned long trialEnded = 0;
// how long between trials
unsigned long betweenTrials = 3000;

// trial specifications
int nTrials = 5;
int trialCount = 0;

// deciding what types to do
// left (0) vs right (1)
int cues[5] = {0,1,2,3,1};
int cueIndex = 0;
int cueNumber = 0;

// endogenous (0) vs exogenous (1)
// int types[5] = {0,1,0,1,1};
// int typeIndex = 0;
// int typeNumber = 0;



void setup() {
  Serial.begin(9600);
  pinMode(leftStimulus, OUTPUT);
  pinMode(rightStimulus, OUTPUT);
  pinMode(ledLeftCue, OUTPUT);
  pinMode(ledRightCue, OUTPUT);

  programState = begin;
  randomSeed(analogRead(0));
}

void loop() {
  if (trialCount < nTrials){
    unsigned long currentMillis = millis();
    leftButtonState = digitalRead(leftButton);
    rightButtonState = digitalRead(rightButton);

    switch(programState){
      case prompt:
        // make sure everything is off in case coming from cheating
        digitalWrite(ledLeftCue, LOW);
        digitalWrite(ledRightCue, LOW);
        if (leftButtonState == 0 && rightButtonState == 0){
          // message for starting the experiment
          Serial.println("Press the button when you're ready to begin");
          trialEnded = currentMillis;
          programState = begin;
        }
      break;

      case begin:
        // if enough time has elapsed between the last trial and right now, start a new one
        if (leftButtonState == 0 && rightButtonState == 0){
          if (currentMillis - trialEnded >= betweenTrials){
            stimulusDelay = random(1000,3000);
            previousMillis = currentMillis;
            cueTurnOn = currentMillis;
            cueNumber = cues[cueIndex];
            cueIndex++;
            if (cueNumber == 0) programState = validLeft;
            if (cueNumber == 1) programState = validRight;
            if (cueNumber == 2) programState = invalidLeft;
            if (cueNumber == 3) programState = invalidRight;
          }
        }
      break;

      // left cue --> left stimulus
      case validLeft:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating");
          programState = prompt;
        }

        // endogenous cue
        if (currentMillis - cueTurnOn <= cueLength){
          digitalWrite(ledLeftCue, HIGH);
        } else {
          digitalWrite(ledLeftCue, LOW);
          if (currentMillis - previousMillis >= stimulusDelay) {
            previousMillis = currentMillis;
            digitalWrite(leftStimulus, HIGH);
            lightOn = millis();
            programState = waitLeftPress;
          }
        }
        // exogenous cue
        
      break;

      // right cue --> right stimulus
      case validRight:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating");
          programState = prompt;
        }

        // endogenous cue
        if (currentMillis - cueTurnOn <= cueLength){
          digitalWrite(ledRightCue, HIGH);
        } else {
          digitalWrite(ledRightCue, LOW);
          if (currentMillis - previousMillis >= stimulusDelay) {
            previousMillis = currentMillis;
            digitalWrite(rightStimulus, HIGH);
            lightOn = millis();
            programState = waitRightPress;
          }
        }
        // exogenous cue

      break;

      // right cue --> left stimulus
      case invalidLeft:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating");
          programState = prompt;
        }

        // endogenous cue
        if (currentMillis - cueTurnOn <= cueLength){
          digitalWrite(ledRightCue, HIGH);
        } else {
          digitalWrite(ledRightCue, LOW);
          if (currentMillis - previousMillis >= stimulusDelay) {
            previousMillis = currentMillis;
            digitalWrite(leftStimulus, HIGH);
            lightOn = millis();
            programState = waitLeftPress;
          }
        }
        // exogenous cue
      break;

      // left cue --> right stimulus
      case invalidRight:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating");
          programState = prompt;
        }

        // endogenous cue
        if (currentMillis - cueTurnOn <= cueLength){
          digitalWrite(ledLeftCue, HIGH);
        } else {
          digitalWrite(ledLeftCue, LOW);
          if (currentMillis - previousMillis >= stimulusDelay) {
            previousMillis = currentMillis;
            digitalWrite(rightStimulus, HIGH);
            lightOn = millis();
            programState = waitRightPress;
          }
        }
        // exogenous cue
      break;

      case waitLeftPress:
        if (millis() - lightOn >= 5000){
            Serial.println("You took too long, try again!");
            // turn light off and set lightOn to zero so this loop doesn't repeat
            digitalWrite(leftStimulus, LOW);
            lightOn = 0;
            programState = begin;
          }
        // reacted within an appropriate amount of time and pressed the right button
        if (leftButtonState == 1){
          // reaction time is the current time minus the time the light turned on
          reactionTime = millis() - lightOn;
          // turn the light off
          digitalWrite(leftStimulus, LOW);
          Serial.println(reactionTime);
          // print the reaction time in milliseconds
          // Serial.println(reactionTime);
          // add to array
          // reactionTimes[trialCount] = reactionTime;
          // trialCount++;
          // if (trialCount == nTrials) displayResults();
          // go back to beginning
          trialEnded = currentMillis;
          programState = begin;
          trialCount++;
        }
        // reacted within an appropriate amount of time but pressed the wrong button
        if (rightButtonState == 1){
          // reaction time is the current time minus the time the light turned on
          digitalWrite(leftStimulus, LOW);
          Serial.println("wrong");
          trialEnded = currentMillis;
          programState = begin;
          trialCount++;
        }
      break;

      case waitRightPress:
        if (millis() - lightOn >= 5000){
          Serial.println("You took too long, try again!");
          // turn light off and set lightOn to zero so this loop doesn't repeat
          digitalWrite(rightStimulus, LOW);
          lightOn = 0;
          trialEnded = currentMillis;
          programState = begin;
        }
        // reacted within an appropriate amount of time and pressed the right button
        if (rightButtonState == 1){
          // reaction time is the current time minus the time the light turned on
          reactionTime = millis() - lightOn;
          // turn the light off
          digitalWrite(rightStimulus, LOW);
          Serial.println(reactionTime);
          trialEnded = currentMillis;
          programState = begin;
          trialCount++;
        }
        // reacted within an appropriate amount of time but pressed the wrong button
        if (leftButtonState == 1){
          // reaction time is the current time minus the time the light turned on
          digitalWrite(rightStimulus, LOW);
          Serial.println("wrong");
          trialEnded = currentMillis;
          programState = begin;
          trialCount++;
        }
      break;

      default:
        Serial.println("Uh oh, let the experimenter know something bad happened");
      }
  }
}
