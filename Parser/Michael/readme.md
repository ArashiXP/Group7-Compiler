10/23<br>
I've started working on the parser, part one is a bit of a doozy, I tried looking at Maria's code for the .y file and <br>
I tried to write my own code in my .y file<br>
I was going to wait after thursday so the TA would shed some insight but I got worried so I started now.<br>

The code is a work in progress and I'll be honest I have no idea what I am doing, I'll try my best but I don't expect too<br>
great of code from me.

I managed to fit in most of the pl0 grammar the professor provided, but hit a wall so I stopped, I won't be working on it<br>
until after thursday cause I am really busy, but next week I promise I'll be working specifically on it.<br>

10/27<br>
I pretty much finished the bison parser, but the only issue that remains is the empty rule for posSign near the bottom of <br>
the code in pl0.y

If anyone can, please try to make it so it can handle no sign or most of the outputs won't work.

The PosSign rule will recognise a posSign like + then do its thing, but most of the inputs don't have a sign so it posSign<br>
should be empty so it will move over to the number and continue parsing.

More or less I need help with writin just the | empty rule for posSign

I haven't even looked at part 2 yet

<br>

I added a rule for factor: and now it just prints out the token code, I don't know how to change that to be honest, but it <br>
is printing mostly all the right things, we just have to fix it printing out token codes

10/30 <br>
For now I finished the first part of the parser, meaning all non-decl stuff is done I'll keep at part 2, I just wanted to push this now so if anyone wants to do something they have the working code

11/5
For now I fixed the main code so the non-decl test cases pass.<br>
I'll take over and try to finish part 2.<br>
If anyone wants to give it a try heres the baseline<br>

Doing some debugging and got most of it to work<br>

we are at 50/56 test cases leaving only 6 left<br>

a problem I saw was that declerrtest 3 and 4 their line numbers is off, if anyone can fix this please do<br>

I'm trying to fix some of the other stuff to get max points but missing 6 cases can't be that bad<br>

Here is what is left in case anyone wants to pull a 100<br>

**declerrtest3** && **declerrtest4**<br>
Something is wrong with the line number, I don't really know what to do with them because if I increment the line number it messes<br>
up the other test cases

**declerrtest6**<br>
Something is wrong with the begin scope check, we can't change professors declaration of the begin statment in ast.h, so I <br>
don't know how we can fix it

**declerrtest7**<br>
Most likely another issue with the begin statments, I bruteforced the if function to make it work, but it may be interferring<br>
with some of the other cases, but none of the ones that already work

**declerrtest9**<br>
Something to do with the if scope check, needs to account for odd?

**declerrtestA**<br>
Another issue with the if statment


