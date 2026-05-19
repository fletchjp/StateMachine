# Arduino R4 State Machines

These are codes for the Arduino Uno R4 Minima using C++17 and will not run on older Arduinos.

They have been built using code based on the work of Tamir Bahar:

https://tamir.dev/posts/a-functional-style-state-machine-in-cpp-part-2/

There is a page on the MERG Knowledgebase here:

https://merg.org.uk/merg_wiki/doku.php?id=programming:finitestatemachines:start

John Fletcher  October 2025

## Code developments

I have done a series of experiments to explore making the code more flexible. I will add some of these.

### AVOID TaskManagerIO

I have been using TaskManagerIO on the R4 series. It turns out that it is not supported on the R4s. 

Tasks appear to run alright.

The problem is classes inheriting from BaseEvent. The code crashes and corrupts the USB port of the computer.

I would like to use it and have raised a request with the library owner.






