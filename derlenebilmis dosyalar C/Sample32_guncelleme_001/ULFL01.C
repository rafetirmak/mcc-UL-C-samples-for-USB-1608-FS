/*ULFL01.C****************************************************************

File:                         ULFL01.C

Library Call Demonstrated:    cbFlashLED()

Purpose:                      Flashes the onboard LED for visual identification

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must have an external LED, 
                              (applies to most USB devices).

Copyright (c) 2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/

/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

int main(void)
{
	/* Variable Declarations */
	int		BoardNum = 0;
	int		ULStat = NOERRORS;
	char	BoardName[BOARDNAMELEN];
	char	ch;

	InitUL();	// Set up error handling

	// get the name of the board
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the screen */
	ClearScreen();
	printf("Demonstration of cbFlashLED() using %s\n", BoardName);
	printf("\n This function is intended for USB devices, etc.\n");
	printf("\n Most other devices will return an error.\n\n");
	printf("\n Press any key to flash the LED on the %s.\n", BoardName);
	DisplayMessage(NOERRORS);

	/* collect the sample from the channel until a key is pressed */
	ch = _getch();

	/*
	cbFlashLED()
	Parameters:
		BoardNum    :number used by CB.CFG to describe this board
	*/
	ULStat = cbFlashLED(BoardNum);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	} else
		printf("\n FlashLED call was successful.\n\n");

	DisplayMessage(ULStat);
	return 0;
}

