In any case, the what we are doing is very similar to 
his asm code, espically the ./asm -l function in his asm
code which is in the example code on his website, or in 
HW1 files

We don't touch the user code in the pl0 lexer, but we do write rules and add declarations

**pl0_lexer.l**<br>
%{<br>
    declarations<br>
%}<br>

%%<br>
    rules<br>
%%<br>

user code<br>

**lexer_main.c** should be done, but you can add an error message in case there is no files called alongside the program

I added all the sym's in the rules section of the pl0_lexer.l file, and we aren't allowed to touch the user code which is <br>
all the way at the bottom of said file

Near the end of the rules section there is an error message for invalid characters, but i don't know how to make one to<br>
limits of numbers

So that is all we have to do to finish, and whenever one of us does that, I'll do the testing on eustis and submit it again

