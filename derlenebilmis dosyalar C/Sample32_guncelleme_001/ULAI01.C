/*ULAI01.C****************************************************************

File:                         ULAI01.C

Library Call Demonstrated:    cbAIn()

Purpose:                      Reads an A/D Input Channel.

Demonstration:                Displays the analog input on a
                              user-specified channel until a
                              key is pressed.

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must have an A/D converter.
                              Analog signal on an input channel.
                              CB.CFG must be in the current directory.

Copyright (c) 1993-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/

/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

int main(void)
{
    /* Variable Declarations */
    int		Row, Col;
	int		Row2, Col2;
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
    int		Chan, NumAIChans;
    int		Range = BIP5VOLTS;
    WORD	DataValue = 0;
	DWORD	DataValue32 = 0;
	float   EngUnits;
	double  HighResEngUnits;
	float   RevLevel = (float)CURRENTREVNUM;
	BOOL	HighResAD = FALSE;
	int		ADRes = 0;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		Options = FOREGROUND;
	int		ChannelType = ANALOGINPUT;
	char	BoardName[BOARDNAMELEN];
	char	ch;
	int		dummy;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	/* Set up error handling */

	/* get the name of the board */
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the screen */
	ClearScreen();
	printf("Demonstration of cbAIn() using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &Chan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("%s (board %u) does not have analog input channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* 
	   Check if the resolution of A/D is higher than 16 bit.
       If it is, use high resolution library functions. 
	*/
	if(ADRes > 16) HighResAD = TRUE;

    printf("Press any key to stop reading data.\n\n");

    /* get the A/D channel to sample */
    printf("Enter the channel to display: ");
    dummy = scanf("%i", &Chan);

	printf("\n\n The raw data value on Channel %u is:\t", Chan);
    GetTextCursor(&Col, &Row);

	printf("\n The voltage on Channel %u is:     \t", Chan);
	GetTextCursor(&Col2, &Row2);

    /* collect the sample from the channel until a key is pressed */
    while (!_kbhit()) {
		/*
		Parameters:
            BoardNum    :number used by CB.CFG to describe this board
            Chan        :input channel number
            Range       :Range for the board
            DataValue   :value collected from Chan 
		*/

		if (HighResAD) {
			ULStat = cbAIn32(BoardNum, Chan, Range, &DataValue32, Options);
			if (ULStat != NOERRORS) {
				DisplayMessage(ULStat);
				return 0;
			}
			ULStat = cbToEngUnits32(BoardNum, Range, DataValue32, &HighResEngUnits);
			if (ULStat != NOERRORS) {
				DisplayMessage(ULStat);
				return 0;
			}
			MoveCursor(Col, Row);
            printf(" %8lu", DataValue32);
            MoveCursor(Col2, Row2);
            printf("%+-10.6f", HighResEngUnits);
		} else {
			ULStat = cbAIn(BoardNum, Chan, Range, &DataValue);
			if (ULStat != NOERRORS) {
				DisplayMessage(ULStat);
				return 0;
			}
			ULStat = cbToEngUnits(BoardNum, Range, DataValue, &EngUnits);
			if (ULStat != NOERRORS) {
				DisplayMessage(ULStat);
				return 0;
			}
			MoveCursor(Col, Row);
            printf("%6u", DataValue);
            MoveCursor(Col2, Row2);
            printf("%+-10.3f", EngUnits);
		}
		Sleep(200);
	}
	printf("\n");
	ch = _getch();
	DisplayMessage(NOERRORS);
	return 0;
}

