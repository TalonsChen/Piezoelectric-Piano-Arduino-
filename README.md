# Piezoelectric-Piano-Arduino-
We had make a piano based on piezo-sesor to detect pressure(velocity). Our goal is to make a cheap piano with the ability to 
take the velcoty.
The project contains:
1) take measurement on the piezo-sensor we bought from the electronic component. We want to know the relation between 
mechanical properties and electric properties, then decide how to detect the signal.

2) Design a keyboard.

3) Using Arduino to handle the signal.

4)Using a MIDI chip to play the sound or transfer info to computer.

5) Power Supply

Problem we met:
1) The signal from the piezo-sensor is not perfect. It has short output time (about 8ms), high output impedance, voltage accumulation...
2) Arduino doesn't have the ability of multithreading, and the analog read is too slow for us to scan the key.

Solving:
1) We use op-amp to change the output impedance, and using a multiplexer to switch between the key.
  Then, we use comparators to do fast ADC (because the ADC chip from the marktet is a little slow or it is expensive).
  At the output of the comparators, we use a shift register to transer parallel signal to serial, and use Arduino SPI(much faster) to read.

2)Because Arduino Uno  clock is 16MHz, the speed is a little slow, we bought a Arduino DUE. Then the program to communicate between 
  UNO and DUE cause us a lot of time. (It is a wrong decision, but we learn a lot from the difficulties.)
  
3) To make a pinao keyboard, we first use 3d printer to make key, but it is too slow. At last, we use wood to make a keyboard, and it 
   worked fine, but looked ugly. (That is ok! Because our goal is to learn how to use piezosensor make piano)
   
The project is end (it is a class in the university.)
We achieve:
All goal, but there are a lot of things to refine. We made a lot of mistakes, and they cause us lot of time to solve. But it is fun.
