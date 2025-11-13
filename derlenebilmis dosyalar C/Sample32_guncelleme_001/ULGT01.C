/*ULGT01.C****************************************************************

File:                         ULGT01.C

Library Call Demonstrated:    cbGetErrMsg()

Purpose:                      Returns the error message associated
                              with an error code.

Demonstration:                An invalid board# is generated and the
                              associated message is retrieved.

Other Library Calls:          cbErrHandling()

Special Requirements:         ---

Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

int main(void)
{
	/* Variable Declarations */
	int		ULStat = NOERRORS;
	int		ECode, Chan, BoardNum, Gain;
	WORD	DataValue;
	char	ErrMessage[80];
	float	RevLevel = (float)CURRENTREVNUM;
	char	BoardName[BOARDNAMELEN];
	char	ch;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	// Set up error handling

	// get the name of the board
	BoardNum = 0;
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the display screen */
	ClearScreen();
	printf("Demonstration of cbGetErrMsg()\n\n");
	printf("Press any key to start.\n\n");
	ch = _getch();

	/* generate an error by passing an invalid variable to a routine */
	printf("Generating an error...\n");
	BoardNum = 99;
	Chan = 0;
	Gain = BIP5VOLTS;
	ULStat = cbAIn(BoardNum, Chan, Gain, &DataValue);
	/* 
	Trap the error and determine the message to print
	Parameters:
        ULStat		:the error code returned by the invalid call statement
        ErrMessage	:the text of the error message associated with ULStat
                     must be dimensioned to at least the length of the
                     longest message 
	*/
	ECode = cbGetErrMsg(ULStat, ErrMessage);

    printf("\a\n");
    printf("The following error was detected:\n");
    printf("\nError Code: %u\n", ULStat);
    printf("Message:  %s", ErrMessage);

    MoveCursor(1, 12);
    printf("\n");
	DisplayMessage(NOERRORS);
	return 0;
}

