// Example_6_HierarchicalStateMachine_test.ino
// I have found in the tests the same example done by the person who did the extension!
// So this is my adoption of the test code test_hfsm to be found at this location:
// https://github.com/ETLCPP/etl/tree/master/test
// I am adapting and not running the test software.
// Some of the naming is not consistent with the test code as I have reused example 4 code.
// I have put in a lot of printouts so that the sequence of operation can be followed.
// There are some new features in this code 
// e.g. ETL ENUM types associating a string with an entry. I have not found an example of use (yet).
//////////////////////////////////////////////////////////////
// See this for a discussion of the implementation when hfsm was being added.
// https://github.com/ETLCPP/etl/pull/374
// One conclusion I have come to is to use hfsm only when needed, and fsm otherwise.
//////////////////////////////////////////////////////////////

// Example_5_HierarchicalStateMachine.ino - Currently not functional
// Starting from hierarchical state machine example on the web page.
// https://www.etlcpp.com/hfsm.html
// There is no example - only some sketch for code.
// I am having to work from the diagram to decide which states respond to which events.
// I have the code running at the top level.
// I have not found how to get the child events to work.
// The documentation is wrong about using Initialise to get the motorControl set up.
// In fact in the test example Initialise is defined inside the class!!
///////////////////////////////////////////////////////////////////////////////////
// Note on setting up an HFSM. See the declaration on line 185.
// The class has to inherit from hfsm instead of fsm.
// The hfsm constructor needs an enum from the struct HFsmId 
// which is not documented anywhere I have found.
// See line 46 for how to do this.
///////////////////////////////////////////////////////////////////////////////////

// I am using Example_4 as a starting point as the problem is similar.
// There are a few changes in the example 4 case.
// I cannot yet compile on_event with a sender.
// I am adding some output.
// I have added a Fault message which is received as "unknown"

// make sure that we do not rely on the STL
#define ETL_NO_STL

#include "Embedded_Template_Library.h"
#include "etl/queue.h"
#include "etl/hfsm.h"
#include "etl/packet.h"
#include "etl/queue.h"
#include "etl/message_packet.h"
#include "etl/enum_type.h"

const etl::message_router_id_t MOTOR_CONTROL = 0;  // This is not used in this example.
const etl::message_router_id_t ANOTHER_SYSTEM = 1; // Messages should be ignored

// *************************************************************************
// This is NOT in the documentation!!
struct HFsmId
{
  enum
  {
    Motor_Control
  };

};

//***************************************************************************
// Events
struct EventId
{
  enum enum_type
  {
    START,
    STOP,
    ESTOP,
    STOPPED,
    SET_SPEED,
    RECURSIVE,
    TIMEOUT,
    UNSUPPORTED,
    FAULT
  };

  ETL_DECLARE_ENUM_TYPE(EventId, etl::message_id_t)
  ETL_ENUM_TYPE(START,       "Start")
  ETL_ENUM_TYPE(STOP,        "Stop")
  ETL_ENUM_TYPE(ESTOP,       "E-Stop")
  ETL_ENUM_TYPE(STOPPED,     "Stopped")
  ETL_ENUM_TYPE(SET_SPEED,   "Set Speed")
  ETL_ENUM_TYPE(RECURSIVE,   "Recursive")
  ETL_ENUM_TYPE(TIMEOUT,     "Timeout")
  ETL_ENUM_TYPE(UNSUPPORTED, "Unsupported")
  ETL_ENUM_TYPE(FAULT,       "Fault")
  ETL_END_ENUM_TYPE
};

//***********************************
class Start : public etl::message<EventId::START>
{
};

//***********************************
class Stop : public etl::message<EventId::STOP>
{
public:

//  Stop() : isEmergencyStop(false) {}
//  Stop(bool emergency) : isEmergencyStop(emergency) {}

//  const bool isEmergencyStop;
};

//***********************************
class EStop : public etl::message<EventId::ESTOP>
{
};

//***********************************
class SetSpeed : public etl::message<EventId::SET_SPEED>
{
public:

  SetSpeed(int speed_) : speed(speed_) {}

  const int speed;
};

//***********************************
class Stopped : public etl::message<EventId::STOPPED>
{
};

//***********************************
class Recursive : public etl::message<EventId::RECURSIVE>
{
};

//***********************************
class TimeOut : public etl::message<EventId::TIMEOUT>
{
};

//***********************************
class Unsupported : public etl::message<EventId::UNSUPPORTED>
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
  enum enum_type
  {
    IDLE,
    RUNNING,
    WINDING_UP,
    WINDING_DOWN,
    AT_SPEED,
    NUMBER_OF_STATES
  };

  ETL_DECLARE_ENUM_TYPE(StateId, etl::fsm_state_id_t)
  ETL_ENUM_TYPE(IDLE,         "Idle")
  ETL_ENUM_TYPE(RUNNING,      "Running")
  ETL_ENUM_TYPE(WINDING_UP,   "Winding Up")
  ETL_ENUM_TYPE(WINDING_DOWN, "Winding Down")
  ETL_ENUM_TYPE(AT_SPEED,     "At Speed")
  ETL_END_ENUM_TYPE
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
// The motor control HFSM.
//***********************************
class MotorControl : public etl::hfsm
{
public:

