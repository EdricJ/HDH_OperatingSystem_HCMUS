#include "syscall.h"

int main(){
    int i;
    PrintString("ASCII Character\n");
    for(i = 32; i <= 127; i++){
        PrintNum(i);
        PrintChar(':');
        PrintChar(' ');
        PrintChar((char)i);
        PrintChar('\n');
    }

    Halt();
}