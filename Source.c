//Daler_Assignment_4
#define WIN32_LEAN_AND_MEAN 
#define NOATOM
#define NOCLIPBOARD
#define NOCOMM
#define NOCTLMGR
#define NOCOLOR
#define NODEFERWINDOWPOS
#define NODESKTOP
#define NODRAWTEXT
#define NOEXTAPI
#define NOGDICAPMASKS
#define NOHELP
#define NOICONS
#define NOTIME
#define NOIMM
#define NOKANJI
#define NOKERNEL
#define NOKEYSTATES
#define NOMCX
#define NOMEMMGR
#define NOMENUS
#define NOMETAFILE
#define NOMSG
#define NONCMESSAGES
#define NOPROFILER
#define NORASTEROPS
#define NORESOURCE
#define NOSCROLL
#define NOSHOWWINDOW
#define NOSOUND
#define NOSYSCOMMANDS
#define NOSYSMETRICS
#define NOSYSPARAMS
#define NOTEXTMETRIC
#define NOVIRTUALKEYCODES
#define NOWH
#define NOWINDOWSTATION
#define NOWINMESSAGES
#define NOWINOFFSETS
#define NOWINSTYLES
#define OEMRESOURCE
#pragma warning(disable : 4996)

#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <io.h>
#include <WinSock2.h>

#if !defined(_Wp64)
#define DWORD_PTR DWORD
#define LONG_PTR LONG
#define INT_PTR INT
#endif

typedef struct _RECORD { //My Record
	int AccountNumber;
	int AccountBalance;
	HANDLE AccountHandle;
	CRITICAL_SECTION AccUpdating;
} RECORD;

typedef struct _TEMPRECORD { //To hold the temporary balance of an account
	int TempAccNumber;
	int TempAccBalance;
} TEMPRECORD;

void WINAPI Transactions(char*); //prototype for the Transactions function

LARGE_INTEGER FileSize;
RECORD *AccountArray;

//CMMDLINE:  trans1.bin  trans2.bin  trans3.bin  < AccInfo.bin > NewAccountInfo.bin

int _tmain(int argc, LPTSTR argv[])
{
	TEMPRECORD Temp;
	HANDLE *hThreads;
	DWORD BIn;

	//STDinput and STDoutpus variables

	HANDLE STDInput;
	HANDLE STDOutput;

	STDInput = GetStdHandle(STD_INPUT_HANDLE);
	STDOutput = GetStdHandle(STD_OUTPUT_HANDLE);

	GetFileSizeEx(STDInput, &FileSize); //Getting the file size

	int NumberOfRecords = FileSize.QuadPart / 8; // / 8 because each record is equal to 8 bytes

	//Giving memories to hThreads and AccountArray

	hThreads = malloc((argc - 1) * sizeof(HANDLE));
	AccountArray = malloc(NumberOfRecords * sizeof(RECORD));


	for (int x = 0; x < NumberOfRecords; x++)
	{
		ReadFile(STDInput, &Temp, sizeof(TEMPRECORD), &BIn, NULL); //Reading the data into Temp
		AccountArray[x].AccountNumber = Temp.TempAccNumber; //Storing the data from Temp to my array
		AccountArray[x].AccountBalance = Temp.TempAccBalance;
		InitializeCriticalSection(&AccountArray[x].AccUpdating); //initializing the Critical Section
	}

	for (int NumThread = 0; NumThread < argc - 1; NumThread++)
	{
		hThreads[NumThread] = (HANDLE)_beginthreadex(NULL, 0, Transactions, argv[NumThread + 1], CREATE_SUSPENDED, NULL); //Beginning the threads in SUSPENDED MODE
	}

	for (int x = 0; x < argc - 1; x++)
	{
		ResumeThread(hThreads[x]);
	}

	for (int x = 0; x < argc - 1; x++)
	{
		WaitForSingleObject(hThreads[x], INFINITE);
	}

	for (int x = 0; x < argc - 1; x++)
	{
		CloseHandle(hThreads[x]);
	}

	//write new account information

	for (int x = 0; x < NumberOfRecords; x++)
	{
		WriteFile(STDOutput, &AccountArray[x], sizeof(TEMPRECORD), &BIn, NULL);
	}

	return 0;
	
}

void WINAPI Transactions(char* TransFile) //Read in the files 8 bytes at a time
{

	HANDLE TransHandle;
	DWORD BIn;
	TEMPRECORD Temp;

	TransHandle = CreateFile(TransFile,
		GENERIC_READ | GENERIC_WRITE, //child can read the files
		FILE_SHARE_READ | FILE_SHARE_WRITE, NULL,
		OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

	ReadFile(TransHandle, &Temp, sizeof(TEMPRECORD), &BIn, NULL);

	while (BIn != 0)
	{
		EnterCriticalSection(&AccountArray[Temp.TempAccNumber].AccUpdating);
		AccountArray[Temp.TempAccNumber].AccountBalance += Temp.TempAccBalance;

	//	printf("%s changed %d balance to %d \n", TransFile, AccountArray[Temp.TempAccNumber], AccountArray[Temp.TempAccNumber].AccountBalance);
		
		LeaveCriticalSection(&AccountArray[Temp.TempAccNumber].AccUpdating);
		ReadFile(TransHandle, &Temp, sizeof(TEMPRECORD), &BIn, NULL);
	}

}