/*ULCT07.C****************************************************************

File:                         ULCT07.C

Library Call Demonstrated:    Event Counter Functions
                              cbCClear()
                              cbCIn32()

Purpose:                      Operate the counter.

Demonstration:                Reset and reads the counter.

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must have an Event Counter
							  (or Scan Counter that doesn't require
                              configuration) such as a miniLAB 1008, 
							  USB-CTR04, or USB-1208LS.

Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

 int main(void)
{
	/* Variable Declarations */
    int Row, Col;
    int BoardNum = 0;
    int ULStat = 0;
    int CounterNum = 0;
	int numCounters, defaultCtr;
	ULONG Count;
	float RevLevel = (float)CURRENTREVNUM;
	int ctrType = CTREVENT;
	char BoardName[BOARDNAMELEN];
	int dummy;

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
	printf("Demonstration of Event Counter Functions using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	numCounters = FindCountersOfType(BoardNum, ctrType, &defaultCtr);
	if (numCounters == 0) {
		//some scan counters can also work with this example
		ctrType = CTRSCAN;
		numCounters = FindCountersOfType(BoardNum, ctrType, &defaultCtr);
		if (numCounters == 0) {
			printf("%s (board %u) does not have Event counters.\n", BoardName, BoardNum);
			DisplayMessage(NOERRORS);
			return 0;
		}
	}

    printf("NOTE: There must be a TTL signal at counter %u input.\n\n", defaultCtr);
    printf("Press any key to stop reading counter.\n\n");


    /* Reset the starting value to the counter with cbCClear()
        Parameters:
            BoardNum    :the number used by CB.CFG to describe this board
            CounterNum  :the counter to be reset to zero */

	CounterNum = defaultCtr;
    ULStat = cbCClear(BoardNum, CounterNum);

    /* use a loop to keep checking the counter value with cbCIn() */
    GetTextCursor(&Col, &Row);

    while (!_kbhit() && ULStat==NOERRORS) {
		/* Parameters:
            BoardNum    :the number used by CB.CFG to describe this board
            CounterNum  :the counter to be setup
            Count       :the count value in the counter */
        ULStat = cbCIn32(BoardNum, CounterNum, &Count);

        MoveCursor(Col, Row + 1);
        printf("The value of Counter %u is %u     ", CounterNum, Count);
		Sleep(200);
	}
	dummy = _getch();
	printf("\n");
	DisplayMessage(NOERRORS);
	return 0;
}

