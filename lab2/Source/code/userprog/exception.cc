// exception.cc
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation
// of liability and disclaimer of warranty provisions.

#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "synchconsole.h"
#define MAX_READ_STRING_LENGTH 255
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2.
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions
//	is in machine.h.
//----------------------------------------------------------------------

// tham khao tu '[3] Cach Viet Mot SystemCall' trong tutorial
/*
Input: - User space address (int)
 - Limit of buffer (int)
Output:- Buffer (char*)
Purpose: Copy buffer from User memory space to System memory space
*/
char *string_User2System(int addr, int convert_length = -1)
{
	int length = 0;
	bool stop = false;
	char *str;

	do
	{
		int oneChar;
		kernel->machine->ReadMem(addr + length, 1, &oneChar);
		length++;
		// if convert_length == -1, we use '\0' to terminate the process
		// otherwise, we use convert_length to terminate the process
		stop = ((oneChar == '\0' && convert_length == -1) || length == convert_length);
	} while (!stop);

	str = new char[length];
	for (int i = 0; i < length; i++)
	{
		int oneChar;
		kernel->machine->ReadMem(addr + i, 1, &oneChar); // copy characters to kernel space
		str[i] = (unsigned char)oneChar;
	}
	return str;
}

/**
 * @brief Convert system string to user string
 *
 * @param str string to convert
 * @param addr addess of user string
 * @param convert_length set max length of string to convert, leave
 * blank to convert all characters of system string
 * @return void
 */
// tham khao tu '[3] Cach Viet Mot SystemCall' trong tutorial
/*
Input: - User space address (int)
 - Limit of buffer (int)
 - Buffer (char[])
Output:- Number of bytes copied (int)
Purpose: Copy buffer from System memory space to User memory space
*/
void string_System2User(char *str, int addr, int convert_length = -1)
{
	int length = (convert_length == -1 ? strlen(str) : convert_length);
	for (int i = 0; i < length; i++)
	{
		kernel->machine->WriteMem(addr + i, 1, str[i]); // copy characters to user space
	}
	kernel->machine->WriteMem(addr + length, 1, '\0');
}

/**
 * Modify program counter
 * This code is adapted from `../machine/mipssim.cc`, line 667
 **/
void PC_counting()
{
	/* set previous programm counter (debugging only)
	 * similar to: registers[PrevPCReg] = registers[PCReg];*/
	kernel->machine->WriteRegister(PrevPCReg,
								   kernel->machine->ReadRegister(PCReg));

	/* set programm counter to next instruction
	 * similar to: registers[PCReg] = registers[NextPCReg]*/
	kernel->machine->WriteRegister(PCReg,
								   kernel->machine->ReadRegister(NextPCReg));

	/* set next programm counter for brach execution
	 * similar to: registers[NextPCReg] = pcAfter;*/
	kernel->machine->WriteRegister(
		NextPCReg, kernel->machine->ReadRegister(NextPCReg) + 4);
}

void system_ReadInt()
{
	printf("Please type your number: \n");
	int num = SysReadNum();					// call system from ksyscall to read integer
	kernel->machine->WriteRegister(2, num); // read integer and put it in register 2

	// ASSERTNOTREACHED();
	PC_counting();
}

void system_PrintInt()
{
	int number = kernel->machine->ReadRegister(4); // read integer from register 4
	SysPrintNum(number);						   // call system from ksyscall to print integer

	// ASSERTNOTREACHED();
	PC_counting();
}

void system_ReadChar()
{
	printf("Please type your character: \n");
	char character = kernel->synchConsoleIn->GetChar(); // call tool from synchconsole to read character
	kernel->machine->WriteRegister(2, (int)character);  // read character and put it in register 2
	PC_counting();
	
	// ASSERTNOTREACHED();
}

void system_PrintChar()
{
	char character = (char)kernel->machine->ReadRegister(4); // read character from register 4
	kernel->synchConsoleOut->PutChar(character);			 // call tool from synchconsole to print character
	PC_counting();

	// ASSERTNOTREACHED();
}

void system_ReadString()
{

	int arr = kernel->machine->ReadRegister(4);	   // read for located arr from register 4
	int length = kernel->machine->ReadRegister(5); // read size of arr from register 5

	if (length > MAX_READ_STRING_LENGTH)
	{ // tránh tràn vùng nhớ
		DEBUG(dbgSys, "String length errupts " << MAX_READ_STRING_LENGTH);
		SysHalt();
	}
	printf("Please type your paragraph: \n");

	char *buffer = SysReadString(length); // call system from ksyscall to print character string
	string_System2User(buffer, arr);	  // call system to transfer data from system to user application
	delete[] buffer;
	PC_counting();
}

