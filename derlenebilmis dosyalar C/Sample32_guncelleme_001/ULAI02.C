/*ULAI02.C****************************************************************

File:                         ULAI02.C

Library Call Demonstrated:    cbAInScan(), FOREGROUND mode

Purpose:                      Scans a range of A/D Input Channels and
                              stores the sample data in an array.

Demonstration:                Displays the analog input on two channels.
                              Continuously updates the display
                              until a key is pressed.

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must have an A/D converter.
                              Analog signals on two input channels.

Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

int main(void)
{
	/* Variable Declarations */
	int		Row, Col, I, J;
	int		BoardNum = 0;
	int		ULStat = NOERRORS;
	int		NumAIChans;
	int		LowChan = 0;
	int		HighChan = 1;
	int		Range = BIP5VOLTS;
	long	Count = 20;
	long	Rate = 500;
	HGLOBAL MemHandle = 0;
	WORD*	ADData = NULL;
	DWORD*	ADData32 = NULL;
	int		Options = FOREGROUND;
	float   RevLevel = (float)CURRENTREVNUM;
	BOOL	HighResAD = FALSE;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		ADRes;
	int		ChannelType = ANALOGINPUT;
	char	BoardName[BOARDNAMELEN];

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
	printf("Demonstration of cbAInScan() in FOREGROUND mode using %s\n\n", BoardName);

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

    if (!MemHandle) {	   /* Make sure it is a valid pointer */
        printf("\n Out of memory\n");
        return 0;
	}

    /* 
	Collect the values with cbAInScan()
        Parameters:
            BoardNum    :the number used by CB.CFG to describe this board
            LowChan     :low channel of the scan
            HighChan    :high channel of the scan
            Count       :the total number of A/D samples to collect
            Rate        :sample rate in samples per second
            Range       :the Range for the board
            MemHandle	:pointer to the array for the collected data values
            Options     :data collection options 
	*/

    Options = CONVERTDATA;
    ULStat = cbAInScan(BoardNum, LowChan, HighChan, Count, &Rate, Range, MemHandle, Options);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

    /* display the data */
    for (J = 0; J < 2; J++) {	/* loop through the channels */
		printf("\n The first 5 values on Channel %u are:", J);
        GetTextCursor(&Col, &Row);

        for (I = 0; I < 5; I++) {	/* loop through the values & print */
			MoveCursor(Col, Row + I);
			if(HighResAD)
				printf("\t%8lu", ADData32[I * 2 + J]);
			else
				printf("\t%4u", ADData[I * 2 + J]);
		}
        printf("\n");
	}
	cbWinBufFree(MemHandle);
	DisplayMessage(NOERRORS);
	return 0;
}
