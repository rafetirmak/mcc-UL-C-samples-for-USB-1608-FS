/*ULAI07.C****************************************************************

File:                         ULAI07.C

Library Call Demonstrated:    cbATrig()

Purpose:                      Waits for a specified analog input channel
                              go above or below a specified value.

Demonstration:                Reads the analog input on a user-specified
                              channel until the user-specified trigger
                              value is detected.

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must have an A/D converter.

Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

WORD GetTrigCounts(int bd, int Range, float val);

int main(void)
{
	/* Variable Declarations */
    char	TrigKind;
    int		Row, Col;
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
    int		Chan, Range;
	int		NumAIChans;
	WORD	DataValue;
    int		TrigType = TRIG_POS_EDGE;
    WORD	TrigValue;
    float	EngUnits;
    float	RevLevel = (float)CURRENTREVNUM;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		ChannelType = ANALOGINPUT;
	int		dummy;
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
	printf("Demonstration of cbATrig() using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &Chan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("%s (board %u) does not have analog input channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}
	if (ADRes > 16) {
		printf("The resolution of the %s (board %u) is greater than 16-bit,\n", BoardName, BoardNum);
		printf("so the cbATrig() function isn't compatible with it.\n");
		DisplayMessage(NOERRORS);
		return 0;
	}

    GetTextCursor(&Col, &Row);

    MoveCursor(Col, Row + 1);
    printf("Enter the channel to display:      ");
    MoveCursor(Col + 31, Row + 1);
	dummy = scanf("%i", &Chan);
    MoveCursor(Col, Row + 3);
    printf("Enter the threshold value (-5 to +5V):     ");
    MoveCursor(Col + 39, Row + 3);
	dummy = scanf("%f", &EngUnits);

	/* convert voltage to counts */
	Range = BIP5VOLTS;

	TrigValue = GetTrigCounts(BoardNum, Range, EngUnits);

	/* if the value is not at the extremes, ask which side to trigger */
    MoveCursor(Col, Row + 5);
    printf("Should the data value be ABOVE or BELOW this threshold (A/B): ");
    TrigKind = (char)_getch();
    MoveCursor(Col + 62, Row + 5);
    printf("%c", TrigKind);

    switch (TrigKind) {
	case 'A':
	case 'a':
		TrigType = TRIGABOVE;
		break;
	case 'B':
	case 'b':
		TrigType = TRIGBELOW;
		break;
	default:
		break;
	}

    /* monitor the channel with cbATrig()
        Parameters:
            BoardNum    :the number used by CB.CFG to describe this board
            Chan        :the input channel number
            TrigType    :specifies whether the trigger is to be above or
                         below TrigValue
            TrigValue   :the threshold value that will cause the trigger
            Range       :the Range value
            DataValue   :the input value read from Chan */

	MoveCursor(Col , Row + 7);
    printf("Waiting for the trigger value...");
	printf(" (Press Ctl-C to abort)");

    ULStat = cbATrig(BoardNum, Chan, TrigType, TrigValue, Range, &DataValue);

    MoveCursor(Col, Row + 9);
    printf("The value that caused the trigger was %u ", DataValue);

    printf("\n");
	DisplayMessage(NOERRORS);
	return 0;
}


/***************************************************************************
*
* Name:     GetTrigCount   
* Arguments: bd - board or device number
*            val - trigger value in engineering units
*            Range - range 
* Returns:   ---
*
* Converts the trigger value in engineerin units to binary counts
*
***************************************************************************/
WORD GetTrigCounts(int bd, int Range, float val)
{
   WORD		counts = 0;
   float	fcounts = 0.0f;
   USHORT	FSCounts = 0;
   float	FSEngUnits = 0.0f;
   int		ulstat = NOERRORS;

   //test if Range is bipolar or unipolar
   ulstat = cbToEngUnits(bd, Range, FSCounts, &FSEngUnits);
   if (FSEngUnits < 0.0f) {
	   //'Range' is bipolar
	   //now, test bitness
	   FSCounts = 0x0fff;  //max 12-bit value
	   ulstat = cbToEngUnits(bd, Range, FSCounts, &FSEngUnits);
	   if (FSEngUnits<0.0f) {
		   //must be 16-bit A/D...
		   FSCounts = 0xffff;
		   ulstat = cbToEngUnits(bd, Range, FSCounts, &FSEngUnits);
	   }

	   fcounts = (FSCounts / 2.0f) * (1.0f + val / FSEngUnits);
	   counts = (WORD)fcounts;
   } else {
	   //'Range' is unipolar
	   // now, test bitness...
	   FSCounts = 0x0fff; //max 12-bit value
	   ulstat = cbToEngUnits(bd, Range, FSCounts, &FSEngUnits);
	   ulstat = cbToEngUnits(bd, Range, (USHORT)(FSCounts + 0x1000), &fcounts);
	   if (fcounts >= 2.0f * FSEngUnits) {
		   //must be 16-bit A/D...
		   FSCounts = 0xffff; //max 16-bit value
		   ulstat = cbToEngUnits(bd, Range, FSCounts,&FSEngUnits);
	   }
	   fcounts = FSCounts * val / FSEngUnits;
	   counts = (WORD)fcounts;
   }
   
   if (counts > FSCounts)
	   counts = FSCounts;
   return counts;
}
