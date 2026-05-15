// Example_8_MiscIdeas.ino
// I am going to use this as a base to explore some more things
// possible using ETL. 
// I am going to start with some things from test_function.cpp
// Some simple tests of etl/function
// This is deprecated - use etl/delegate instead.
// I now have a few tests for delegate as well
// The test results are at the end of the outputs.
// Adding  inplace_function as alternative to delegate.

// Example_4_StateMachine.ino
// Starting from state machine example on the web page.
// There are a few changes.
// I cannot yet compile on_event with a sender.
// I am adding some output.
// I have added a Fault message which is received as "unknown"

// make sure that we do not rely on the STL
#define ETL_NO_STL

#include "Embedded_Template_Library.h"
#include "etl/queue.h"
#include "etl/fsm.h"
#include "etl/message_packet.h"
#include "etl/function.h"
#include "etl/delegate.h"
#include "etl/functional.h"
// Only in 20.45.0 not yet available in the Arduino library....
//#include "etl/inplace_function.h"

  //*****************************************************************************
  // Tests for etl/function
  //*****************************************************************************
  const int VALUE = 1;
  bool function_called = false;
  bool parameter_correct = false;


  //*****************************************************************************
  // Object data structure.
  //*****************************************************************************
  struct Data
  {
    int d;
  };

  //*****************************************************************************
  // Call the function taking no parameters.
  //*****************************************************************************
  void call(etl::ifunction<void>& function)
  {
    function();
   }

  //*****************************************************************************
  // Call the const function taking no parameters.
  //*****************************************************************************
  void call(const etl::ifunction<void>& function)
  {
    function();
  }

  //*****************************************************************************
  // The free function taking no parameters.
  //*****************************************************************************
  void free_void()
  {
    function_called = true;
    Serial.println("function free_void called");
   }

  //*****************************************************************************
  // The free function taking an int parameter.
  //*****************************************************************************
  void free_int(int i)
  {
    function_called = true;
    parameter_correct = (i == VALUE);
  }

 //*****************************************************************************
  // The free function taking a Data reference parameter.
  //*****************************************************************************
  void free_reference(const Data& data)
  {
    function_called = true;
    parameter_correct = (data.d == VALUE);
  }


  //*****************************************************************************
  // The test class with member functions.
  //*****************************************************************************
  class ObjectFunction
  {
  public:

    void member_void()
    {
      function_called = true;
    }

    void member_int(int i)
    {
      function_called = true;
      parameter_correct = (i == VALUE);
    }

    void member_reference(const Data& data)
    {
      function_called = true;
      parameter_correct = (data.d == VALUE);
    }
  };

  ObjectFunction test_static_function;


void test_functions()
{
  {
    etl::function<void, void> function(free_void);

    call(function);
  }

  {
    const etl::function<void, void> function(free_void);

    call(function);
  }

}

  //*****************************************************************************
  // Tests for etl/delegate - see https://www/etlcpp.com/delegate.html
  //*****************************************************************************
 
class Test {
  public:
    int member_function(int);
    static int member_static(int);
    int member_function2(int,int);
};

int Test::member_function(int x) { Serial.print("Test = "); Serial.println(x);return x;}
int Test::member_function2(int x, int y) 
{ Serial.print("Test2 = "); Serial.println(x + y);return x + y;}


Test test;

int global(int n) { Serial.print("global called with "); Serial.println(n); return n; }

auto lambda = [](int i) { Serial.print("lambda called with "); Serial.println(i); return i; };

void test_delegates()
{
  auto d = etl::delegate<int(int)>::create<global>();
  d(1);

  auto dd = etl::delegate<int(int)>::create(lambda);
  dd(2);

  auto dt = etl::delegate<int(int)>::create<Test, &Test::member_function>(test);
  dt(3);
  auto dt2 = etl::delegate<int(int,int)>::create<Test, &Test::member_function2>(test);
  dt2(1,3);
}

  //*****************************************************************************
  // Tests for etl/functional - see https://www/etlcpp.com/functional.html
  // The web page lacks examples so I have looked at test_functional.cpp as well.
  //*****************************************************************************

