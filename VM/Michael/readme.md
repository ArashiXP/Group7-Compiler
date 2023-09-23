## Notes

**9-22-23**<br>
I moved all the stuff I didn't need into files to reduce clogging, so the other test files
and the asm code.  From what I saw in the slides, the bof, instructions, machine_types, regname,
and utilities are the only codes we need for the VM

Also made good progress on the output/.myp part of the project.  Need to fix the formatting,
and eventually make it produce a .myp file since it just prints to terminal

to compile use, also make sure you have the vm.h file
### gcc -o vm vm.c bof.c utilities.c instruction.c regname.c machine_types.c

to test the .lst's/.myp's

### ./vm -p vm_test#.bof
