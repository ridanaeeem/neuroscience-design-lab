typedef enum{
  programSetup,
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

// states for reading buttons
int leftButtonState = 0; 
int rightButtonState = 0; 

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
unsigned long cueLength = 150;
// when did the last trial end
unsigned long trialEnded = 0;
// how long between trials
unsigned long betweenTrials = 3000;

// trial specifications 
// probability out of 100 that the cue should be valid
float probValid = 75;
// for indexing the arrays
int trialCount = -1;
// update numbers below if trial number changed from 10
// make sure valid + invalid = nTrials 
int numValid = 5;
int numInvalid = 5;
int nTrials = 10;
// states what the cues should be (valid, invalid)
int cues[10] = {};
// states what the cues should be (endogenous 0, exogenous 1)
int endoExo[10] = {};
// whether or not they responded correctly
bool correctness[10] = {};
// results
unsigned long reactionTimes[10];

// starting with the first position in the array
int cueIndex = 0;
int cueNumber = 0;

// for printing purposes, enumerates correctness and cue types
String correctnessDisplay[] = {"incorrect", "correct"};
String cuesDisplay[] = {"validLeft", "validRight", "invalidLeft", "invalidRight"};
String endoExoDisplay[] = {"endogenous", "exogenous"};


void setup() {
  Serial.begin(9600);
  pinMode(leftStimulus, OUTPUT);
  pinMode(rightStimulus, OUTPUT);
  pinMode(ledLeftCue, OUTPUT);
  pinMode(ledRightCue, OUTPUT);

  programState = programSetup;
  randomSeed(analogRead(0));
  Serial.println(" ");
  Serial.println("In this experiment a light will flash and then an LED will turn on.");
  Serial.println("Once this LED turns on after the flash, click the button corresponding to that LED.");
  Serial.println("(Left button for left LED, right button for right LED)");
  Serial.println("Don't click the button until the LED turns on after the flash.");
  Serial.println("Press the button when you're ready to begin the experiment.");
  Serial.println(" ");
}

void loop() {
  if (trialCount < nTrials){
    unsigned long currentMillis = millis();
    leftButtonState = digitalRead(leftButton);
    rightButtonState = digitalRead(rightButton);

    switch(programState){
      case programSetup:

        // swapping based randomization code, w requirements for valid vs invalid
        for (int i=0; i < nTrials; i++){
          if (numValid != 0){
            float rand = random(0,100);
            if (rand <= 50) cues[i] = 0;
            else cues[i] = 1;
            numValid --;
          } else if (numInvalid != 0){
            float rand = random(0,100);
            if (rand <= 50) cues[i] = 2;
            else cues[i] = 3;
            numInvalid --;
          }
          //0 is endo 1 is exo
          endoExo[i] = random(2);
        }

        randomizeArray(cues, nTrials);
        
        if (leftButtonState == 1 || rightButtonState == 1){
          programState = prompt;
        }

      break;

      case prompt:
        // make sure everything is off in case coming from cheating or timing out
        digitalWrite(ledLeftCue, LOW);
        digitalWrite(ledRightCue, LOW);
        if (leftButtonState == 0 && rightButtonState == 0){
          // message for starting the experiment
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
        } else {
          programState = prompt;
        }
      break;

      // left cue --> left stimulus
      case validLeft:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating");
          programState = prompt;
        }

        if (endoExo[cueIndex] == 0){
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
        } else {
          // exogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            digitalWrite(leftStimulus, HIGH);
          } else {
            digitalWrite(leftStimulus, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              digitalWrite(leftStimulus, HIGH);
              lightOn = millis();
              programState = waitLeftPress;
            }
          }
        }
     
      break;

      // right cue --> right stimulus
      case validRight:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating");
          programState = prompt;
        }

        if (endoExo[cueIndex] == 0){
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
        } else {
          // exogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            digitalWrite(rightStimulus, HIGH);
          } else {
            digitalWrite(rightStimulus, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              digitalWrite(rightStimulus, HIGH);
              lightOn = millis();
              programState = waitRightPress;
            }
          }
        }
      break;

      // right cue --> left stimulus
      case invalidLeft:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating");
          programState = prompt;
        }

        if (endoExo[cueIndex] == 0){
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
        } else {
          // exogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            digitalWrite(rightStimulus, HIGH);
          } else {
            digitalWrite(rightStimulus, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              digitalWrite(leftStimulus, HIGH);
              lightOn = millis();
              programState = waitLeftPress;
            }
          }
        }
      break;

      // left cue --> right stimulus
      case invalidRight:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating");
          programState = prompt;
        }

        if (endoExo[cueIndex] == 0){
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
        } else {
          // exogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            digitalWrite(leftStimulus, HIGH);
          } else {
            digitalWrite(leftStimulus, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              digitalWrite(rightStimulus, HIGH);
              lightOn = millis();
              programState = waitRightPress;
            }
          }
        }
      break;

      case waitLeftPress:
        if (millis() - lightOn >= 5000){
            Serial.println("You took too long. Press the button when you're ready to try again");
            // turn light off and set lightOn to zero so this loop doesn't repeat
            digitalWrite(leftStimulus, LOW);
            lightOn = 0;
            programState = prompt;
          }
        // reacted within an appropriate amount of time and pressed the right button
        if (leftButtonState == 1){
          // reaction time is the current time minus the time the light turned on
          reactionTime = millis() - lightOn;
          Serial.println(reactionTime);
          // turn the light off
          digitalWrite(leftStimulus, LOW);
          // add to array
          trialCount++;
          reactionTimes[trialCount] = reactionTime;
          correctness[trialCount] = true;
          if (trialCount == nTrials) displayResults();
          // go back to beginning
          trialEnded = currentMillis;
          programState = begin;
        }
        // reacted within an appropriate amount of time but pressed the wrong button
        if (rightButtonState == 1){
          // reaction time is the current time minus the time the light turned on
          reactionTime = millis() - lightOn;
          Serial.println(reactionTime);
          // turn the light off
          digitalWrite(leftStimulus, LOW);
          // add to array
          trialCount++;
          reactionTimes[trialCount] = reactionTime;
          correctness[trialCount] = false;
          if (trialCount == nTrials) displayResults();
          // go back to beginning
          trialEnded = currentMillis;
          programState = begin;
        }
      break;

      case waitRightPress:
        if (millis() - lightOn >= 5000){
          Serial.println("You took too long. Press the button when you're ready to try again");
          // turn light off and set lightOn to zero so this loop doesn't repeat
          digitalWrite(rightStimulus, LOW);
          lightOn = 0;
          programState = prompt;
        }
        // reacted within an appropriate amount of time and pressed the right button
        if (rightButtonState == 1){
          // reaction time is the current time minus the time the light turned on
          reactionTime = millis() - lightOn;
          Serial.println(reactionTime);
          // turn the light off
          digitalWrite(rightStimulus, LOW);
          // add to array
          trialCount++;
          reactionTimes[trialCount] = reactionTime;
          correctness[trialCount] = true;
          if (trialCount == nTrials) displayResults();
          // go back to beginning
          trialEnded = currentMillis;
          programState = begin;
        }
        // reacted within an appropriate amount of time but pressed the wrong button
        if (leftButtonState == 1){
          // reaction time is the current time minus the time the light turned on
          reactionTime = millis() - lightOn;
          Serial.println(reactionTime);
          // turn the light off
          digitalWrite(rightStimulus, LOW);
          // add to array
          trialCount++;
          reactionTimes[trialCount] = reactionTime;
          correctness[trialCount] = false;
          if (trialCount == nTrials) displayResults();
          // go back to beginning
          trialEnded = currentMillis;
          programState = begin;
        }
      break;

      default:
        Serial.println("Uh oh, let the experimenter know something bad happened");
      }
  }
}

// returns nothing, takes in nothing
void displayResults(void){
  float meanReactionTime = 0;
  Serial.print("All reaction times: ");
  Serial.println(" ");
  for (int i=0; i < nTrials; i++){
    int correctIndex = correctness[i];
    int cueIndex = cues[i];
    int endoExoIndex = endoExo[i];
    Serial.print(reactionTimes[i]);
    Serial.print(" ");
    Serial.print(correctnessDisplay[correctIndex]);
    Serial.print(" ");
    Serial.print(cuesDisplay[cueIndex]);
    Serial.print(" ");
    Serial.print(endoExoDisplay[endoExoIndex]);
    Serial.println(" ");
    meanReactionTime += (reactionTimes[i] / nTrials);
  }
  Serial.print("\nAverage reaction time: ");
  Serial.print(meanReactionTime);
  Serial.print("\n");
}

// via chatgpt
void randomizeArray(int arr[], int size) {
  for (int i = size - 1; i > 0; i--) {
    int j = random(0, i + 1); // Generate a random index between 0 and i
    // Swap arr[i] with the element at the random index
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}