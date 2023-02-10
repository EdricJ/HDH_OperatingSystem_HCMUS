#include "syscall.h"

int main() {
    char fileName[256];
    int length;
    int fileid;

    PrintString("Enter file's name's length: ");
    
    do{
        length = ReadNum();
        if(length > 256)
            PrintString("\n Error, just <= 255 character\n");
    }while(length > 256);

    PrintString("Enter file's name: ");
    ReadString(fileName, length); 

    if (Remove(fileName) == 0) {
        fileid = Open(fileName);
        if(fileid != -1 || (fileid >= 2 && fileid <= 10)){
            PrintString("Your file is opened\n");
            Close(fileid);
            fileid = -1;
        }

        PrintString("File ");
        PrintString(fileName);
        PrintString(" removed successfully!\n");
    } else
        PrintString("Remove file failed\n");

    Halt();
}