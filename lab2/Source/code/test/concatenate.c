#include "syscall.h"

int main() {
    int idSrc;
    int idDes;
    int read, write;
    int size_des, size_src;
    int len;
    char srcFilename[5] = "a.txt";     //gan cung
    char desFilename[5] = "d.txt";     
    //char c[2];                  // temp char
    char buffer[100];

    PrintString("\n\n\t\t <<<< COPY >>>> \n\n");

    idSrc = Open(srcFilename);   // open source file
    idDes = Open(desFilename);   // open destination file by read&write mode

    // exception : full openTable
    if (idSrc == -1 || idDes == -1 )
    {
        return -1;
    }
    else 
        PrintString("Open file successfully\n");

    size_des = Seek(-1, idDes); // size of file
    size_src = Seek(-1, idSrc);
    Seek(size_src + 1, idSrc);             // seek cursor to first character of source file
    Seek(0, idDes);             // seek cursor to first character of destinationdest file
    //c[1] = '\0';      

    read = Read(buffer, size_des, idDes); 
    len = 0;
    while (buffer[len] != '\0') ++len;

    write = Write(buffer, len, idSrc);

    PrintString("Write ");
    PrintNum(write);
    PrintString(" characters: ");
    PrintString(buffer);
    PrintString("\n");

    PrintString(">Concatenate successfully !!\n\n"); // Thong bao thanh cong

    Close(idSrc); // Close source file
    Close(idDes); // Close destinationdest file

    Halt();
}