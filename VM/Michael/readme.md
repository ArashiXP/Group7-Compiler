## Notes

**9-22-23**<br>
I moved all the stuff I didn't need into files to reduce clogging, so the other test files
and the asm code.  From what I saw in the slides, the bof, instructions, machine_types, regname,
and utilities are the only codes we need for the VM

Also made good progress on the output/.myp part of the project.  Need to fix the formatting,
and eventually make it produce a .myp file since it just prints to terminal

**9-24-23**<br>
- Updated the Vm header
- Added the NOTR and STRA function to the GPR, (stops at NOTR and restarts at STRA)
- Accounted for an EXIT command
- Adjusted a few formatting bugs in GPR
- Moved around a few bits of code to both fix formatting and usability
- Changed parameters for a few functions

Still need to implement a jump function and actually do the math with the registers.

~~I commented out the printData() function in print tracing cause it does a~~
~~weird number bug~~
~~but it is needed to do the 1028 1024 ... stuff~~
~~uncomment it out and run the test to see for yourself I'll try to fix it~~
~~but would like help if~~
~~possible~~

**9-25-23**<br>
- Added the JMP and JAL functions (Though may not be complete yet)
- Updated the Vm header
- Fixed improper function calls
- Changed the code to accept 2 arrays, one that holds all the instructions and one for
  the bottom numbers
- Made two functions to get the two arrays, I implemented it into the functions so no
  need to worry
- Fixed the prior bug that I crossed out above
- Finished the ADD, ADDI, SUB functions and got it to display on the GPR
- Added AND ANDI XOR XORI BOR BORI SLL SRL

With the 2 arrays, The one that holds all the instructions is 2D because it is a Array of strings and 
the other just holds the bottom ints.
just access them how you would any array like instruct[i]
Though be careful calling __bof_read_header__ , __bof_read_word__, and so forth because they only go forward
if that makes sense. Try to only call them when you need to or the order will mess up.

In printTracing there is a little block of code that has the comment __This is to split the instruction...__
That is to take a string, for example the instruction and split it into individual parts and
stores into an array I called token

SUB $a0, $a1, $v0 <br>
__Becomes__ <br>
["SUB, $a0, $a1, $v0"] <br>

This could help you or could not, idk.<br>


**9-26-23**<br>
- Fixed a few bugs with existing commands
- Updated the header
- Made changes to the MAKEFILE so we can use it to complile
- Add HI and LO to be shown if they aren't zero
- Implemented JR PCH RCH (Sort of RCH idk how to test it)
- The PCH command prints the words that were confusing before so that has been addressed, but I think <br>
  of the professors tests cases (test case 5) is wrong because the code skips over the Y
- Fixed how NOTR and STRA worked, I thought it stopped until STRA, but it stops tracking but still does<br>
  the commands
- Adjusted formatting bugs
- Moved around blocks of code to look better
- Made a union for the memory
- Made sort of commands for LBU, LW, SB, SW but they don't fully work for all test cases<br>
- Started implementing MEMORY but it doesn't look very correct it's needed for LBU, LW, SB,<br>
  SW, PSTR

I need help fixing the memory, I have no real idea what's going on but here is what I put as a placeholder<br>

So The LW SW and all of them usually only get stored into the $gp or the $sp<br>
The $gp stores all of the words at the bottom which I copied into the memory.gp array<br>
The $sp stores other stuff so it just exists<br>
So I have a global union inside of the VM header which can just be called whenever, no need to pass it<br>
and if you look at the commands near the bottom of the printTrace() function, you'll see my crazy attempt<br>
I'm just at a loss honestly on how to do it<br>


to compile use, also make sure you have the vm.h file
### gcc -o vm vm.c bof.c utilities.c instruction.c regname.c machine_types.c
<br>
or with update MAKEFILE just type in make
<br>

### make

to test the .lst's/.myp's

### ./vm -p vm_test#.bof

to test the .out's/.myo's

### ./vm vm_test#.bof
