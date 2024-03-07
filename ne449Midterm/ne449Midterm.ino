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
const int ledLeftStimulus = 2;  
const int ledRightStimulus = 13;
const int ledLeftCue = 7; 
const int ledRightCue = 10;
const int vibLeftStimulus = 5;
const int vibRightStimulus = 9;
const int vibLeftCue = 6;
const int vibRightCue = 8;

int leftButton = 4;
int rightButton = 11;

// states for reading buttons
int leftButtonState = 0; 
int rightButtonState = 0; 

// time variables
// how long the light/vib has been on
unsigned long stimOn = 0;
// how long it took the person to react
unsigned long reactionTime = 0;
// last time light/vib was turned on
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
// for indexing the arrays
int trialCount = 0;
// update numbers below if trial number changes from 20
// make sure valid + invalid = nTrials 
int numValid = 10;
int numInvalid = 10;
int nTrials = 20;
// states what the cues should be (valid, invalid)
int cues[20] = {};
// states what the cues should be (endogenous 0, exogenous 1)
int endoExo[20] = {};
// whether or not they responded correctly
bool correctness[20] = {};
// results
unsigned long reactionTimes[20];

// the current cue type
int cueNumber = 0;

//what stimulus type we are working with
bool visual = true;
bool tactile = false;

// for printing purposes, enumerates correctness and cue types
String correctnessDisplay[] = {"incorrect", "correct"};
String cuesDisplay[] = {"validLeft", "validRight", "invalidLeft", "invalidRight"};
String endoExoDisplay[] = {"endogenous", "exogenous"};


void setup() {
  Serial.begin(9600);
  // set all the stimuli to be outputs
  pinMode(ledLeftStimulus, OUTPUT);
  pinMode(ledRightStimulus, OUTPUT);
  pinMode(ledLeftCue, OUTPUT);
  pinMode(ledRightCue, OUTPUT);
  pinMode(vibLeftStimulus, OUTPUT);
  pinMode(vibRightStimulus, OUTPUT);
  pinMode(vibLeftCue, OUTPUT);
  pinMode(vibRightCue, OUTPUT);

  programState = programSetup;
  randomSeed(analogRead(0));

  // digitalWrite(vibRightCue, HIGH);
  // digitalWrite(vibLeftCue, HIGH);
  // digitalWrite(vibLeftStimulus, HIGH);
  // digitalWrite(vibRightStimulus, HIGH);

  Serial.println(" ");
  Serial.println("In this experiment a light will flash and then an LED will turn on.");
  Serial.println("Once this LED turns on after the flash, click the button corresponding to that LED.");
  Serial.println("(Left button for left LED, right button for right LED)");
  Serial.println("Don't click the button until the LED turns on after the flash.");
  Serial.println("Press the button when you're ready to begin the experiment.");
  Serial.println(" ");
}

