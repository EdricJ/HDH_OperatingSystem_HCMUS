#include "syscall.h"
#define SIZE 100

int main(){
    int n, key, A[SIZE + 1], i, j, flag, temp;
    n = key = 0;
    
    //check size array
    do
    {
        PrintString("n (0 <= n <= 100): ");
        n = ReadNum();
        if (n < 0 || n > 100)
            PrintString("n has to be an integer between 1 and 100 , please try again in format\n");
    } while (n < 0 || n > 100);

    //Input items to array
    for(i = 0; i < n; i++){
        PrintChar('[');
        PrintNum(i);
        PrintString("]: ");
        A[i] = ReadNum();
    }

    //check key for sort
    do
    {
        PrintString("sort order (1: increasing, 2: decreasing): ");
        key = ReadNum();
        if (key != 1 && key != 2)
            PrintString("Error, please try again\n");
    } while (key != 1 && key != 2);
    
    //bubble sort 
    flag = 0;
    for (i = 0; i < n - 1; i++) {
        for (j = 0; j < n - i - 1; j++) {
            if (key == 1) {                 //sap xep tang dan
                if (A[j] > A[j + 1]) {      //neu so truoc lon hon so sau thi doi cho
                    temp = A[j];
                    A[j] = A[j + 1];
                    A[j + 1] = temp;
                    
                    flag = 1;
                }
            } else if (key == 2) {          //sap xep giam dan
                if (A[j] < A[j + 1]) {      //neu so truoc nho hon so sau thi doi cho
                    temp = A[j];
                    A[j] = A[j + 1];
                    A[j + 1] = temp;

                    flag = 1;
                }
            }
        }

        if(flag == 0) break;
    }

    PrintString("Array is sorted: ");

    //Output array to console
    for(i = 0; i < n; i++){
        PrintNum(A[i]);
        PrintChar(' ');
    }

    Halt();
}