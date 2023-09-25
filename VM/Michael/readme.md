## Notes

**9-22-23**<br>
I moved all the stuff I didn't need into files to reduce clogging, so the other test files
and the asm code.  From what I saw in the slides, the bof, instructions, machine_types, regname,
and utilities are the only codes we need for the VM

Also made good progress on the output/.myp part of the project.  Need to fix the formatting,
and eventually make it produce a .myp file since it just prints to terminal

**9-25-23**<br>
- Updated the Vm header
- Added the NOTR and STRA function to the GPR, (stops at NOTR and restarts at STRA)
- Accounted for an EXIT command
- Adjusted a few formatting bugs in GPR
- Moved around a few bits of code to both fix formatting and usability
- Changed parameters for a few functions

Still need to implement a jump function and actually do the math with the registers.<br>

__I commented out the printData() function in print tracing cause it does a weird number bug__<br>
__but it is needed to do the 1028 1024 ... stuff__<br>
__uncomment it out and run the test to see for yourself I'll try to fix it but would like help if__<br>
__possible__<br>
Other than that we are getting close to the end<br>


to compile use, also make sure you have the vm.h file
### gcc -o vm vm.c bof.c utilities.c instruction.c regname.c machine_types.c

to test the .lst's/.myp's

### ./vm -p vm_test#.bof

to test the .out's/.myo's

### ./vm vm_test#.bof
