/*ULAI13.C****************************************************************

File:                         ULAI13.C

Library Call Demonstrated:    cbAInScan(), sampling mode options.

Purpose:                      Scans a range of A/D Input Channels and
                              stores the sample data in an array using
                              a user specified sampling mode.

Demonstration:                Displays the analog input on two channels.

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must have an A/D converter.
                              Analog signals on two input channels.
                              board must support the mode selected.

Copyright (c) 1993-2020, Measurement Computing Corp.
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
    int		LowChan = 0;
    int		HighChan = 1;
	int		NumAIChans;
	int		SampMode = -1;
    int		Range = BIP5VOLTS;
    long	Count = 2048;
    long	Rate = 3125;
	int		Options = FOREGROUND;
	HGLOBAL	MemHandle = 0;
    WORD*	ADData = NULL;
	DWORD*	ADData32 = NULL;
    float	EngUnits;
	double	HighResEngUnits;
    float   RevLevel = (float)CURRENTREVNUM;
	BOOL	HighResAD = FALSE;
	int		ADRes;
	int		dummy;
	int		DefaultTrig = TRIG_POS_EDGE;
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
	printf("Demonstration of cbAInScan() with various sampling mode options using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &LowChan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("%s (board %u) does not have analog input channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}
	if(ADRes > 16)
		HighResAD = TRUE;

	/*  set aside memory to hold data */
	if(HighResAD) {
		MemHandle = cbWinBufAlloc32(Count);
		ADData32 = (DWORD*) MemHandle;
	} else {
		MemHandle = cbWinBufAlloc(Count);
		ADData = (WORD*) MemHandle;
	}

    if (!MemHandle) {	/* Make sure it is a valid pointer */
		printf("\nout of memory\n");
        exit(1);
	}

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

    while (1) {  /* infinite loop */
		MoveCursor(4,4);
        /* select mode */
        while (SampMode < 0 || SampMode > 3) {
			MoveCursor(4,4);
            printf("Please select the sampling mode(1 to 3):\n\n");
            printf("                                SINGLEIO --> 1\n");
            printf("                                DMAIO -----> 2\n");
            printf("                                BLOCKIO ---> 3\n");
            printf("                                   Quit ---> 0\n\n");
            printf("                             Your Choice -->  ");
            GetTextCursor(&Col, &Row);
            MoveCursor(--Col, Row);
            dummy = scanf("%d", &SampMode);
            printf(" ");
		}

        if (SampMode==0) {
			cbErrHandling(DONTPRINT, DONTSTOP);
			break;
		} else if (SampMode==1)
			Options = CONVERTDATA + SINGLEIO;
		else if (SampMode==2)
			Options = CONVERTDATA + DMAIO;
        else if (SampMode==3)
            Options = CONVERTDATA + BLOCKIO;
        
        ULStat = cbAInScan(BoardNum, LowChan, HighChan, Count, &Rate,
			Range, MemHandle, Options);
        
        if (ULStat > 0) {	//erase the last selection and try aRange
			MoveCursor(Col, Row);
			printf(" ");
			
			SampMode = -1;
			continue;
		}

        MoveCursor(1, 15);
        SampMode = -1;

        /* display the data */
        for (J = 0; J < 2 && !ULStat; J++) {       /* loop through the channels */
			printf("\nThe first 5 values on Channel %u are ", J);
            GetTextCursor(&Col, &Row);

            for (I = 0; I < 5; I++) {  /* loop through the values & print */
				MoveCursor(Col, Row + I);
				if(HighResAD) {
					ULStat = cbToEngUnits32(BoardNum, Range, ADData32[I*2+J], &HighResEngUnits);
					printf("%.5f", HighResEngUnits);
				} else {
					ULStat = cbToEngUnits(BoardNum, Range, ADData[I*2+J], &EngUnits);
					printf("%.2f", EngUnits);
				}
			}
			printf("\n");
		}
        GetTextCursor(&Col, &Row);
	}

    cbWinBufFree(MemHandle);
	DisplayMessage(NOERRORS);
	return 0;
}

