/*ULAI11.C****************************************************************

File:                         ULAI11.C

Library Call Demonstrated:    cbToEngUnits()

Purpose:                      Convert a raw data value to a voltage.

Demonstration:                Displays a voltage on a user-specified
                              channel.

Other Library Calls:          cbErrHandling()
                              cbAIn()

Special Requirements:         Board 0 must have an A/D converter.
                              Analog signal on an input channel.
                              If the board is a 16 bit board, see the
                              CONVERT RAW DATA VALUE section for the
                              correct value to use.

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
    int		Chan;
	int		NumAIChans;
	int		Range = BIPPT625VOLTS;
    WORD	DataValue = 0;
	DWORD	DataValue32 = 0;
    float	EngUnits;
	double	HighResEngUnits;
    float   RevLevel = (float)CURRENTREVNUM;
	BOOL	HighResAD = FALSE;
	int		ADRes;
	int		Options = FOREGROUND;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		ChannelType = ANALOGINPUT;
	int		selection;
	int		dummy;
	double	rangeVolts;
	char	BoardName[BOARDNAMELEN];
	char	rangeName[RANGENAMELEN];

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
	printf("Demonstration of voltage conversions using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, 
		ChannelType, &ADRes, &Range, &Chan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("%s (board %u) does not have analog input channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}
	if(ADRes > 16)
		HighResAD = TRUE;

    /* get the A/D channel to sample */
    printf("Enter the channel to display 0 to %u: ", NumAIChans - 1);
	dummy = scanf("%d", &Chan);
	if (Chan > (NumAIChans - 1)) {
		printf("\n\nInvalid channel number. Exiting program.");
		DisplayMessage(NOERRORS);
		return 0;
	}

    printf("\n\nNote: Please make certain that the board you are using supports\n");
    printf("      the Range you are choosing and if it is not a programmable\n");
    printf("      Range that the switches on the board are set correctly.\n\n");
    GetTextCursor(&Col, &Row);
	selection = 4;

    /* collect the sample with cbAIn() */
    while (selection > 0) {
		do
		{ /* select Range */
			MoveCursor(12, 10);
            printf("Please select one of the following ranges(1 to 4):\n\n");
            printf("                           10 VOLTS UNIPOLAR --> 1\n");
            printf("                           10 VOLTS BIPOLAR ---> 2\n");
            printf("                            5 VOLTS UNIPOLAR --> 3\n");
            printf("                            5 VOLTS BIPOLAR ---> 4\n");
            printf("                                       Quit ---> 0\n\n");
			printf("                                Your Choice ---> ");
			dummy = scanf("%i", &selection);
		} while ((selection < 0) || (selection > 4));

        /* Set Range, MaxVal, and MinVal */
        switch (selection)
		{
		case 0:
			MoveCursor(2, 22);
			DisplayMessage(NOERRORS);
			return 0;
		case 1:
			Range = UNI10VOLTS;
			break;
		case 2:
			Range = BIP10VOLTS;
			break;
		case 3:
			Range = UNI5VOLTS;
			break;
		case 4:
			Range = BIP5VOLTS;
			break;
		default:
			break;
		}

        /*Parameters:
            BoardNum    :number used by CB.CFG to describe this board
            Chan        :input channel number
            Range        :Range for the board in BoardNum
            DataValue   :value collected from Chan */

		GetRangeInfo(Range, rangeName, &rangeVolts);
        if (Range >= 0) {
			if(HighResAD) {
				ULStat = cbAIn32(BoardNum, Chan, Range, &DataValue32, Options);
				if (ULStat == NOERRORS) {
					ULStat = cbToEngUnits32(BoardNum, Range, DataValue32, &HighResEngUnits);
					printf("\nThe voltage on channel %d using %s is %.5f   ", 
						Chan, rangeName, HighResEngUnits);
				}
			} else {
				ULStat = cbAIn(BoardNum, Chan, Range, &DataValue);
				if (ULStat == NOERRORS) {
					ULStat = cbToEngUnits(BoardNum, Range, DataValue, &EngUnits);
					printf("\nThe voltage on channel %d using %s is %.2f   ", 
						Chan, rangeName, EngUnits);
				}
			}
			if (ULStat != NOERRORS)
				if (ULStat == BADRANGE)
					printf("\n%s is not a valid range for the %s             ", rangeName, BoardName);
				else
					DisplayMessage(ULStat);
		}
		Range = BIPPT625VOLTS;
	}
	DisplayMessage(NOERRORS);
	return 0;
}

