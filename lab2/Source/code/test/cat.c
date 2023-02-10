#include "syscall.h"
#define MAXSIZE 256

int main()
{
    int fileid;
    int size;
    int read; 
    //char c[2];                  // temp char
    char filename[MAXSIZE];     // allocate filename charArr by MAXSIZE
    //int pos;
    int length;
    char buffer[100];

    PrintString("\n\n\t\t <<<< CAT >>>> \n\n");

    // Get filename from console
    PrintString("Enter file's name's length: ");
    
    do{
        length = ReadNum();
        if(length > 256)
            PrintString("\n Error, just <= 255 character\n");
    }while(length > 256);

    PrintString(">Input filename: ");
    ReadString(filename, length);

    fileid = Open(filename);

    // exception : full openTable
    if (fileid == -1)
    {
        return -1;
    }
    else 
        PrintString("Open file successfully\n");

    size = Seek(-1, fileid); // size of file
    Seek(0, fileid);             // seek cursor to first character of file
    //c[1] = '\0';      

    read = Read(buffer, size, fileid); 
    length = 0;
    while (buffer[length] != '\0') ++length;
    PrintString("Read ");
    PrintNum(length);
    PrintString(" characters: ");
    PrintString(buffer);
    PrintString("\n");
    Close(fileid);

    Halt();
}