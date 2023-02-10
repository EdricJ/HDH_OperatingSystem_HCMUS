/**************************************************************
 *
 * userprog/ksyscall.h
 *
 * Kernel interface for systemcalls
 *
 * by Marcus Voelp  (c) Universitaet Karlsruhe
 *
 **************************************************************/

#ifndef __USERPROG_KSYSCALL_H__
#define __USERPROG_KSYSCALL_H__
#define INT32_Min -2147483647 - 1
#define LF ((char)10)
#define CR ((char)13)
#define TAB ((char)9)
#define SPACE ((char)' ')

/* maximum length of an interger (included the minus sign) */
#define max_length 11
#define MAX_FILE 10

#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "kernel.h"
#include "synchconsole.h"
#include <stdlib.h>

void SysHalt()
{
    kernel->interrupt->Halt();
}

// He thong cong
int SysAdd(int op1, int op2)
{
    return op1 + op2;
}

// khoi tao mot buffer chua so
char _numberBuffer[max_length + 2];

// kiem tra khoang trang va cac ki tu dac biet
char isWrongChar(char c) { return c == LF || c == CR || c == TAB || c == SPACE; }

void readuntilWrong()
{
    memset(_numberBuffer, 0, sizeof(_numberBuffer)); // tao buffer NULL
    char c = kernel->synchConsoleIn->GetChar();

    if (c == EOF)
    {
        DEBUG(dbgSys, "end of file unrecognized - number expected");
        return;
    }

    if (isWrongChar(c))
    {
        DEBUG(dbgSys, "white-space unrecognized - number expected");
        return;
    }

    int n = 0;

    while (!(isWrongChar(c) || c == EOF))
    {
        _numberBuffer[n++] = c;
        if (n > max_length)
        {
            DEBUG(dbgSys, "Number is too long");
            return;
        }
        c = kernel->synchConsoleIn->GetChar();
    }
}

// kiem tra so dang o dang string
bool compareNumstillString(int integer, const char *s)
{
    if (integer == 0)
        return strcmp(s, "0") == 0;

    int len = strlen(s);

    // neu la so am nhung trong khong co dau tru trong chuoi
    if (integer < 0 && s[0] != '-')
        return false;

    // neu la so am thi chuyen thanh so duong va bo dau tru dang truoc trong chuoi di
    if (integer < 0)
    {
        s++;
        --len;
        integer = -integer;
    }

    while (integer > 0)
    {
        int digit = integer % 10;

        if (s[len - 1] - '0' != digit)
            return false; // khi trong chuoi co 1 ky tu khong phai la so, chang han so thuc

        --len;
        integer /= 10;
    }

    return len == 0; // tra ve true neu compare Num va String hoan tat
}

// He thong doc so
int SysReadNum()
{
    readuntilWrong();

    int len = strlen(_numberBuffer);
    // Read nothing -> return 0
    if (len == 0)
        return 0;

    // Check min
    if (strcmp(_numberBuffer, "-2147483648") == 0)
        return INT32_Min;

    bool nega = (_numberBuffer[0] == '-');
    int zeros = 0; // dung de tinh so luong so 0 co trong chuoi neu luong so 0 no nam o dau
    bool checkZero = true;
    int num = 0;

    // kiem tra so am hay duong
    /*if(nega)
        i = 1;  bo dau tru dang truoc de doc vao so
    else
        i = 0;*/
    // doc tu dau chuoi

    for (int i = nega; i < len; ++i)
    {
        char c = _numberBuffer[i];

        // kiem tra so 0 o dau
        if (c == '0' && checkZero)
            ++zeros;
        else
            checkZero = false;

        // ki tu khong phai so
        if (c < '0' || c > '9')
        {
            DEBUG(dbgSys, "Number " << _numberBuffer << " unrecognized");
            return 0;
        }
        num = num * 10 + (c - '0');
    }

    // 00            01 or -0       04923 -0432
    if (zeros > 1 || (zeros == 1 && (num || nega)))
    {
        DEBUG(dbgSys, "Number " << _numberBuffer << " unrecognized");
        return 0;
    }

    // so am hop le
    if (nega)

        num = -num;

    // It's safe to return directly if the number is small
    if (len <= max_length - 2)
        return num;

    if (compareNumstillString(num, _numberBuffer))
        return num;
    else
        DEBUG(dbgSys, "Expected int32 but number " << _numberBuffer << " unrecognized");

    return 0;
}