  MotorControl()
    : hfsm(HFsmId::Motor_Control)  // Was MOTOR_CONTROL when an fsm
  {
 }

  //*****************************************************
  void Initialise(etl::ifsm_state** p_states, size_t size)
  {
    set_states(p_states, size);
    ClearStatistics();
  }

  //*****************************************************
  void ClearStatistics(){
    // Leave this for now
    isLampOn = false;
    speed = 0;
  }

  void SetSpeedValue(int speed_)
  {
    speed = speed_;
  }
 
  void SetRunningLampOn()
  {
    isLampOn = true;
  }

  void SetRunningLampOff()
  {
    isLampOn = false;
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

  template <typename T>
  void queue_recursive_message(const T& message)
  {
    messageQueue.emplace(message);
  }

  void LogUnknownEvent(const etl::imessage& msg) /* added const */
  {
    Serial.print("Received unknown message "); 
    Serial.println(msg.get_message_id());
    Serial.flush();
  }

  typedef etl::largest<Start, Stop, EStop, SetSpeed, Stopped, Recursive, TimeOut> Largest_t;

  typedef etl::packet<etl::imessage, Largest_t::size, Largest_t::alignment> Packet_t;

  etl::queue<Packet_t, 2> messageQueue;
  
  bool isLampOn;
  int speed;

};

//***********************************
// The idle state.
// Accepts Start events.
//***********************************
class Idle : public etl::fsm_state<MotorControl, Idle, StateId::IDLE, Start, Recursive>
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
    etl::fsm_state_id_t on_event(const Recursive&)
    {
      get_fsm_context().queue_recursive_message(Start());
      return StateId::IDLE;
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
    Serial.println("Idle: on entering state");
    return STATE_ID;
  }

  //***********************************
  void on_exit_state()
  {
    //get_fsm_context().SetRunningLampOff();
    Serial.println("Idle: on exiting state");
    //return STATE_ID;
  }
};

//***********************************
// The running state.
// Accepts EStop events.
// On entering it immediately calls WindingUp and I cannot find out why.
//***********************************
class Running : public etl::fsm_state<MotorControl, Running, StateId::RUNNING, EStop>
{
public:

  //***********************************
  etl::fsm_state_id_t on_event(const EStop& event)
  {
    Serial.print("Received E-Stop event "); 
    Serial.println(event.get_message_id());
    Serial.flush();

    return StateId::IDLE;
/*
    if (event.isEmergencyStop)
    {
      get_fsm_context().SetEmergencyLampOn();     

      return StateId::IDLE;
    }
    else
    {
      return StateId::WINDING_DOWN;
    }
  */

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
    Serial.println("Running: on entering state");
    return STATE_ID;
  }

  //***********************************
  void on_exit_state()
  {
    get_fsm_context().SetRunningLampOff();
    Serial.println("Running: on exiting state");
    //return STATE_ID;
  }
};

//***********************************
// The winding up state.
// Accepts Start events.
//***********************************
class WindingUp : public etl::fsm_state<MotorControl, WindingUp, StateId::WINDING_UP, TimeOut, Stop>
{

public:
  //*********************************** on_event with sender does not compile.
  // This is interesting as the web page documents a missing argument sender!
  etl::fsm_state_id_t on_event(/*etl::imessage_router& sender,*/ const Stop& event)
  {
    Serial.print("Received Stop event "); 
    Serial.println(event.get_message_id());
    Serial.flush();
  return StateId::WINDING_DOWN;
  }

  //*********************************** on_event with sender does not compile.
  // This is interesting as the web page documents a missing argument sender!
  etl::fsm_state_id_t on_event(/*etl::imessage_router& sender,*/ const TimeOut& event)
  {
    Serial.print("Received TimeOut event "); 
    Serial.println(event.get_message_id());
    Serial.flush();
  return StateId::AT_SPEED;
  }

 //***********************************
  etl::fsm_state_id_t on_event_unknown(/*etl::imessage_router& sender,*/ const etl::imessage& event)
  {
    get_fsm_context().LogUnknownEvent(event);

    return STATE_ID;
  }

  //***********************************
  etl::fsm_state_id_t on_enter_state()
  {
    get_fsm_context().SetRunningLampOn();
    Serial.println("Winding up: on entering state");
    return STATE_ID;
  }

   //***********************************
  void on_exit_state()
  {
    //get_fsm_context().SetRunningLampOff();
    Serial.println("Winding up: on exiting state");
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
  //***********************************
  etl::fsm_state_id_t on_enter_state()
  {
    get_fsm_context().SetRunningLampOn();
    Serial.println("Winding down: on entering state");
    return STATE_ID;
  }

   //***********************************
  void on_exit_state()
  {
    //get_fsm_context().SetRunningLampOff();
    Serial.println("Winding down: on exiting state");
    //return STATE_ID;
  }

};

//***********************************
// The at speed state.
// Accepts Stop events.
//***********************************
class AtSpeed : public etl::fsm_state<MotorControl, AtSpeed, StateId::AT_SPEED, SetSpeed, Stop>
{
public:

  //*********************************** on_event with sender does not compile.
  // This is interesting as the web page documents a missing argument sender!
  etl::fsm_state_id_t on_event(/*etl::imessage_router& sender,*/ const Stop& event)
  {
    Serial.print("Received Stop event "); 
    Serial.println(event.get_message_id());
    Serial.flush();
  return StateId::WINDING_DOWN;
  }

  //***********************************
  etl::fsm_state_id_t on_event(/*etl::imessage_router& sender,*/ const SetSpeed& event)
  {
    Serial.print("Received Set Speed event "); 
    Serial.println(event.get_message_id());
    Serial.flush();
  return No_State_Change;
  }

  //***********************************
  etl::fsm_state_id_t on_event_unknown(/*etl::imessage_router& sender,*/ const etl::imessage& event)
  {
    get_fsm_context().LogUnknownEvent(event);

    return STATE_ID;
  }

//***********************************
  etl::fsm_state_id_t on_enter_state()
  {
    get_fsm_context().SetRunningLampOn();
    Serial.println("At Speed: on entering state");
    return STATE_ID;
  }

   //***********************************
  void on_exit_state()
  {
    //get_fsm_context().SetRunningLampOff();
    Serial.println("At Speed: on exiting state");
    //return STATE_ID;
  }

};

// The states.
// Now global as they could be used outside setup()
Idle        idle;
Running     running;
WindingUp   windingUp;
WindingDown windingDown;
AtSpeed     atSpeed;

// The FSM. Made global
MotorControl motorControl;
AnotherSystem anotherSystem;


String board(ARDUINO_BOARD);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println(" ");
  Serial.println("===========================================================================");
  Serial.println("Embedded Template Library Example 6 Motor Hierarchical Finite State Machine");
  Serial.print("running on ");
  Serial.println(board);
  Serial.println("===========================================================================");

// The states must be in state id order.
// The first state is the one entered on Start().
etl::ifsm_state* stateList[StateId::NUMBER_OF_STATES] =
{
  &idle, &running, &windingUp, &windingDown, &atSpeed
};

// Note: The first event (windingUp) is the one entered when Running is entered. No action is needed.
// The action is within Running until one of the others goes to Idle.
etl::ifsm_state* childStates[] = 
{
  &windingUp, &windingDown, &atSpeed
};

// Set the FSM's state list - this is essential.
anotherSystem.set_states(stateList, etl::size(stateList));

anotherSystem.start();

running.set_child_states(childStates, etl::size(childStates));

// Set the FSM's state list
motorControl.set_states(stateList, etl::size(stateList));
//motorControl.Initialise(stateList, etl::size(stateList)); This is in the documentation but not in the code!!!

// Not set yet ( returns 152)
//Serial.print("State is "); Serial.println(motorControl.get_state_id());

// Start the motor. The first state is 'IDLE'.
//motorControl.start(false);
motorControl.start();

Serial.print("State is "); Serial.println(motorControl.get_state_id());

// Receive a Start event. The next state is 'Winding Up'.
motorControl.receive(Start());
Serial.print("State is "); Serial.println(motorControl.get_state_id());

// Receive a Timeout event. The next state is 'At Speed'.
motorControl.receive(TimeOut());
Serial.print("State is "); Serial.println(motorControl.get_state_id());

// Receive a SetSpeed event. The state is still 'At Speed'.
motorControl.receive(SetSpeed(100));
Serial.print("State is "); Serial.println(motorControl.get_state_id());

// Receive a Stop event. The next state is 'WINDING_DOWN'.
// MOTOR_CONTROL is optional here.
motorControl.receive(MOTOR_CONTROL,Stop());
Serial.print("State is "); Serial.println(motorControl.get_state_id());

// Receive a Stopped event. The next state is 'IDLE'.
motorControl.receive(Stopped());
Serial.print("State is "); Serial.println(motorControl.get_state_id());

// Receive a Start event. The next state is 'RUNNING'.
motorControl.receive(Start());
Serial.print("State is "); Serial.println(motorControl.get_state_id());


// This is received as unknown message.
motorControl.receive(Fault());

// This is ignored
motorControl.receive(ANOTHER_SYSTEM,Fault());

// Output now from this.
anotherSystem.receive(ANOTHER_SYSTEM,Fault());

// Receive a Stop(emergency) event. The next state is 'IDLE'.
motorControl.receive(Stop());
Serial.print("State is "); Serial.println(motorControl.get_state_id());

motorControl.receive(Stopped());
Serial.print("State is "); Serial.println(motorControl.get_state_id());


Serial.println("At end of processing");

}

void loop()
{
  
}
