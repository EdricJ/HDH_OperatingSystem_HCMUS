#include "syscall.h"

int main() {
    char srcFilename[256], desFilename[256], file[11] = "newFile.txt";
    int idSrc;
    int idDes;
    int read, write;
    int size;
    int len;
    int src_length, des_length;
    
    //char c[2];                  // temp char
    char buffer[100];

    PrintString("\n\n\t\t <<<< COPY >>>> \n\n");

    //Read length file src
    PrintString("Enter source file's name's length: ");
    do{
        src_length = ReadNum();
        if(src_length > 256)
            PrintString("\n Error, just <= 255 character\n");
    }while(src_length > 256);

    // PrintString("Enter destinate file's name's length: ");
    // do{
    //     des_length = ReadNum();
    //     if(des_length > 256)
    //         PrintString("\n Error, just <= 255 character\n");
    // }while(des_length > 256);

    PrintString("Input srcFile's name: ");
    ReadString(srcFilename, src_length);

    // PrintString("Input desFile's name: ");
    // ReadString(desFilename, des_length + 1);

    if (Create(file) == 0) {                         //create newfile for copied
        PrintString("File ");
        PrintString(file);
        PrintString(" created for copy successfully!\n");
    } else
        PrintString("Create file failed\n");

    
    idSrc = Open(srcFilename);   // open source file
    idDes = Open(file);   // open destin ation file by read&write mode
    
    // exception : full openTable
    if (idSrc == -1 || idDes == -1)
    {
        return -1;
    }
    else 
        PrintString("Open file successfully\n");

    size = Seek(-1, idSrc); // size of file
    Seek(0, idSrc);             // seek cursor to first character of source file
    Seek(0, idDes);             // seek cursor to first character of destinationdest file
    //c[1] = '\0';      

    read = Read(buffer, size, idSrc);
    len = 0;
    while (buffer[len] != '\0') ++len;

    write = Write(buffer, len, idDes);

    PrintString("Write ");
    PrintNum(write);
    PrintString(" characters: ");
    PrintString(buffer);
    PrintString("\n");

    // // Print file Content
    // for (pos = 0; pos < size; pos++)
    // {
    //     // Copy each character
    //     Read(&c, 1, idSrc);
    //     Write(&c, 1, idDes);
    // }

    PrintString(">Copied successfully !!\n\n"); // Thong bao thanh cong
    Close(idSrc); // Close source file
    Close(idDes); // Close destinationdest file

    Halt();
}