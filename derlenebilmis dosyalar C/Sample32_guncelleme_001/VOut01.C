/*VOut01.C****************************************************************

File:                         VOut01.C

Library Call Demonstrated:    cbVOut()

Purpose:                      Writes to a D/A Output Channel.

Demonstration:                Sends a voltage value to D/A 0.

Other Library Calls:          cbErrHandling()
                              cbFromEngUnits()

Special Requirements:         Board 0 must have a D/A output port.

Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

int main(void)
{
	/* Variable Declarations */
	int		ch = 0;
    int		Row, Col;
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
    int		Range = BIP5VOLTS;
    int		Chan, ExitFlag;
	int		NumAOChans;
	int		Options = FOREGROUND;
    float	EngUnits, voltsSent;
    float   RevLevel = (float)CURRENTREVNUM;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		ChannelType = ANALOGOUTPUT;
	char	rangeName[RANGENAMELEN];
	double	rangeVolts, fullScalePlus, fullScaleMinus;
	char	BoardName[BOARDNAMELEN];
	int		dummy;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	// get the name of the board
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the display screen */
	ClearScreen();
	printf("Demonstration of cbAOut() using %s.\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAOChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &Chan, &DefaultTrig);
	if (NumAOChans == 0) {
		printf("%s (board %u) does not have analog output channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}

	GetRangeInfo(Range, rangeName, &rangeVolts);
	fullScalePlus = rangeVolts;
	fullScaleMinus = 0;
	if (Range < 99) {
		fullScalePlus = rangeVolts / 2;
		fullScaleMinus = fullScalePlus * -1;
	}
	printf("Current range is %s\n\n", rangeName);
	GetTextCursor(&Col, &Row);

    ExitFlag = FALSE;
	Chan = 0;
	while (!ExitFlag) {
		MoveCursor(0, 2);
		printf("Enter a voltage between %.3f and %.3f: ", fullScaleMinus, fullScalePlus);
		MoveCursor(44, 2);
		dummy = scanf("%f", &EngUnits);
		//ULStat = cbFromEngUnits(BoardNum, Range, EngUnits, &DataValue);

		//ULStat = cbAOut(BoardNum, Chan, Range, DataValue);
		ULStat = cbVOut(BoardNum, Chan, Range, EngUnits, Options);
		if (ULStat != NOERRORS) {
			DisplayMessage(ULStat);
			return 0;
		}
		ClearScreen();
		voltsSent = EngUnits;
		if (EngUnits > fullScalePlus) voltsSent = (float)fullScalePlus;
		if (EngUnits < fullScaleMinus) voltsSent = (float)fullScaleMinus;
		printf("\n  %.3f volts has been sent to D/A 0.\n\n", voltsSent);
		printf("Press Q to quit , any other key to continue:\n ");
		while (!_kbhit()) {}
		ch = _getch();
		if (ch == 'q' || ch == 'Q')
			ExitFlag = TRUE;
		ClearScreen();
	}
	DisplayMessage(NOERRORS);
	return 0;
}