void loop() {
  // only proceed if not at desired number of trials yet
  if (trialCount < nTrials){
    // always keep track of current time and button states
    unsigned long currentMillis = millis();
    leftButtonState = digitalRead(leftButton);
    rightButtonState = digitalRead(rightButton);
    // these will dynamically update with each trial
    int validCount = numValid;
    int invalidCount = numInvalid;

    switch(programState){
      // decides what order to do cues in according to specficications above
      case programSetup:
        // swapping based randomization code, w requirements for valid vs invalid
        // only swaps from the current trial we're on to the end
        // so the older values that already happened are not lost
        for (int i=trialCount; i < nTrials; i++){
          if (validCount > 0){
            float rand = random(0,100);
            if (rand <= 50) cues[i] = 0; //validLeft
            else cues[i] = 1; // validRight
            validCount --;
          } else if (invalidCount > 0){
            float rand = random(0,100);
            if (rand <= 50) cues[i] = 2; //invalidLeft
            else cues[i] = 3; //invalidRight
            invalidCount --;
          }
          //0 is endo 1 is exo
          endoExo[i] = random(2);
        }

        // randomize order of array
        randomizeArray(cues, nTrials);

        // move on once user presses button
        if (leftButtonState == 1 || rightButtonState == 1){
          programState = prompt;
          // Serial.println("post-swap");
          // for (int i=0; i < nTrials; i++){
          //   Serial.println(cues[i]);
          // }
          // Serial.println(" ");
        }
      break;

      // marks when the trial starts
      case prompt:
        // make sure everything is off in case coming from cheating or timing out
        if (visual){
          digitalWrite(ledLeftCue, LOW);
          digitalWrite(ledRightCue, LOW);
        } else if (tactile) {
          digitalWrite(vibLeftCue, LOW);
          digitalWrite(vibRightCue, LOW);
        }
        // make sure button is not being held down and then proceed
        if (leftButtonState == 0 && rightButtonState == 0){
          // marks when to start keeping track of the time for next state
          trialEnded = currentMillis;
          programState = begin;
        }
      break;

      // keeps track of when to turn the cue on
      case begin:
        // if enough time has elapsed between the last trial and right now, start a new one
        // only if the user is not holding down the button
        if (leftButtonState == 0 && rightButtonState == 0){
          if (currentMillis - trialEnded >= betweenTrials){
            // how long until stimulus should turn on
            stimulusDelay = random(1000,3000);
            previousMillis = currentMillis;
            cueTurnOn = currentMillis;

            // what cue should be done at this trial
            cueNumber = cues[trialCount];
            if (cueNumber == 0) programState = validLeft;
            if (cueNumber == 1) programState = validRight;
            if (cueNumber == 2) programState = invalidLeft;
            if (cueNumber == 3) programState = invalidRight;
            Serial.println(cuesDisplay[cueNumber]);
          }
        // if the user is holding down the button, go back and wait for them to let go
        } else {
          programState = prompt;
        }
      break;

      // left cue --> left stimulus
      case validLeft:
      // button should not be pressed until the stimulus turns on
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating VL");
          programState = prompt;
        }

        if (endoExo[trialCount] == 0){
          // endogenous cue - cue is in the central visual field
          // keep cue on for the specified amount of time
          if (currentMillis - cueTurnOn <= cueLength){
            if (visual) digitalWrite(ledLeftCue, HIGH);
            else if (tactile) digitalWrite(vibLeftCue, HIGH);
          // once that time has passed, turn the cue off
          } else {
            if (visual) digitalWrite(ledLeftCue, LOW);
            else if (tactile) digitalWrite(vibLeftCue, LOW);
            // after stimulus delay time has passed, turn the stimulus on and wait for response
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              if (visual) digitalWrite(ledLeftStimulus, HIGH);
              else if (tactile) digitalWrite(vibLeftStimulus, HIGH);
              stimOn = millis();
              programState = waitLeftPress;
            }
          }
        } else {
          // exogenous cue - cue is in peripheral aka cue is also the stimulus LED
          // keep cue on for the specified amount of time
          if (currentMillis - cueTurnOn <= cueLength){
            if (visual) digitalWrite(ledLeftStimulus, HIGH);
            else if (tactile) digitalWrite(vibLeftStimulus, HIGH);
          // once that time has passed, turn the cue off
          } else {
            if (visual) digitalWrite(ledLeftStimulus, LOW);
            else if (tactile) digitalWrite(vibLeftStimulus, LOW);
            // after stimulus delay time has passed, turn the stimulus on and wait for response
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              if (visual) digitalWrite(ledLeftStimulus, HIGH);
              else if (tactile) digitalWrite(vibLeftStimulus, HIGH);
              stimOn = millis();
              programState = waitLeftPress;
            }
          }
        }
     
      break;

      // right cue --> right stimulus
      case validRight:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating VR");
          programState = prompt;
        }

        if (endoExo[trialCount] == 0){
          // endogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            if (visual) digitalWrite(ledRightCue, HIGH);
            else if (tactile) digitalWrite(vibRightCue, HIGH);
          } else {
            digitalWrite(ledRightCue, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              if (visual) digitalWrite(ledRightStimulus, HIGH);
              if (tactile) digitalWrite(vibRightStimulus, HIGH);
              stimOn = millis();
              programState = waitRightPress;
            }
          }
        } else {
          // exogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            if (visual) digitalWrite(ledRightStimulus, HIGH);
            if (tactile) digitalWrite(vibRightStimulus, HIGH);
          } else {
            if (visual) digitalWrite(ledRightStimulus, LOW);
            else if (tactile) digitalWrite(vibRightStimulus, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              if (visual) digitalWrite(ledRightStimulus, HIGH);
              else if (tactile) digitalWrite(vibRightStimulus, HIGH);
              stimOn = millis();
              programState = waitRightPress;
            }
          }
        }
      break;

      // right cue --> left stimulus
      case invalidLeft:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating IL");
          programState = prompt;
        }

        if (endoExo[trialCount] == 0){
          // endogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            if (visual) digitalWrite(ledRightCue, HIGH);
            else if (tactile) digitalWrite(vibRightCue, HIGH);
          } else {
            if (visual) digitalWrite(ledRightCue, LOW);
            else if (tactile) digitalWrite(vibRightCue, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              if (visual) digitalWrite(ledLeftStimulus, HIGH);
              else if (tactile) digitalWrite(vibLeftStimulus, HIGH);
              stimOn = millis();
              programState = waitLeftPress;
            }
          }
        } else {
          // exogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            if (visual) digitalWrite(ledRightStimulus, HIGH);
            else if (tactile) digitalWrite(vibRightStimulus, HIGH);
          } else {
            if (visual) digitalWrite(ledRightStimulus, LOW);
            else if (tactile) digitalWrite(vibRightStimulus, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              if (visual) digitalWrite(ledLeftStimulus, HIGH);
              else if (tactile) digitalWrite(vibLeftStimulus, HIGH);
              stimOn = millis();
              programState = waitLeftPress;
            }
          }
        }
      break;

      // left cue --> right stimulus
      case invalidRight:
        if (leftButtonState == 1 || rightButtonState == 1){
          Serial.println("Stop cheating IR");
          programState = prompt;
        }

        if (endoExo[trialCount] == 0){
          // endogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            if (visual) digitalWrite(ledLeftCue, HIGH);
            else if (tactile) digitalWrite(vibLeftCue, HIGH);
          } else {
            if (visual) digitalWrite(ledLeftCue, LOW);
            else if (tactile) digitalWrite(vibLeftCue, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              if (visual) digitalWrite(ledRightStimulus, HIGH);
              else if (visual) digitalWrite(vibRightStimulus, HIGH);
              stimOn = millis();
              programState = waitRightPress;
            }
          }
        } else {
          // exogenous cue
          if (currentMillis - cueTurnOn <= cueLength){
            if (visual) digitalWrite(ledLeftStimulus, HIGH);
            else if (tactile) digitalWrite(vibLeftStimulus, HIGH);
          } else {
            if (visual) digitalWrite(ledLeftStimulus, LOW);
            else if (tactile) digitalWrite(vibLeftStimulus, LOW);
            if (currentMillis - previousMillis >= stimulusDelay) {
              previousMillis = currentMillis;
              if (visual) digitalWrite(ledRightStimulus, HIGH);
              else if (tactile) digitalWrite(vibRightStimulus, HIGH);
              stimOn = millis();
              programState = waitRightPress;
            }
          }
        }
      break;


      case waitLeftPress:
        // if user takes more than 5 seconds, start over
        if (millis() - stimOn >= 5000){
            Serial.println("You took too long. Press the button when you're ready to try again");
            // turn light off and set stimOn to zero so this loop doesn't repeat
            if (visual) digitalWrite(ledLeftStimulus, LOW);
            else if (tactile) digitalWrite(vibLeftStimulus, LOW);
            stimOn = 0;
            programState = programSetup;
          }
        // reacted within an appropriate amount of time and pressed the right button
        if (leftButtonState == 1){
          // keep track of how many valid vs invalid trials have been done
          if (cueNumber == 0) numValid--;
          else if (cueNumber == 2) numInvalid--;
          // reaction time is the current time minus the time the light turned on
          reactionTime = millis() - stimOn;
          Serial.println(reactionTime);
          // turn the light off
          if (visual) digitalWrite(ledLeftStimulus, LOW);
          else if (tactile) digitalWrite(vibLeftStimulus, LOW);
          // add to array
          reactionTimes[trialCount] = reactionTime;
          correctness[trialCount] = true;
          trialCount++;
          if (trialCount == nTrials/2){
            visual = false;
            tactile = true;
          }
          if (trialCount == nTrials) displayResults();
          // go back to beginning
          trialEnded = currentMillis;
          programState = begin;
        }
        // reacted within an appropriate amount of time but pressed the wrong button
        if (rightButtonState == 1){
          // keep track of how many valid vs invalid trials have been done
          if (cueNumber == 0) numValid--;
          else if (cueNumber == 2) numInvalid--;
          // reaction time is the current time minus the time the light turned on
          reactionTime = millis() - stimOn;
          Serial.println(reactionTime);
          // turn the light off
          if (visual) digitalWrite(ledLeftStimulus, LOW);
          else if (tactile) digitalWrite(vibLeftStimulus, LOW);
          // add to array
          reactionTimes[trialCount] = reactionTime;
          correctness[trialCount] = false;
          trialCount++;
          if (trialCount == nTrials/2){
            visual = false;
            tactile = true;
          }
          if (trialCount == nTrials) displayResults();
          // go back to beginning
          trialEnded = currentMillis;
          programState = begin;
        }
      break;

      case waitRightPress:
        if (millis() - stimOn >= 5000){
          Serial.println("You took too long. Press the button when you're ready to try again");
          if (visual) digitalWrite(ledRightStimulus, LOW);
          else if (tactile) digitalWrite(vibRightStimulus, LOW);
          stimOn = 0;
          programState = programSetup;
        }
        // reacted within an appropriate amount of time and pressed the right button
        if (rightButtonState == 1){
          // keep track of how many valid vs invalid trials have been done
          if (cueNumber == 1) numValid--;
          else if (cueNumber == 3) numInvalid--;
          reactionTime = millis() - stimOn;
          Serial.println(reactionTime);
          if (visual) digitalWrite(ledRightStimulus, LOW);
          else if (tactile) digitalWrite(vibRightStimulus, LOW);
          reactionTimes[trialCount] = reactionTime;
          correctness[trialCount] = true;
          trialCount++;
          if (trialCount == nTrials/2){
            visual = false;
            tactile = true;
          }
          if (trialCount == nTrials) displayResults();
          trialEnded = currentMillis;
          programState = begin;
        }
        // reacted within an appropriate amount of time but pressed the wrong button
        if (leftButtonState == 1){
          if (cueNumber == 1) numValid--;
          else if (cueNumber == 3) numInvalid--;
          reactionTime = millis() - stimOn;
          Serial.println(reactionTime);
          if (visual) digitalWrite(ledRightStimulus, LOW);
          else if (tactile) digitalWrite(vibRightStimulus, LOW);
          reactionTimes[trialCount] = reactionTime;
          correctness[trialCount] = false;
          trialCount++;
          if (trialCount == nTrials/2){
            visual = false;
            tactile = true;
          }
          if (trialCount == nTrials) displayResults();
          trialEnded = currentMillis;
          programState = begin;
        }
      break;

      default:
        Serial.println("Uh oh, let the experimenter know something bad happened");
      }
  }
}

