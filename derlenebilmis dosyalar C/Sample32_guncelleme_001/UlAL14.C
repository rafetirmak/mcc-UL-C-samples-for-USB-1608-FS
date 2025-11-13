/*ULAI14.C****************************************************************

File:                         ULAI14.C

Library Call Demonstrated:    cbSetTrigger()

Purpose:                      Selects the trigger source. This trigger is
                              used to initiate A/D conversion using cbAinScan,
							  with EXTTRIGGER selected
										

Demonstration:                Selects the trigger source.
                              Displays the analog input on two channels.
                              Continuously updates the display
                              until a key is pressed.

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must have software selectable 
                              triggering source and type.
                              must have an A/D converter.
                              Analog signals on two input channels.

Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/

#define STRMAX 100

/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

 int main(void)
{
	/* Variable Declarations */
	int		Row, Col, I, J;
	int		BoardNum = 0;
	int		ULStat = NOERRORS;
	int		LowChan = 0;
	int		HighChan = 1;
	int		NumAIChans;
	int		Range = BIP5VOLTS;
	long	Count = 20;
	long	Rate = 3125;
	HGLOBAL	MemHandle = 0;
	WORD*	ADData = NULL;
	DWORD*	ADData32 = NULL;
	int		Options = FOREGROUND;
	int		TrigType = TRIG_POS_EDGE;
	float	LSB, VoltageRange;
	int		FSCounts;
	USHORT	LowThreshold;
	USHORT	HighThreshold;
	BOOL	HighResAD = FALSE;
	int		ADRes, ATrigRes, ATrigRange;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		ChannelType = ATRIGIN;
	char	trigSource[STRMAX];
	char	BoardName[BOARDNAMELEN];
	float	highVal, lowVal;
	char	rangeName[RANGENAMELEN];
	double	rangeVolts;

	InitUL();	// Set up error handling

	// get the name of the board
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the display screen */
	ClearScreen();
	printf("Demonstration of cbAInScan() with EXTTRIGGER mode using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &LowChan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("Either %s (board %u) does not have analog input \n", BoardName, BoardNum);
		printf("or it does not support analog trigger.\n");
		DisplayMessage(NOERRORS);
		return 0;
	}
	/*
	   Check if the resolution of A/D is higher than 16 bit.
	   If it is, use high resolution library functions.
	*/
	if (ADRes > 16) HighResAD = TRUE;

	if(HighResAD) {		/*  set aside memory to hold data */
		MemHandle = cbWinBufAlloc32(Count);
		ADData32 = (DWORD*) MemHandle;
	} else {
		MemHandle = cbWinBufAlloc(Count);
		ADData = (WORD*) MemHandle;
	}

    if (!MemHandle) {	   /* Make sure it is a valid pointer */
		printf("\nout of memory\n");
        return 0;
	}

    /* 
	Select the trigger source 
    Parameters:
       BoardNum      :the number used by CB.CFG to describe this board
       TrigType      :the type of triggering based on the external trigger
                      source.
       LowThreshold  :Low threshold when trigger input is analog
       HighThreshold :High threshold when trigger input is analog 	 */

	highVal = 1.53F;
	lowVal = 0.1F;
	TrigType = TRIGABOVE;
	ATrigRes = 0;
	ATrigRange = 0;
	rangeVolts;

	strcpy(trigSource, "analog trigger input");
	HighThreshold = 0;
	LowThreshold = 0;
	GetTrigResolution(&ATrigRes, &ATrigRange);
	GetRangeInfo(Range, rangeName, &rangeVolts);

	if (ATrigRes == 0) {
		ULStat = cbFromEngUnits(BoardNum, Range, highVal, &HighThreshold);
		ULStat = cbFromEngUnits(BoardNum, Range, lowVal, &LowThreshold);
	} else {
		//Use the value acquired from the AnalogIO module, since the resolution
		//of the input is different from the resolution of the trigger.
		//Calculate trigger based on resolution returned and trigger range.
		VoltageRange = (float)ATrigRange;
		if (ATrigRange == -1) {
			strcpy(trigSource, "first channel in scan");
		}
		FSCounts = (int)pow(2, ATrigRes);
		LSB = VoltageRange / FSCounts;
		LowThreshold = (USHORT)((lowVal / LSB) + (FSCounts / 2));
		HighThreshold = (USHORT)((highVal / LSB) + (FSCounts / 2));
	}
	TrigType = TRIGABOVE; /* AD conversions are enabled when the external
                             analog trigger makes a transition from below
							 HighThreshold to above. */
	ULStat = cbSetTrigger(BoardNum, TrigType, LowThreshold, HighThreshold);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}
	
	printf("   Waiting for a trigger at %s\n", trigSource);
	printf("   Trigger criterea: signal rising above %-3.2fV.\n", highVal);
	printf("\n   (Ctl - Break to abort.)\n\n");

	/* Collect the values with cbAInScan()
        Parameters:
            BoardNum    :the number used by CB.CFG to describe this board
            LowChan     :low channel of the scan
            HighChan    :high channel of the scan
            Count       :the total number of A/D samples to collect
            Rate        :sample rate in samples per second
            Range        :the Range for the board
            DataBuffer[]:the array for the collected data values
            Options     :data collection options */

	Options = CONVERTDATA | EXTTRIGGER;
	ULStat = cbAInScan(BoardNum, LowChan, HighChan, Count, &Rate,
                                            Range, MemHandle, Options);

	/* display the data */
	for (J = 0; J < 2; J++) {      /* loop through the channels */
		printf("\nThe first 5 values on Channel %u are ", J);
		GetTextCursor(&Col, &Row);
		for (I = 0; I < 5; I++) {  /* loop through the values & print */
			MoveCursor(Col, Row + I);
			if(HighResAD)
				printf("%8u", ADData32[ I * 2 + J]);
			else
				printf("%4u", ADData[ I * 2 + J]);
		}
		printf("\n");
	}
	DisplayMessage(NOERRORS);
	cbWinBufFree(MemHandle);
	return 0;
}
    


