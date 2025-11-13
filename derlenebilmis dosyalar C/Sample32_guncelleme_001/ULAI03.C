/*ULAI03.C****************************************************************

File:                         ULAI03.C

Library Call Demonstrated:    cbAInScan(), BACKGROUND mode

Purpose:                      Scans a range of A/D Input Channels
                              in the background.

Demonstration:                Displays the analog input on one channel.
                              Calls cbGetStatus to determine the status
                              of the background operation. Updates the
                              display until a key is pressed.

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
	int		NumAIChans;
	int		LowChan = 0;
    int		HighChan = 0;
    int		Range = BIP5VOLTS;
    short	Status = IDLE;
    long	CurCount;
    long	CurIndex;
    int		Count = 5000;
	int		PrnNum;
	long	Rate = 390;
    int		Options;
	HGLOBAL	MemHandle = 0;
    WORD*	ADData = NULL;
	DWORD*	ADData32 = NULL;
    float   RevLevel = (float)CURRENTREVNUM;
	BOOL	HighResAD = FALSE;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		ADRes;
	int		ChannelType = ANALOGINPUT;
	char	BoardName[BOARDNAMELEN];
	int		numToPrint;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	// Set up error handling

	// get the name of the board
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the display screen */
	ClearScreen();
	printf("Demonstration of cbAInScan() in BACKGROUND mode using %s\n\n", BoardName);

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

    printf(" %d data points are being collected in the background. This program\n", Count);
    printf(" could be doing many other things right now, but it will display every\n");
    printf(" 1000th data value collected.\n\n");
    printf(" 10000 Points Being Collected:\n\n");
    printf(" Press any key to stop reading data.\n\n");
    printf(" Point #   Value\n");
    printf("-------   ------\n");
    GetTextCursor(&Col, &Row);

    /* 
	Collect the values with cbAInScan() in BACKGROUND mode
        Parameters:
             BoardNum    :the number used by CB.CFG to describe this board
             LowChan     :low channel of the scan
             HighChan    :high channel of the scan
             Count       :the total number of A/D samples to collect
             Rate        :sample rate in samples per second
             Range       :the Range for the board
             MemHandle   :pointer to the array for the collected data values
             Options     :data collection options 
	*/
    Options = CONVERTDATA + BACKGROUND;
    ULStat = cbAInScan(BoardNum, LowChan, HighChan, Count, &Rate, Range, MemHandle, Options);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

    /* During the BACKGROUND operation, check the status, print every ten values */
    Status = RUNNING;
	PrnNum = Count / 10;
	numToPrint = 0;
    while (!_kbhit() && Status == RUNNING) {
		/* 
		Check the status of the current background operation
        Parameters:
            BoardNum		:the number used by CB.CFG to describe this board
            Status			:current status of the operation (IDLE or RUNNING)
            CurCount		:current number of samples collected
            CurIndex		:index to the last data value transferred 
            FunctionType	:A/D operation (AIFUNCTIOM)
		*/
        ULStat = cbGetStatus(BoardNum, &Status, &CurCount, &CurIndex, AIFUNCTION);
        
        if (CurCount > 0) {		/* Check if there are data in the buffer */
			MoveCursor(Col + 25, Row - 2);
            printf(" Samples collected:  %ld", CurCount);
            if (CurCount > numToPrint) {
				if (CurCount > 0) {
					MoveCursor(Col, Row + (numToPrint / PrnNum));
					if(HighResAD)
						printf("  %4d    %8u",  numToPrint, ADData32[numToPrint]);
					else
						printf("  %4d    %5u",  numToPrint, ADData[numToPrint]);
				}
				numToPrint += PrnNum;
			}
		}
		Sleep(200);
	}

	MoveCursor(2, 24);
    if (Status == IDLE)
		printf("Data Collection finished.");
	else {
		char ch;
		ch = _getch();
		printf("Data collection interrupted by user.\n");
		DisplayMessage(NOERRORS);
	}
    /* 
	The BACKGROUND operation must be explicitly stopped
        Parameters:
             BoardNum    :the number used by CB.CFG to describe this board 
             FunctionType: A/D operation (AIFUNCTIOM)
	*/  
    ULStat = cbStopBackground(BoardNum,AIFUNCTION);

    printf("\n");
    cbWinBufFree(MemHandle);
	DisplayMessage(NOERRORS);
	return 0;
}

