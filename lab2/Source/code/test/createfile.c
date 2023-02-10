#include "syscall.h"

int main() {
    char fileName[256];
    int length;

    PrintString("Enter file's name's length: ");

    do{
        length = ReadNum();
        if(length > 256)
            PrintString("\n Error, just <= 255 character\n");
    }while(length > 256);
    
    PrintString("Enter file's name: ");
    ReadString(fileName, length);
    

    if (Create(fileName) == 0) {
        PrintString("File ");
        PrintString(fileName);
        PrintString(" created successfully!\n");
    } else
        PrintString("Create file failed\n");

    Halt();
}