void system_PrintString()
{

	int memPtr = kernel->machine->ReadRegister(4); // doc dia c-string
	char *buffer = string_User2System(memPtr);	   // call system to transfer data from user application to system in odfer to view on console

	// printf("Your paragraph received: \n");
	SysPrintString(buffer, strlen(buffer)); // call system from ksyscall to print character string
	delete[] buffer;
	PC_counting();
}

void system_CreateFile()
{
	DEBUG(dbgSys, "\n SC_Create call ...");
	DEBUG(dbgSys, "\n Reading virtual address of filename");
	// Lấy tham số tên tập tin từ thanh ghi r4
	int virtAddr = kernel->machine->ReadRegister(4);
	DEBUG(dbgSys, "\n Reading filename.");
	char *fileName = string_User2System(virtAddr);

	if (SysCreateFile(fileName))
		kernel->machine->WriteRegister(2, 0); // trả về cho chương trình người dùng thành công
	else
		kernel->machine->WriteRegister(2, -1); // trả về lỗi cho chương trình người dùng

	delete[] fileName;
	PC_counting();
}

//he thong mo tap tin
void system_OpenFile()
{
	//lay tham so tu thanh ghi r4
	int virtAddr = kernel->machine->ReadRegister(4);
	char *fileName = string_User2System(virtAddr);
	//chuyen du lieu kernel-user

	kernel->machine->WriteRegister(2, SysOpen(fileName)); //tra ve ma so tap tin can mo

	delete[] fileName;
	PC_counting();
}

//he thong dong tap tin
void system_CloseFile()
{
	int id = kernel->machine->ReadRegister(4);
	kernel->machine->WriteRegister(2, kernel->fileSystem->Close(id));  //tra ve ma so tap tin can dong
	PC_counting();
}

void system_ReadFile()
{
	//lay tham so tu cac thanh ghi
	int virtAddr = kernel->machine->ReadRegister(4);
	int charCount = kernel->machine->ReadRegister(5);
	char *buffer = string_User2System(virtAddr, charCount);
	int fileId = kernel->machine->ReadRegister(6);

	DEBUG(dbgSys, "Read " << charCount << " chars from file " << fileId << "\n");

	kernel->machine->WriteRegister(2, kernel->fileSystem->Read(buffer, charCount, fileId)); //tra ve kich thuoc noi dung trong tap tin
	string_System2User(buffer, virtAddr, charCount);

	delete[] buffer;
	PC_counting();
}

void system_WriteFile()
{
	//lay cac tham so
	int virtAddr = kernel->machine->ReadRegister(4);
	int charCount = kernel->machine->ReadRegister(5);
	char *buffer = string_User2System(virtAddr, charCount);
	int fileId = kernel->machine->ReadRegister(6);

	DEBUG(dbgSys, "Write " << charCount << " chars to file " << fileId << "\n");

	kernel->machine->WriteRegister(2, kernel->fileSystem->Write(buffer, charCount, fileId)); //tra ve kich thuoc noi dung can duoc ghi
	string_System2User(buffer, virtAddr, charCount);

	delete[] buffer;
	PC_counting();
}

//He thong đặt con trỏ, đọc file từ vị trí trỏ đến hết file
void system_SeekLocation()
{
	//lay cac tham so
	int seekPos = kernel->machine->ReadRegister(4);
	int fileId = kernel->machine->ReadRegister(5);

	kernel->machine->WriteRegister(2, kernel->fileSystem->Seek(seekPos, fileId)); //tra ve vi tri con tro hoac la kich thuoc noi dung trong tap tin
	PC_counting();
}


void system_RemoveFile()
{
	DEBUG(dbgSys, "\n SC_Remove call ...");
	DEBUG(dbgSys, "\n Reading virtual address of filename");
	// Lấy tham số tên tập tin từ thanh ghi r4
	int virtAddr = kernel->machine->ReadRegister(4);
	DEBUG(dbgSys, "\n Reading filename.");
	char *fileName = string_User2System(virtAddr);

	if (SysRemove(fileName))
		kernel->machine->WriteRegister(2, 0); // trả về cho chương trình người dùng thành công
	else
		kernel->machine->WriteRegister(2, -1); // trả về lỗi cho chương trình người dùng

	delete[] fileName;
	PC_counting();
}

