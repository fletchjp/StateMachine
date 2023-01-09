// arduino_state_blink_moustache2.ino
// This is a reduced example to compare the memory use with arduino_state_blink.

// arduino_state_blink with moustache output
// Simple example using the StateMachine library.
// This also shows how the moustache library can be used.
// moutache_size is something I have invented.
// There are a number of new moustache functions now in moustache_code.h

/*
 * Converting example from 
 * https://arduinoplusplus.wordpress.com/2019/07/06/finite-state-machine-programming-basics-part-1/
 */

#include <StateMachine.h>
#include <moustache.h>
#include <moustache_code.h>

//
const int STATE_DELAY = 1000;
const int LED = 13;

StateMachine machine = StateMachine();
// Define the formats to be used to output a state.
//const char *available_state = "State {{state}} is available";
const char *current_state = "Current state is {{state}}";
//const char *position = "moveServo reaches {{pos}}";
//const char *point = " ( {{x}} , {{y}} ) ";

//const moustache_variable_t test_error[] ={ {"error", "test_error" } };
// Definitions for each state.
const moustache_variable_t state0[] = { {"state", "0, reset"} };
const moustache_variable_t state1[] = { {"state", "1, blink"} };
const moustache_variable_t state2[] = { {"state", "2, wait"} };
// Array of states (optional)  
const moustache_variable_t *states_array[]= { state0, state1, state2 };

//int pos = 0;    // variable to store the servo position
//const moustache_variable_t position_value[] = { { "pos", String(pos) } };

//double x = 0, y = 0;
//const moustache_variable_t point_value[] = { { "x", String(x) }, { "y", String(y) } };

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
    //Serial.println("State 0, reset");
    //const moustache_variable_t what[] =  { *states_array[0] };    
    //Serial.println(moustache_render(current_state,what));
    Serial.println(moustache_render_array(current_state,states_array,0));
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
      //Serial.println("State 1, blink");
      size_t i = 1;
      //const moustache_variable_t what[] =  { *states_array[i] };    
      //Serial.println(moustache_render(current_state,what));
      Serial.println(moustache_render_array(current_state,states_array,1));
      digitalWrite(LED, !digitalRead(LED));
    }
});

// Wait
State* S2 = machine.addState([]() {
    if(machine.executeOnce){
      Blink_State = WAIT;
      //Serial.println("State 2, wait");
      //const moustache_variable_t what[] =  { *states_array[2] };    
      Serial.println(moustache_render_array(current_state,states_array,2));
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
  Serial.println("Arduino state blink");
  //size_t n = moustache_size(states_array);
  //Serial.println("List of available states");
  //for (size_t i = 0; i < n; i++ ) {
  //  const moustache_variable_t what[] =  { *states_array[i] };
  //  Serial.println(moustache_render(available_state,what)); 
  //}
  //Serial.println("Demo of range error");
  //Serial.println(moustache_render_array(current_state,states_array,3));
  //Serial.println("Start state machine");
  // Add transition from S0 to S1
  S0->addTransition(&transitionS0S1,S1);
  // Add transitions from S1 to S2 and back again.
  S1->addTransition(&transitionS1S2,S2);
  S2->addTransition(&transitionS2S1,S1);

  //Serial.println(moustache_render(position,position_value));
  // This does now change the output.
  //pos = 2;
  //position_value[0].value = String(pos);
  //Serial.println(moustache_render(position,position_value));
  //pos = 3;
  //moustache_value(position_value,0,pos);
  //Serial.println(moustache_render(position,position_value));
  //pos = 4;
 
  //Serial.println(moustache_render_value(position,position_value,0,pos));
  //Serial.println("Test of value range error");
  //Serial.println(moustache_render_value(position,position_value,1,pos));

  //Serial.println(moustache_render(point,point_value));
  //x = 3;
  //y = 4;  
  //point_value[0].value = String(x);
  //point_value[1].value = String(y);
  //Serial.println(moustache_render(point,point_value));
  //moustache_value(point_value,0,y);
  //moustache_value(point_value,1,x);
  //Serial.println(moustache_render(point,point_value));
 
  //Serial.println(moustache_render_value2(point,point_value,0,x,1,y));

  //Serial.println(moustache_render_value2(point,point_value,2,x,1,y));
  //Serial.println(moustache_render_value2(point,point_value,0,x,3,y));

  //Serial.println(moustache_render(moustache_error,test_error));

 }

void loop() {
  // put your main code here, to run repeatedly:
  machine.run();
  //delay(STATE_DELAY);
}
