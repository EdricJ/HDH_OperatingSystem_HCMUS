#ifndef __USERPROG_KSYSCALLHELPER_H__
#define __USERPROG_KSYSCALLHELPER_H__

#include "kernel.h"
#include "synchconsole.h"   //goi thu vien synchconsole de co the doc ghi ki tu

#define LF ((char)10)
#define CR ((char)13)
#define TAB ((char)9)
#define SPACE ((char)' ')

/* maximum length of an interger (included the minus sign) */
#define max_length 11

//khoi tao mot buffer chua so
char _numberBuffer[max_length + 2];

//kiem tra khoang trang va cac ki tu dac biet
char isWrongChar(char c) { return c == LF || c == CR || c == TAB || c == SPACE; }

void readuntilWrong() {
    memset(_numberBuffer, 0, sizeof(_numberBuffer)); //tao buffer NULL
    char c = kernel->synchConsoleIn->GetChar();

    if (c == EOF) {
        DEBUG(dbgSys, "end of file unrecognized - number expected");
        return;
    }

    if (isWrongChar(c)) {
        DEBUG(dbgSys, "white-space unrecognized - number expected");
        return;
    }

    int n = 0;

    while (!(isWrongChar(c) || c == EOF)) {
        _numberBuffer[n++] = c;
        if (n > max_length) {
            DEBUG(dbgSys, "Number is too long");
            return;
        }
        c = kernel->synchConsoleIn->GetChar();
    }
}

//kiem tra so o dang string
bool compareNumstillString(int integer, const char *s) {
    if (integer == 0) 
        return strcmp(s, "0") == 0;

    int len = strlen(s);

    //neu la so am nhung trong khong co dau tru trong chuoi
    if (integer < 0 && s[0] != '-') 
        return false;

    //neu la so am thi chuyen thanh so duong va bo dau tru dang truoc trong chuoi di
    if (integer < 0) {
        s++;
        --len;
        integer = -integer;
    }

    while (integer > 0) {
        int digit = integer % 10;

        if (s[len - 1] - '0' != digit) return false; // khi trong chuoi co 1 ky tu khong phai la so, chang han so thuc

        --len;
        integer /= 10;
    }

    return len == 0; //tra ve true neu compare Num va String hoan tat
}

#endif /* ! __USERPROG_KSYSCALLHELPER_H__ */