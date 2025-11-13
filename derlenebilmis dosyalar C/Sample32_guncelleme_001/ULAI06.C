/*ULAI06.C****************************************************************

File:                         ULAI06.C

Library Call Demonstrated:    cbAInScan(), continuous BACKGROUND mode

Purpose:                      Scans a range of A/D Input Channels
                              in the background.

Demonstration:                Continuously collect data on Channel 0
                              until a key is pressed.

Other Library Calls:          cbGetStatus()
                              cbStopBackground()
                              cbErrHandling()

Special Requirements:         Board 0 must have an A/D converter.
                              Analog signals on an input channel.


Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

int main(void)
{
	/* Variable Declarations */
    int		Row, Col;
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
    int		LowChan = 0;
    int		HighChan = 0;
	int		NumAIChans;
	int		Range = BIP5VOLTS;
    short	Status = RUNNING;
    long	CurCount;
    long	CurIndex;
    long	Count = 10000;
    long	Rate = 390;
	HGLOBAL	MemHandle = 0;
    WORD*	ADData = NULL;
	DWORD*	ADData32 = NULL;
    int		Options;
    float   revision = (float)CURRENTREVNUM;
	BOOL	HighResAD = FALSE;
	int		ADRes;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		ChannelType = ANALOGINPUT;
	char	BoardName[BOARDNAMELEN];
	char	ch;

   /* Declare Revision level of the Universal Library */
    ULStat = cbDeclareRevision(&revision);

	InitUL();	// Set up error handling

	// get the name of the board
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the display screen */
	ClearScreen();
	printf("Demonstration of cbAInScan() using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &LowChan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("%s (board %u) does not have analog input channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}
	/*
	   Check if the resolution of A/D is higher than 16 bit.
	   If it is, use high resolution library functions.
	*/
	if (ADRes > 16) HighResAD = TRUE;

	/*  set aside memory to hold data */
	if (HighResAD) {
		MemHandle = cbWinBufAlloc32(Count);
		ADData32 = (DWORD*) MemHandle;
	} else {
		MemHandle = cbWinBufAlloc(Count);
		ADData = (WORD*) MemHandle;
	}

    if (!MemHandle) {	/* Make sure it is a valid pointer */
        printf("\n Out of memory\n");
        return 0;
	}

    printf("This sample collects data in the BACKGROUND, CONTINUOUSLY\n");
    /* 
	Collect the values with cbAInScan() in BACKGROUND mode, CONTINUOUSLY
        Parameters:
             BoardNum    :the number used by CB.CFG to describe this board
             LowChan     :low channel of the scan
             HighChan    :high channel of the scan
             Count       :the total number of A/D samples to collect
             Rate        :sample rate in samples per second
             Range        :the Range for the board
             ADData[]    :the array for the collected data values
             Options     :data collection options 
	*/
	Options = CONVERTDATA + BACKGROUND + CONTINUOUS;
    ULStat = cbAInScan(BoardNum, LowChan, HighChan, Count, &Rate, Range,
                        MemHandle, Options);
	if (ULStat != NOERRORS) {
		if (ULStat == BMCONTINUOUSCOUNT) {
			printf("\n  The value for 'Count' (%d) is not compatible with this device.\n", Count);
			printf("  Try changing 'Count' to a multiple of the packet size (see documentation -\n");
			printf("  1024 for many devices, 31 or 32 for some low speed USB devices).\n");
			DisplayMessage(NOERRORS);
			return 0;
		} else {
			DisplayMessage(ULStat);
			return 0;
		}
	}

	printf("\n Collecting data...\n\n");
	GetTextCursor(&Col, &Row);
	MoveCursor(1, 7);
	printf("--------------------------------------------------------------------------------\n");
    printf("| Your program could be doing something useful here while data are collected...|\n");
    printf("--------------------------------------------------------------------------------\n\n");
	printf(" Press any key to stop reading data.\n\n");


    while (!_kbhit() && Status==RUNNING) {
        /* 
		Check the status of the current background operation
        Parameters:
            BoardNum  :the number used by CB.CFG to describe this board
            Status    :current status of the operation (IDLE or RUNNING)
            CurCount  :current number of samples collected
            CurIndex  :index to the last data value transferred 
            FunctionType: A/D operation (AIFUNCTIOM)
		*/
        ULStat = cbGetStatus(BoardNum, &Status, &CurCount, &CurIndex, AIFUNCTION);

        /* check the current status of the background operation */
        if ((Status == RUNNING) && CurCount > 0) {
			MoveCursor(Col, Row);
            printf("  Data point: %6ld   ", CurIndex);
			if(HighResAD)
				printf("  Value: %8d  ", ADData32[CurIndex]);
			else
				printf("  Value: %6d  ", ADData[CurIndex]);
		}
		Sleep(100);
	}
	ch = _getch();
	printf("\n");
    MoveCursor(1, 14);
    printf("Data collection terminated.         \n");

    /* The BACKGROUND operation must be explicitly stopped
        Parameters:
             BoardNum    :the number used by CB.CFG to describe this board 
             FunctionType: A/D operation (AIFUNCTIOM)*/  
    ULStat = cbStopBackground(BoardNum, AIFUNCTION);

    cbWinBufFree(MemHandle);
    //MoveCursor(1, 14);
    printf("\n");
	DisplayMessage(NOERRORS);
	return 0;
}

