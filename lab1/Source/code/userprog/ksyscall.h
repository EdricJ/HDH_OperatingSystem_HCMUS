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
#define INT32_Min -2147483647-1

#include "kernel.h"
#include "synchconsole.h"
#include "ksyscallhelper.h"
#include <stdlib.h>

void SysHalt()
{
  kernel->interrupt->Halt();
}

//He thong cong
int SysAdd(int op1, int op2)
{
  return op1 + op2;
}

//He thong doc so
int SysReadNum() {
    readuntilWrong();

    int len = strlen(_numberBuffer);
    // Read nothing -> return 0
    if (len == 0) return 0;

    // Check min
    if (strcmp(_numberBuffer, "-2147483648") == 0) return INT32_Min;

    bool nega = (_numberBuffer[0] == '-');
    int zeros = 0; //dung de tinh so luong so 0 co trong chuoi neu luong so 0 no nam o dau
    bool checkZero = true;
    int num = 0;
    
    //kiem tra so am hay duong
    /*if(nega)
        i = 1;  bo dau tru dang truoc de doc vao so
    else
        i = 0;*/ //doc tu dau chuoi
   
    for (int i = nega; i < len; ++i) {
        char c = _numberBuffer[i];
        
        //kiem tra so 0 o dau
        if (c == '0' && checkZero)
            ++zeros;
        else
            checkZero = false;
        
        //ki tu khong phai so
        if (c < '0' || c > '9') {
            DEBUG(dbgSys, "Number " << _numberBuffer << " unrecognized");
            return 0;
        }
        num = num * 10 + (c - '0');
    }

    // 00            01 or -0       04923 -0432
    if (zeros > 1 || (zeros == 1 && (num || nega))) {
        DEBUG(dbgSys, "Number " << _numberBuffer << " unrecognized");
        return 0;
    }

    //so am hop le
    if (nega)
        
        num = -num;

    // It's safe to return directly if the number is small
    if (len <= max_length - 2) 
        return num;

    if (compareNumstillString(num, _numberBuffer))
        return num;
    else
        DEBUG(dbgSys,"Expected int32 but number " << _numberBuffer << " unrecognized");

    return 0;
}

//He thong in so
void SysPrintNum(int num) {
    //print 0
    if (num == 0) 
        return kernel->synchConsoleOut->PutChar('0');

    //print min
    if (num == INT32_Min) {
        kernel->synchConsoleOut->PutChar('-');
        for (int i = 0; i < 10; ++i)
            kernel->synchConsoleOut->PutChar("2147483648"[i]);
        return;
    }

    // print '-' o dau neu la so am
    if (num < 0) {
        kernel->synchConsoleOut->PutChar('-');
        num = -num;
    }

    //chuyen vao chuoi de co the in ra man hinh console
    int n = 0;
    while (num) {
        _numberBuffer[n++] = num % 10;
        num /= 10;
    }

    //vi chuyen tu hang don vi vao nen phai in ra nguoc lai
    for (int i = n - 1; i >= 0; --i)
        kernel->synchConsoleOut->PutChar(_numberBuffer[i] + '0');
}

//He thong doc chuoi
char* SysReadString(int length) {
    char* buffer = new char[length + 1];
    for (int i = 0; i < length; i++) {
        buffer[i] = kernel->synchConsoleIn->GetChar();      //su dung tool tu synchConsoleIn de doc tung ki tu trong chuoi
    }
    buffer[length] = '\0';   //ket thuc chuoi \0 
    return buffer;
}

//He thong xuat chuoi
void SysPrintString(char* buffer, int length) {
    for (int i = 0; i < length; i++) {
        kernel->synchConsoleOut->PutChar(buffer[i]);        //su dung tool tu synchConsoleOut de ghi tung ki tu trong chuoi
    }
}

#endif /* ! __USERPROG_KSYSCALL_H__ */
