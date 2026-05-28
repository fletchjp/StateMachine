// StateMachine_R4.ino
// Bring across the StateMachine code from the Raspberry Pi 5.
// This is the example I have had working on the Raspberry Pi 5.

// I want now to have an equivalent for the UNO R3 or MEGA 
// using the embedded template library.

// The problem with this style of code is that for N event types and M states
// there are N*M lambdas to be written.

// This is a C++17 file from the work of Tamir Bahar
// https://tamir.dev/posts/a-functional-style-state-macnine-in-cpp
// https://tamir.dev/posts/a-functional-style-state-macnine-in-cpp-part2
// cppreference has this as C++26.
// No that is extension as member function.

// I have extended the example slightly to use the Context to report
// the current state based on the last event processed.

#include <variant>
#include <vector>
#include <tuple> // Needed for std::tie
#include <Streaming.h>

/////////////////////////////////////////////////////////////
// StateMachine code here.
/////////////////////////////////////////////////////////////

#include "StateMachine.hpp"

/////////////////////////////////////////////////////////////
// Application specific code here.
/////////////////////////////////////////////////////////////

// Events are part of the user code.
// Define an event such that multiplication is idempotent.
// Wierd idea based on Clifford Algebra.
// Two successive events of the same type have the same effect as one.
// Pushing 4 for floor four twice does not give me floor 8.
struct EventA {
	const char* msg{nullptr};
	EventA mult(const EventA& e) { return *this; }
};

struct EventB {
	int number{0};
	// Multiplying the numbers as an example
	EventB mult(const EventB& e) { number *= e.number; return *this; }
};

using Event = std::variant<EventA, EventB>;

// Context may change with the user code.
// In this example I have added a single char to indicate the state.
// This is passed in as an argument to the Inc function.
// The state does not hold information on its state.
struct Context {
	Context Inc(char b) const {
		return Context{counter + 1, cstate = b};
	}
	int counter = 0;
	mutable char cstate {'X'};  // Initial value
};

// A std::pair can be decoded by std::tie.
// This is not true for etl which needs etl::tuple.
template <class T>
using PairWithCtx = std::pair<T, const Context>;

using State = SelfReturning<PairWithCtx>::WithArgs<const Context&, Event>;

State::RetType A(const Context&, Event);
State::RetType B(const Context&, Event);

State::RetType A(const Context& ctx, Event evt) {
	Serial << "State A, counter = " << ctx.counter << endl;
	return std::visit(overloaded {
		[&] (EventA e) {
			if (e.msg != nullptr) {
				Serial << "A message = " << e.msg << endl;
			} else {
				Serial << "A message = nullptr" << endl; 
			}
			return make_pair(A, ctx);
		},
		[&] (EventB) { return make_pair(B, ctx.Inc('B')); }
	}, evt);
}

State::RetType B(const Context& ctx, Event evt){
	Serial << "State B, counter = " << ctx.counter << endl;
	return std::visit(overloaded {
		[&] (EventA) { return make_pair(A, ctx.Inc('A')); },
		[&] (EventB e) {
			Serial << "B number = " << e.number << endl;
			return make_pair(B, ctx);
		}
	}, evt);
}


// Moved to be global to be accessible in the loop
// This gives the initial state.
State state = A;
Context ctx{};



void setup() {
  // put your setup code here, to run once:
    Serial.begin(115200);
    delay(5000);
#ifdef ARDUINO_ARCH_RP2040
    Serial.println("Running tests on a Pico");
#endif
#ifdef ARDUINO_MINIMA
    Serial.println("\nArduino R4 Minima");
#endif
#ifdef ARDUINO_UNOR4_WIFI
    Serial.println("\nArduino R4 Wifi");
#endif
    //pinMode(LED,OUTPUT);
    printVersionInfo();
    Serial.print("C++ version is ");
    Serial.println(__cplusplus);
    Serial.println("Arduino R4 State Machine test");

    Serial << "========================================" << endl;
	  Serial << "StateMachine from example by Tamir Bahar" << endl;
	  Serial << "Some code moved to file StateMachine.hpp" << endl;
    Serial << "========================================" << endl;
	  Event events[] = {EventA{"Starting"},EventA{},EventB{},EventB{2},EventB{10},
		              EventA{},EventA{"Hello World"},EventB{},EventB{20},
									EventA{},EventB{},EventB{25}};
		Serial << "Initial situation" << endl;
		ctx.cstate = 'A'; // Set initial state in ctx.
	  Serial << "Context " << ctx.cstate << " has counter = " << ctx.counter << endl;
    for (auto evt : events) {
		   // This unpacks the std::pair returned.
		   std::tie(state, ctx) = state(ctx, evt);
	  }
    Serial << "Context " << ctx.cstate << " has counter = " << ctx.counter << endl;
    Serial << "Note that the event data is ignored when the event type changes." << endl;
		// Entering data from the Serial monitor.
		Serial << "Enter an integer to pass to an EventB" << endl;
		Serial.setTimeout(10000);
		int b = Serial.parseInt(); //delay(1000);
		Serial << "Read number " << b << endl;
		auto evt = EventB{b};
		auto evt2 = EventB{b+1};
		std::tie(state,ctx) = state(ctx,evt);
    Serial << "Context " << ctx.cstate << " has counter = " << ctx.counter << endl;
		Serial << "Multiplying events" << endl;
		Serial << "evt2.mult(evt).number = " << evt2.mult(evt).number << endl;
		Serial << "After tests" << endl;
}

//
// A function to print information about the file being compiled.
//
void printVersionInfo(void) {
  Serial.println(__FILE__);
  Serial.print("Compiled on ");
  Serial.print(__DATE__);
  Serial.print(" at ");
  Serial.print(__TIME__);
  Serial.print(" with IDE version ");
  Serial.println(ARDUINO);
}

void loop() {
  // put your main code here, to run repeatedly:

}
