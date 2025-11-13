/*VIn01.C****************************************************************

File:                         VIn01.C

Library Call Demonstrated:    cbVIn()

Purpose:                      Reads an A/D Input Channel as voltage.

Demonstration:                Displays the voltage input on a
                              user-specified channel until a
                              key is pressed.

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must have an A/D converter and support cbVIn().
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
    int		Row,Col;
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
    int		Chan, NumAIChans;
    int		Range = BIP5VOLTS;
	float   EngUnits;
	double  HighResEngUnits;
	float   RevLevel = (float)CURRENTREVNUM;
	BOOL	HighResAD = FALSE;
	int		ADRes = 0;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		Options = FOREGROUND;
	int		ChannelType = ANALOGINPUT;
	char	BoardName[BOARDNAMELEN];
	int		dummy;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	// Set up error handling

	// get the name of the board
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the screen */
	ClearScreen();
	printf("Demonstration of cbVIn() using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &Chan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("%s (board %u) does not have analog input channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* check If the resolution of A/D is higher than 16 bit.
       If it is, then the A/D is high resolution. */
	if(ADRes > 16)
		HighResAD = TRUE;

    printf("Press any key to stop reading data.\n\n");

    /* get the A/D channel to sample */
    printf("Enter the channel to display: ");
    dummy = scanf("%i", &Chan);

	printf("\n\nThe voltage data value on Channel %u is: ", Chan);
    GetTextCursor(&Col, &Row);

    /* collect the sample from the channel until a key is pressed */
    while (!_kbhit()) {
		/*Parameters:
            BoardNum    :number used by CB.CFG to describe this board
            Chan        :input channel number
            Range        :Range for the board in BoardNum
            DataValue   :value collected from Chan */

		if(HighResAD) {
			ULStat = cbVIn32(BoardNum, Chan, Range, &HighResEngUnits, Options);
            MoveCursor(Col, Row);
            printf("%+0.5f ", HighResEngUnits);
		} else {
			ULStat = cbVIn(BoardNum, Chan, Range, &EngUnits, Options);
			MoveCursor(Col, Row);
            printf("%+0.3f ", EngUnits);
		}
		Sleep(200);
	}
	dummy = _getch();
	printf("\n\n");
	DisplayMessage(NOERRORS);
	return 0;
}

