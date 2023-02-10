#include "syscall.h"

int main() {
    PrintString("Personel:\n");
    PrintString("20120184 Pham Quang Tan\n");
    PrintString("20120201 Pham Gia Thong\n");
    
    PrintString("This is the description about sort program and ascii program\n");
    PrintString("ASCII: at directory code, run build.linux/nachos -x test/ascii to print the ASCII table\n");
    PrintString("\t- From 32 to 126, PrintChar(char(num)) to print out list ascii (32 <= num <= 127)\n");
    PrintString("Sort: at directory code, run build.linux/nachos -x test/bubble_sort to start the sort program\n");
    PrintString("\t- Enter n (the length of the array, 0 <= n <= 100)\n");
    PrintString("\t- Enter n elements of the array\n");
    PrintString("\t- Enter the order you want to sort the array with (1: increasing, 2: decreasing)\n");
    PrintString("\t- The program will print out the sorted array and then exit\n");

    Halt();
}