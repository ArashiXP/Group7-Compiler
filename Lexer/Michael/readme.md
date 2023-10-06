I got some answers from the professor regarding this assignment

I already did all the necessary stuff to make the pl0_lexers so don't worry too much about that 
    However, there could be an error in his lexer_output code which I talked about him in class 
    so that will be fixed soon probably<br>
    So it is best to probably wait a 
    little bit before starting

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

and we write our main stuff in lexer_main.c

run make when finishing editing the **pl0_lexer.l** to run the flex and then that will make **pl0_lexer.c** and **pl0_lexer.h**

**lexer_main.c** should initialise the lexer and close it and all the other functions

Just take a look at the asm code, it pretty much is word
for word how to do this