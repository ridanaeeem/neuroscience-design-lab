// Print the message
// Read button value
// Read value (pushed or not)
// Keep track of the change and when it switches to not, move on
// Get a random number for LED
// Wait that long
// Light on
// millis to record seconds until button pushed again
// If they take more than 5 seconds (5000) then end trial (turn off light, wait for push again) and start over

// button and led pins for testing
int pushButton = 2;
int testPin = 13;

// static LED pin and its variables
int staticPin = 7;
int ledState = LOW;
unsigned long previousMillis = 0;
const long interval = 1000;

// reaction time variables
typedef enum{
  prompt,
  waitForPress,
  startPress,
  experimentStart,
  reactionMeasure
} state_defs;
state_defs programState;

int randomNumber = random(3000); 
int nTrials = 10;
unsigned long reactionTimes[10] = {0};
int trialCount = 0;


// test LED variables
// how long the light has been on
unsigned long lightOn = 0;
// how long it took the person to react
unsigned long reactionTime = 0;
// last time test light was turned on
unsigned long previousTestMillis = 0;

void setup() {
  Serial.begin(9600);
  // set button as input, LEDs as output
  pinMode(pushButton, INPUT);
  pinMode(testPin, OUTPUT);
  pinMode(staticPin, OUTPUT);
  programState = prompt;
  randomSeed(analogRead(0));
}

void loop() {
  if (trialCount < nTrials){
    // static pin stuff - blink without delay demo
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
      digitalWrite(staticPin, ledState);
    }

    // code for the pin we're testing
    // read the button
    int buttonState = digitalRead(pushButton);

    // switch cases
    switch (programState){
      // prompt to user to click button when they're ready to begin
      case prompt:
        if (buttonState == 0){
          // in case they were cheating and it was on before
          // the light will be off now
          digitalWrite(testPin, LOW);
          // message for starting the experiment
          Serial.println("Press the button when you're ready to begin");
          programState = waitForPress;
        }
        break;
      
      // wait for them to press the button
      case waitForPress:
        if (buttonState == 1){
          programState = startPress;
      }
      break;

      // button has been pressed, when they unpress it move on
      case startPress:
        if (buttonState == 0){
          programState = experimentStart;
          randomNumber = random(3000,5000);
          previousTestMillis = currentMillis;
        }
      break;

      // wait for LED to turn on or catch them cheating
      case experimentStart:
        // holding button down when they should have been waiting
        // for the light to turn = cheating
        if (buttonState == 1){
          Serial.println("Stop cheating");
          // digitalWrite(testPin, LOW);
          programState = prompt;
        }
        // enough time has elapsed to turn the light on, move on
        if (currentMillis - previousTestMillis >= randomNumber) {
          previousTestMillis = currentMillis;
          digitalWrite(testPin, HIGH);
          lightOn = millis();
          programState = reactionMeasure;
        }
      break;

      // start measuring reaction time
      case reactionMeasure:
        // 5 seconds max before we restart
        if (millis() - lightOn >= 5000){
          Serial.println("You took too long, try again!");
          // turn light off and set lightOn to zero so this loop doesn't repeat
          digitalWrite(testPin, LOW);
          lightOn = 0;
          programState = prompt;
        }
        // reacted within an appropriate amount of time, keep track of reaction time
        if (buttonState == 1){
          // reaction time is the current time minus the time the light turned on
          reactionTime = millis() - lightOn;
          // turn the light off
          digitalWrite(testPin, LOW);
          // print the reaction time in milliseconds
          // Serial.println(reactionTime);
          // add to array
          reactionTimes[trialCount] = reactionTime;
          trialCount++;
          if (trialCount == nTrials) displayResults();
          // go back to beginning
          programState = startPress;
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
  for (int i=0; i < nTrials; i++){
    Serial.print(reactionTimes[i]);
    Serial.print(" ");
    meanReactionTime += (reactionTimes[i] / nTrials);
  }
  Serial.print("\nAverage reaction time: ");
  Serial.print(meanReactionTime);
  Serial.print("\n");
}
