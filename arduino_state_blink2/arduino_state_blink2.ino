// arduino_state_blink
// Simple example using the StateMachine library.

/*
 * Converting example from 
 * https://arduinoplusplus.wordpress.com/2019/07/06/finite-state-machine-programming-basics-part-1/
 */

#include <StateMachine.h>

const int STATE_DELAY = 1000;
const int LED = 13;

StateMachine machine = StateMachine();

/***********************************************************
 * What do we mean by state?
 * Do we mean the state of the LED (On/Off)
 * or the state of the process (Blinking/waiting)?
 * I think that is up to the user.
 ***********************************************************/


// Global variables for the actual state.

enum { LED_off, LED_on } Led_State;

enum { RESET, BLINK, WAIT } Blink_State;

uint32_t timeLastTransition = 0;
 
/*
 * States for the blink_state machine.
 */

// Reset and wait
State* S0 = machine.addState([]() {
  if(machine.executeOnce){
    Serial.println(String("State 0, reset"));
    digitalWrite(LED,LOW);
    Blink_State = RESET;
    Led_State = LED_off;
    timeLastTransition = millis();
  }
});

// Blink
State* S1 = machine.addState([]() {
    if(machine.executeOnce){
      Blink_State = BLINK;
      Serial.println(String("State 1, blink"));
      digitalWrite(LED, !digitalRead(LED));
    }
});

// Wait
State* S2 = machine.addState([]() {
    if(machine.executeOnce){
      Blink_State = WAIT;
      Serial.println(String("State 2, wait"));
      timeLastTransition = millis();
    }
});

/*
 * Transitions between states for the blink_state machine.
 */

bool transitionS0S1(){
    if (millis() - timeLastTransition >= STATE_DELAY) {
      return true;
    }
    return false;
}

// Go straight away to the wait state.
bool transitionS1S2(){
    return true;
}

bool transitionS2S1(){
    if (millis() - timeLastTransition >= STATE_DELAY) {
      return true;
    }
    return false;
}




void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while(!Serial) { }
  pinMode(LED,OUTPUT);
  Serial.println(String("Arduino state blink"));
  
  // Add transition from S0 to S1
  S0->addTransition(&transitionS0S1,S1);
  // Add transitions from S1 to S2 and back again.
  S1->addTransition(&transitionS1S2,S2);
  S2->addTransition(&transitionS2S1,S1);


}

void loop() {
  // put your main code here, to run repeatedly:
  machine.run();
  //delay(STATE_DELAY);
}
