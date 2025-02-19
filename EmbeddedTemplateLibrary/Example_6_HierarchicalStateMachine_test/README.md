# Example_6_HierarchicalStateMachine

This is an example implementation of the HierarchicalStateMachine.

This diagram is being built using Mermaid.

https://mermaid.js.org/intro/getting-started.html

```mermaid
stateDiagram-v2
   state: "HierarchicalStateMachine"
   [*] --> Idle
   Idle --> Running : START
   state Running {
   [*] --> WindingUp
   WindingUp --> AtSpeed  : TIMEOUT
   WindingUp --> WindingDown  : STOP
   AtSpeed--> WindingDown : STOP
   AtSpeed --> AtSpeed : SET_SPEED
   WindingDown --> [*] : STOPPED
   }
   Running --> Idle
```