// returns nothing, takes in nothing, just displays the results
void displayResults(void){
  float meanReactionTime = 0;
  Serial.print("All reaction times: ");
  Serial.println(" ");
  for (int i=0; i < nTrials; i++){
    // get the values at a particular trial's index
    int correctnessIndex = correctness[i];
    int cuesIndex = cues[i];
    int endoExoIndex = endoExo[i];
    // enumerate those values
    Serial.print(reactionTimes[i]);
    Serial.print(" ");
    Serial.print(correctnessDisplay[correctnessIndex]);
    Serial.print(" ");
    Serial.print(cuesDisplay[cuesIndex]);
    Serial.print(" ");
    Serial.println(endoExoDisplay[endoExoIndex]);
    meanReactionTime += (reactionTimes[i] / nTrials);
  }
  Serial.print("\nAverage reaction time: ");
  Serial.print(meanReactionTime);
  Serial.print("\n");
}

// originally via chatgpt, randomizes the items in an array by swapping their positions
// modified to only swap the elements after the trialCount, so that not everything
// will be swapped if the array was altered due to timing out
void randomizeArray(int arr[], int size) {
  for (int i = size - 1; i >= trialCount; i--) {
    int j = random(trialCount, i + 1); // Generate a random index between trialCount and i
    // Swap arr[i] with the element at the random index
    int temp = arr[i];
    arr[i] = arr[j];
    arr[j] = temp;
  }
}