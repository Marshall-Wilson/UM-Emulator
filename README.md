# Universal Machine Emulator

##### Written by: Marshall Wilson and Megan Gelement 
##### for Tufts Comp40, April 16, 2021

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

## Purpose
Thanks to Miles for help during office hours. 

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

## Installation

You can compile an executable version of the UM using the `make um` command

However, this program was developed as part of a course and as such, relies on some course-sepcific libraries. Specifically, it requires data structures from David Hanson's <em>C Interfaces and Implementations: Techniques for Creating Reusable Software</em>

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

## Usage

`./um um_program.um`
The UM takes in one program file in the .um executable binary file format and executes that program. 

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

## Demo
The following gif shows the UM performing several operations on an RPN calculator app I coded in the .um assembly language for a later project. 
![UM Demo](https://github.com/Marshall-Wilson/UM-Emulator/blob/main/um-demo.gif)

* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 

## TESTS 
#### 50mil.um 
        This test executes roughly 50 million instructions by looping through 
        a series of instructions until the total number of instructions executed
        is > 50000000, at which point it outputs '!' and halts. 
        
#### add-limit.um
        This file tests that the addition function returns the result mod 2^32
        by adding large numbers together whose result mod 2^32 is a character

#### add.um
        This file tests addition with small numbers by printing out the 
        results for numbers that encode ascii characters 

#### div.um
        This file tests that the division function works as expected on ints
        including division that divides evenly, division that does not divide 
        evenly, division by 1, and division of 0 by an int.

#### halt-verbose.um
        This file tests the halt command and prints out a message if the 
        machine doesn't halt 
        
#### halt.um
        This file tests the halt command only 
        
#### hello.um
        This file tests the load-val and output command by printing 
        "Hello World!\n" to the output 

#### input.um 
        This file tests the input command by taking an input from stdin and 
        then outputting that same input 

#### load-print.um
        This file also tests the load value and output commands by printing 
        the digits 0 through 9
        
#### load-prog.um
        This file tests the load program command by reloading segment 0, 
        writing a new segment with the commands to output 'A' and halt, 
        and then loading that new segment. The program would fail if any 
        of the load program operations didn't work as expected 
        
#### load-store.um
        This file tests the segmented load and store commands by mapping 
        a new segment, storing a series of values into that segment, and then 
        loading and printing those values in order, resulting in "Hello World!"

#### map-unmap.um
        This file tests the segment map and unmap functions by mapping and 
        unmapping several segments repeatedly and printing out the segment 
        ids of each mapped segment. 

#### mov.um
        This file tests the conditional move command by running the mov 
        command with different r[C] values and outputting the resulting r[A]
        
#### mult.um
        This file tests the multiplication command both on small cases and 
        on cases which exceed the 2^32 limit and wrap back around by printing 
        the results. 

#### nand.um
        This file tests the nand command by executing it on values that result 
        in the character 'B'

#### print-six.um
        This file tests the loadval, output, and addition by adding two numbers
        and then outputting the result as a character. 


* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
