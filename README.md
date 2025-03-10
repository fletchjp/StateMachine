# StateMachine
 Arduino State Machine experiments

I have made this repository which was not at first visible in my GitHub as it is private and I needed to log in.

I have now made this repository public.

## State Machine diagrams

I have begun to use Mermaid (https://mermaid.js.org/intro/getting-started.html) as a way of making diagrams for Finite State Machines.

Mermaid can be used to make a range of different diagrams and they can be coded as special text in GitHub pages using Markdown.

``` mermaid
stateDiagram-v2
  state: "Door"
  open --> closed : Closing
  closed --> open : Opening
```
There needs to be some other text between diagrams.

### Another diagram

``` mermaid
  stateDiagram-v2
  direction LR
  state: "Door"
  open --> closed : Closing
  closed --> open : Opening
```



## Embedded Template Library

I am putting my examples for the Embedded Template Library Arduino library into a folder here.

## Other codes

Some of these codes use the StateMachine library and some have been changed to use TaskManagerIO instead.

## arduino_state_test

Empty base case

## arduino_state_machine

Example with serial monitor output

## arduino_state_blink

This is the first example to blink the LED.

## arduino_state_blink_new

Example using enum class instead of enum.

## arduino_state_blink_moustache

This is a new example which replaces the previous output using the Arduino moustache for output.

This shows how a format can be defined for the output of the description of a state which will then output one of an array of state descriptions.

This has now been extended to allow for run time values and also error reporting.

The extensions have now been moved to the moustache library.

## states_timers

Timers example from the StateMachine library.