void ExceptionHandler(ExceptionType which)
{
	int type = kernel->machine->ReadRegister(2);

	DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

	switch (which)
	{
	case NoException:							  // return control to kernel
		kernel->interrupt->setStatus(SystemMode); // tra lai quyen dieu khien cho he thong neu khong co cac exception tu usermode
		DEBUG(dbgSys, "Switch to system mode\n");

		return;
		break;

	case PageFaultException:
		DEBUG(dbgAddr, "\n No valid translation found");

		SysHalt();
		ASSERT(FALSE);
		break;

	case ReadOnlyException:
		DEBUG(dbgAddr, "\n Write attempted to page marked read-only");

		SysHalt();
		ASSERT(FALSE);
		break;

	case BusErrorException:
		DEBUG(dbgAddr, "\n Translation resulted invalid physical address");

		SysHalt();
		ASSERT(FALSE);
		break;

	case AddressErrorException:
		DEBUG(dbgAddr, "\n Unaligned reference or one that was beyond the end of the address space");

		SysHalt();
		ASSERT(FALSE);
		break;

	case OverflowException:
		DEBUG(dbgAddr, "\nInteger overflow in add or sub.");

		SysHalt();
		ASSERT(FALSE);
		break;

	case IllegalInstrException:
		DEBUG(dbgAddr, "\n Unimplemented or reserved instr.");

		SysHalt();
		ASSERT(FALSE);
		break;

	case NumExceptionTypes:
		// cerr << "Error " << which << " occurs\n";
		DEBUG(dbgAddr, "\n Number exception types");

		SysHalt();
		ASSERT(FALSE);
		break;

	case SyscallException:
		switch (type)
		{
		case SC_Halt:
			DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

			SysHalt();

			// ASSERTNOTREACHED();
			break;

		case SC_Add:
			DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");

			/* Process SysAdd Systemcall*/
			int result;
			result = SysAdd(/* int op1 */ (int)kernel->machine->ReadRegister(4),
							/* int op2 */ (int)kernel->machine->ReadRegister(5));

			DEBUG(dbgSys, "Add returning with " << result << "\n");
			/* Prepare Result */
			kernel->machine->WriteRegister(2, (int)result);

			PC_counting();

			// ASSERTNOTREACHED();
			return;
			break;

		case SC_ReadNum:
			return system_ReadInt();
			break;

		case SC_PrintNum:
			return system_PrintInt();
			break;

		case SC_ReadChar:
			return system_ReadChar();
			break;

		case SC_PrintChar:
			return system_PrintChar();
			break;

		case SC_RandomNum:
			printf("Random Number is here: \n");
			kernel->machine->WriteRegister(2, (int)random()); // read a random integer (called by random() in C) and put it in register 2
			PC_counting();
			return;
			break;

		case SC_Sub:
			DEBUG(dbgSys, "Sub " << kernel->machine->ReadRegister(4) << " - " << kernel->machine->ReadRegister(5) << "\n");
			int op1, op2;
			op1 = (int)kernel->machine->ReadRegister(4);
			op2 = (int)kernel->machine->ReadRegister(5);
			result = op1 - op2;
			DEBUG(dbgSys, "Sub returning with " << result << "\n");
			kernel->machine->WriteRegister(2, (int)result);

			PC_counting();
			// ASSERTNOTREACHED();
			return;
			break;

		case SC_ReadString:
			return system_ReadString();
			break;

		case SC_PrintString:
		{
			return system_PrintString();
			break;
		}

		case SC_Open:
		{
			return system_OpenFile();
			break;
		}

		case SC_Close:
		{
			return system_CloseFile();
			break;
		}

		case SC_Read:
		{

			return system_ReadFile();
			break;
		}

		case SC_Write:
		{
			return system_WriteFile();
			break;
		}

		case SC_Seek:
		{
			return system_SeekLocation();
			break;
		}

		case SC_Remove:
			return system_RemoveFile();
			break;

		/**
		 * Handle all not implemented syscalls
		 * If you want to write a new handler for syscall:
		 * - Remove it from this list below
		 * - Write system_name()
		 * - Add new case for SC_name
		 */
		case SC_Create:
			return system_CreateFile();
			break;

		default:
			cerr << "Unexpected system call " << type << "\n";
			break;
		}
		break;
	default:
		cerr << "Unexpected user mode exception" << (int)which << "\n";
		break;
	}
	ASSERTNOTREACHED();
}