namespace { 

  template <typename TCompare>
  bool compare(int a, int b)
  {
    return TCompare()(a, b);
  }

  struct testf : etl::binary_function<int, int, bool>
  {
    bool operator()(int a, int b) const
    {
      return a < b;
    }
  };

}

void test_functional() {

  if(compare<etl::less<int> >(1,2) ) Serial.println("compare<less> is true");

  if(etl::bind1st(testf(),1)(2) ) Serial.println("etl::bind1st(testf(),1)(2) is true");
  if(etl::bind2nd(testf(),2)(1) ) Serial.println("etl::bind2nd(testf(),2)(1) is true");

}

const etl::message_router_id_t MOTOR_CONTROL = 0;
const etl::message_router_id_t ANOTHER_SYSTEM = 1; // Messages should be ignored

//***************************************************************************
// Events
struct EventId
{
  enum
  {
    START,
    STOP,
    STOPPED,
    SET_SPEED,
    FAULT
  };
};

//***********************************
class Start : public etl::message<EventId::START>
{
};

//***********************************
class Stop : public etl::message<EventId::STOP>
{
public:

  Stop() : isEmergencyStop(false) {}
  Stop(bool emergency) : isEmergencyStop(emergency) {}

  const bool isEmergencyStop;
};

//***********************************
class SetSpeed : public etl::message<EventId::SET_SPEED>
{
public:

  SetSpeed(int speed) : speed(speed) {}

  const int speed;
};

//***********************************
class Stopped : public etl::message<EventId::STOPPED>
{
};

//***********************************
class Fault : public etl::message<EventId::FAULT>
{
};

//***************************************************************************
// States
struct StateId
{
  enum
  {
    IDLE,
    RUNNING,
    WINDING_DOWN,
    NUMBER_OF_STATES
  };
};

//***********************************
// Another system
//***********************************
class AnotherSystem : public etl::fsm
{
public:
  AnotherSystem()
    : fsm(ANOTHER_SYSTEM)
  {
  }

  void LogUnknownEvent(const etl::imessage& msg) /* added const */
  {
    Serial.print("Received unknown message "); 
    Serial.println(msg.get_message_id());
    Serial.flush();
  }

};

//***********************************
// The motor control FSM.
//***********************************
class MotorControl : public etl::fsm
{
public:

  MotorControl()
    : fsm(MOTOR_CONTROL)
  {
  }

  void SetRunningLampOn()
  {
  }

  void SetRunningLampOff()
  {
  }

  void SetEmergencyLampOn()
  {
  }

  // Added for consistency
  void SetEmergencyLampOff()
  {
  }

  void SpeedChangeWarning()
  {
  }

  void LogUnknownEvent(const etl::imessage& msg) /* added const */
  {
    Serial.print("Received unknown message "); 
    Serial.println(msg.get_message_id());
    Serial.flush();
  }
};

//***********************************
// The idle state.
// Accepts Start events.
//***********************************
class Idle : public etl::fsm_state<MotorControl, Idle, StateId::IDLE, Start>
{
public:

  //***********************************
  etl::fsm_state_id_t on_event(const Start& event)
  {
    Serial.print("Received Start event "); 
    Serial.println(event.get_message_id());
    Serial.flush();

    return StateId::RUNNING;
  }

  //***********************************
  etl::fsm_state_id_t on_event_unknown(const etl::imessage& event)
  {
    get_fsm_context().LogUnknownEvent(event);

    return STATE_ID;
  }
};

//***********************************
// The running state.
// Accepts Stop and SetSpeed events.
//***********************************
class Running : public etl::fsm_state<MotorControl, Running, StateId::RUNNING, Stop, SetSpeed>
{
public:

  //***********************************
  etl::fsm_state_id_t on_event(const Stop& event)
  {
    Serial.print("Received Stop event "); 
    Serial.println(event.get_message_id());
    Serial.flush();

    if (event.isEmergencyStop)
    {
      get_fsm_context().SetEmergencyLampOn();     

      return StateId::IDLE;
    }
    else
    {
      return StateId::WINDING_DOWN;
    }
  }
 
//***********************************
  etl::fsm_state_id_t on_event(const SetSpeed& event)
  {
    Serial.print("Received Set Speed event "); 
    Serial.println(event.get_message_id());
    Serial.flush();
  
    get_fsm_context().SpeedChangeWarning();

    return STATE_ID;
  }

  //***********************************
  etl::fsm_state_id_t on_event_unknown(const etl::imessage& event)
  {
    get_fsm_context().LogUnknownEvent(event);

    return STATE_ID;
  }

  //***********************************
  etl::fsm_state_id_t on_enter_state()
  {
    get_fsm_context().SetRunningLampOn();

    return STATE_ID;
  }

  //***********************************
  void on_exit_state()
  {
    get_fsm_context().SetRunningLampOff();

    //return STATE_ID;
  }
};

//***********************************
// The winding down state.
// Accepts Stopped events.
//***********************************
class WindingDown : public etl::fsm_state<MotorControl, WindingDown, StateId::WINDING_DOWN, Stopped>
{
public:

  //*********************************** on_event with sender does not compile.
  // This is interesting as the web page documents a missing argument sender!
  etl::fsm_state_id_t on_event(/*etl::imessage_router& sender,*/ const Stopped& event)
  {
    Serial.print("Received Stopped event "); 
    Serial.println(event.get_message_id());
    Serial.flush();
  return StateId::IDLE;
  }

  //***********************************
  etl::fsm_state_id_t on_event_unknown(/*etl::imessage_router& sender,*/ const etl::imessage& event)
  {
    get_fsm_context().LogUnknownEvent(event);

    return STATE_ID;
  }
};

// The states.
Idle        idle;
Running     running;
WindingDown windingDown;

// The states must be in state id order.
etl::ifsm_state* stateList[StateId::NUMBER_OF_STATES] =
{
  &idle, &running, &windingDown
};

 
  
String board(ARDUINO_BOARD);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Embedded Template Library Example 8 Motor Finite State Machine and function calls");
  Serial.print("running on ");
  Serial.println(board);

// The states.
Idle        idle;
Running     running;
WindingDown windingDown;

// The states must be in state id order.
etl::ifsm_state* stateList[StateId::NUMBER_OF_STATES] =
{
  &idle, &running, &windingDown
};

// The FSM.
MotorControl motorControl;
AnotherSystem anotherSystem;

// Set the FSM's state list - this is essential.
anotherSystem.set_states(stateList, etl::size(stateList));

anotherSystem.start();

// Set the FSM's state list
motorControl.set_states(stateList, etl::size(stateList));

// Start the motor. The first state is 'IDLE'.
motorControl.start();

// Receive a Start event. The next state is 'RUNNING'.
motorControl.receive(Start());

// Receive a SetSpeed event. The state is still 'RUNNING'.
motorControl.receive(SetSpeed(100));

// Receive a Stop event. The next state is 'WINDING_DOWN'.
// MOTOR_CONTROL is optional here.
motorControl.receive(MOTOR_CONTROL,Stop());

// Receive a Stopped event. The next state is 'IDLE'.
motorControl.receive(Stopped());

// Receive a Start event. The next state is 'RUNNING'.
motorControl.receive(Start());

// This is received as unknown message.
motorControl.receive(Fault());

// This is ignored
motorControl.receive(ANOTHER_SYSTEM,Fault());

// Output now from this.
anotherSystem.receive(ANOTHER_SYSTEM,Fault());

// Receive a Stop(emergency) event. The next state is 'IDLE'.
motorControl.receive(Stop(true));

  Serial.println("testing functions and delegates");
  test_functions();
  test_delegates();
  test_functional();
  Serial.println("At end of processing");

}

void loop()
{
  
}
