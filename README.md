# Introduction

This program simulates a moore or mealy (you can choose) state machine for 1-bit, 2-bit, 3-bit, and 4-bit inputs. You can print the graph of the state machine in the form of a table. Have fun simulating your state machines :)

# Summary
For this project, we chose to implement our finite state machine in C++ using an adjacency list, which was done using vectors in C++. For each state, a `Node_Entry` is created. The entry contains a name field, a map of input values to Boolean values to see which inputs have been defined, a vector of `Arc_Entry` elements that originate from the original node, and an optional output field (to be used for Moore state machines). Each `Arc_Entry` has a field for the `start_node`,`end_node`,`input`,and `output` (forMealymachines). The inclusion of the vector of `Arc_Entry` elements is what makes our implementation an adjacency list. The fields included in each entry of this vector make it easier to keep track of the relationship between nodes and make it easier to print out the graph and the table.

# Requirements

* GCC to compile and run C++ code

# Steps
```
> cd finite-state-machine/src
> make
> ./fsm
```

# Example Run for 2-bit input Moore State Machine
```
FSM Simulator

-------- FSM Help ------
Please enter the machine type and number of input bits as prompted upon starting simulation. The following commands can be used to define different states and the transitions between them.           
--------------------------------------------------------------------- 
NODE [name] {Mealy} || NODE [name] [output] {Moore} -  add a node to the graph         
ARC [start] [end] [in / out] {Mealy} || ARC [start] [end] [in] {Moore}
output -  shows output graph              
?      -  display this help menu          
quit   -  exit the program                

FSM-SIM> Please specify simulation type. Enter MEALY or MOORE: Mealy
FSM-SIM> Please specify the number of input bits (1-4): 2
FSM-SIM> Node Blue 
FSM-SIM> Node Red
FSM-SIM> Arc Red Red 00 / hot
FSM-SIM> Arc Blue Blue 0x / hold
FSM-SIM> Arc Red Blue 01 / cold
FSM-SIM> Arc Orange Blue 10 / cold
%% error: state "Orange" not defined %%
FSM-SIM> Arc Blue Red 11 / on
FSM-SIM> Arc Red Blue 1x off
off
FSM-SIM> output
Output GRAPH:
Blue
	Blue 0x / hold
	Red 11 / on
	%% warning: input 10 not specified %%
Red
	Red 00 / hot
	Blue 01 / cold
	Blue 1x / off

Current	|	Next State / Output
State		| X = 00      X = 01      X = 10      X = 11      
--------------------------------------------------------
Blue		| Blue/hold   Blue/hold   x/x         Red/on      
Red		| Red/hot     Blue/cold   Blue/off    Blue/off

```