// He thong in so
void SysPrintNum(int num)
{
    // print 0
    if (num == 0)
        return kernel->synchConsoleOut->PutChar('0');

    // print min
    if (num == INT32_Min)
    {
        kernel->synchConsoleOut->PutChar('-');
        for (int i = 0; i < 10; ++i)
            kernel->synchConsoleOut->PutChar("2147483648"[i]);
        return;
    }

    // print '-' o dau neu la so am
    if (num < 0)
    {
        kernel->synchConsoleOut->PutChar('-');
        num = -num;
    }

    // chuyen vao chuoi de co the in ra man hinh console
    int n = 0;
    while (num)
    {
        _numberBuffer[n++] = num % 10;
        num /= 10;
    }

    // vi chuyen tu hang don vi vao nen phai in ra nguoc lai
    for (int i = n - 1; i >= 0; --i)
        kernel->synchConsoleOut->PutChar(_numberBuffer[i] + '0');
}

// He thong doc chuoi
char *SysReadString(int length)
{
    char *buffer = new char[length + 1];
    for (int i = 0; i < length; i++)
    {
        buffer[i] = kernel->synchConsoleIn->GetChar(); // su dung tool tu synchConsoleIn de doc tung ki tu trong chuoi
    }
    buffer[length] = '\0'; // ket thuc chuoi \0
    return buffer;
}

// He thong xuat chuoi
void SysPrintString(char *buffer, int length)
{
    for (int i = 0; i < length; i++)
    {
        kernel->synchConsoleOut->PutChar(buffer[i]); // su dung tool tu synchConsoleOut de ghi tung ki tu trong chuoi
    }
}

// He thong tao file moi trong file code/test
bool SysCreateFile(char *fileName)
{
    bool success;

    if (strlen(fileName) == 0)
    {
        DEBUG(dbgSys, "\nFile name can't be empty");
        success = false;
    }

    if (strlen(fileName) > 255)
    {
        // tránh lỗi tràn vùng nhớ
        DEBUG(dbgSys, "String length errupts %s" << 256);
        success = false;
    }
    
    else if (fileName == NULL)
    {
        DEBUG(dbgSys, "\nNot enough memory in system");
        success = false;
    }
    else
    {
        DEBUG(dbgSys, "\nFile's name read successfully");
        // Create file with size = 0
        // Dùng đối tượng fileSystem của lớp OpenFile để tạo file,
        // việc tạo file này là sử dụng các thủ tục tạo file của hệ điều
        // hành Linux, chúng ta không quản ly trực tiếp các block trên
        // đĩa cứng cấp phát cho file, việc quản ly các block của file
        // trên ổ đĩa là một đồ án khác
        if (!kernel->fileSystem->Create(fileName))
        {
            DEBUG(dbgSys, "\nError creating file");
            success = false;
        }
        else
        {
            success = true;
        }
    }

    return success;
}

// He thong mo file
int SysOpen(char *fileName)
{
    // if (type != 0 && type != 1)
    //     return -1;

    int id = kernel->fileSystem->OpenF(fileName);
    if (id == -1)
        return -1;
    DEBUG(dbgSys, "\nOpened file");
    return id;
}

// He thong loai bo
bool SysRemove(char* filename){
     bool success;

    if(SysOpen(filename) != -1)
        success = false;

    if (strlen(filename) == 0)
    {
        DEBUG(dbgSys, "\nFile name can't be empty");
        success = false;
    }
    
    else if (filename == NULL)
    {
        DEBUG(dbgSys, "\nNot enough memory in system");
        success = false;
    }
    else
    {
        DEBUG(dbgSys, "\nFile will be removed successfully");
        // tuong tu nhu createfile
        if (!kernel->fileSystem->Remove(filename))
        {
            DEBUG(dbgSys, "\nError remove file");
            success = false;
        }
        else
        {
            success = true;
        }
    }

    return success;
}

#endif /* ! __USERPROG_KSYSCALL_H__ */