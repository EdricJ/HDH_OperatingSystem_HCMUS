/* random.c
 *	Test RandomNum()
 */

#include "syscall.h"

int main() {
    PrintNum(RandomNum());

    Halt();
}