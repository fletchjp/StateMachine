// Example_2_StateMachine.ino
// Starting from state machine example 
// This is now running. The messages are processed by STATE1 and STATE2 does no work.
// make sure that we do not rely on the STL
#define ETL_NO_STL

#include "Embedded_Template_Library.h"
#include "etl/queue.h"
#include "etl/fsm.h"
#include "etl/message_packet.h"

//*****************************************************************************
// The messages.
//*****************************************************************************
struct Message1 : public etl::message<1>
{
  Message1(int i_)
    : i(i_)
  {
  }

  int i;
};

struct Message2 : public etl::message<2>
{
  Message2(double d_)
    : d(d_)
  {
  }

  double d;
};

struct Message3 : public etl::message<3>
{
  Message3(String s_)
    : s(s_)
  {
  }

  String s;
};

struct Message4 : public etl::message<4>
{
};

enum
{
  STATE1,
  STATE2
};

//*****************************************************************************
// The Finite State Machine.
//*****************************************************************************
class Fsm : public etl::fsm
{
public:

  //***************************************************************************
  // Constructor.
  //***************************************************************************
  Fsm()
    : fsm(1)
  {
  }
 //***************************************************************************
  // The overridden virtual receive function.
  //***************************************************************************
  void receive(const etl::imessage& msg_) override
  {
    if (accepts(msg_))
    {
      // Place in queue.
      queue.emplace(msg_);

      Serial.print("Queueing message ");
      Serial.println(int(msg_.get_message_id()));
    }
    else
    {
      Serial.print("Ignoring message ");
      Serial.println(int(msg_.get_message_id()));
    }
  }

  //***************************************************************************
  // The method to call to handle any queued messages.
  //***************************************************************************
  void process_queue()
  {
    while (!queue.empty())
    {
      message_packet& packet = queue.front();
      etl::imessage& msg = packet.get();
      Serial.print("Processing message ");
      Serial.println(int(msg.get_message_id()));

      // Call the base class's receive function.
      // This will route it to the correct 'on_event' handler.
      etl::fsm::receive(msg);

      queue.pop();
    }
  }

private:

  typedef etl::message_packet< Message1, Message2, Message3, Message4> message_packet;

  // The queue of message items.
  etl::queue<message_packet, 10> queue;
};

//*****************************************************************************
// State 1.
//*****************************************************************************
class State1 : public etl::fsm_state<Fsm, State1, STATE1, Message1, Message2, Message3>
{
public:

  //***************************************************************************
  // When we enter this state.
  //***************************************************************************
  etl::fsm_state_id_t on_enter_state() override
  {
    Serial.println("  S1 : Enter state");
    return STATE1;
  }

  //***************************************************************************
  void on_exit_state() override
  {
    Serial.println("  S1 : Exit state");
  }

  //***************************************************************************
  etl::fsm_state_id_t on_event(const Message1& msg)
  {
    Serial.print("  S1 : Received message ");
    Serial.print(int(msg.get_message_id())); Serial.print(" : '");
    Serial.print(msg.i);  Serial.println("'");
    Serial.flush();
    return STATE1;
  }

  //***************************************************************************
  etl::fsm_state_id_t on_event(const Message2& msg)
  {
    Serial.print("S1 : Received message "); Serial.print(msg.get_message_id());
    Serial.print(" : '"); Serial.print(msg.d); Serial.println("'");
    Serial.flush();
    return STATE1;
  }

  //***************************************************************************
  etl::fsm_state_id_t on_event(const Message3& msg)
  {
    Serial.print("S1 : Received message "); Serial.print(msg.get_message_id());
    Serial.print(" : '"); Serial.print(msg.s); Serial.println("'");
    Serial.flush();
    return STATE1;
  }

  //***************************************************************************
  etl::fsm_state_id_t on_event_unknown(const etl::imessage& msg)
  {
    Serial.print("S1 : Received unknown message "); Serial.println(msg.get_message_id());
    Serial.flush();
    return STATE2;
  }
};


//*****************************************************************************
// State 2.
//*****************************************************************************
class State2 : public etl::fsm_state<Fsm, State2, STATE2, Message1, Message2, Message3>
{
public:

  //***************************************************************************
  etl::fsm_state_id_t on_enter_state() override
  {
    Serial.println("  S2 : Enter state");
    return STATE2;
  }

  //***************************************************************************
  // When we enter this state.
  //***************************************************************************
  void on_exit_state() override
  {
    Serial.println("  S2 : Exit state");
  }

  //***************************************************************************
  etl::fsm_state_id_t on_event(const Message1& msg)
  {
    Serial.print("  S2 : Received message ");
    Serial.print(int(msg.get_message_id())); Serial.print(" : '");
    Serial.print(msg.i);  Serial.println("'");
    Serial.flush();
    return STATE2;
  }

  //***************************************************************************
  etl::fsm_state_id_t on_event(const Message2& msg)
  {
    Serial.print("S2 : Received message "); Serial.print(msg.get_message_id());
    Serial.print(" : '"); Serial.print(msg.d); Serial.println("'");
    Serial.flush();
    return STATE2;
  }

  //***************************************************************************
  etl::fsm_state_id_t on_event(const Message3& msg)
  {
    Serial.print("S1 : Received message "); Serial.print(msg.get_message_id());
    Serial.print(" : '"); Serial.print(msg.s); Serial.println("'");
    Serial.flush();
    return STATE2;
  }

  //***************************************************************************
  etl::fsm_state_id_t on_event_unknown(const etl::imessage& msg)
  {
    Serial.print("S2 : Received unknown message "); Serial.println(msg.get_message_id());
    Serial.flush();
    return STATE1;
  }
};


String board(ARDUINO_BOARD);

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println("Embedded Template Library Example 2 Finite State Machine");
  Serial.print("running on ");
  Serial.println(board);

  Fsm fsm;

  State1 state1;
  State2 state2;
 
  // The list of states.
  etl::ifsm_state* state_list[] = { &state1, &state2 };

  // Define some messages.
  Message1 m1(1);
  Message2 m2(1.2);
  Message3 m3("Hello");

  // Set up the FSM
  fsm.set_states(state_list, 2);
  fsm.start();

  // Queue all of the messages.
  etl::send_message(fsm, m1);
  etl::send_message(fsm, Message1(2));
  etl::send_message(fsm, m2);
  etl::send_message(fsm, Message2(3.4));
  etl::send_message(fsm, m3);
  etl::send_message(fsm, Message3("World"));
  etl::send_message(fsm, Message4());

  Serial.println(" ");

  // De-queue them
  fsm.process_queue();

  Serial.println("At end of processing");

}

void loop()
{
  
}
