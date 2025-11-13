/*ULDO04.C****************************************************************

File:                         ULDO04.C

Library Call Demonstrated:    cbDOut()

Purpose:                      Writes a value to a digital port.

Demonstration:                Writes a selected value to a port.

Other Library Calls:          cbErrHandling()
                              cbGetConfig() using DIGITALINFO
                              cbDConfigPort()

Special Requirements:         Board 0 must have a digital output port.


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
	int		userRow, userCol;
	int		BoardNum = 0;
	int		ULStat = NOERRORS;
	int		PortNum, ExitFlag;
    int		Direction;
	USHORT	DataValue;
	int		NumPorts, NumBits, FirstBit;
	int		ProgAbility, userValue;
	int		maxValue;
	float   RevLevel = (float)CURRENTREVNUM;
	int		PortType = PORTOUT;
	char	portTypeStr[PORTTYPENAMELEN];
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

	/* set up the display screen */
	ClearScreen();
	printf("Demonstration of cbDOut() using %s\n\n", BoardName);
	GetTextCursor(&Col, &Row);

	// Determine if device is compatible with this example
	NumPorts = FindPortsOfType(BoardNum, PortType, &ProgAbility,
		&PortNum, &NumBits, &FirstBit);
	if (NumPorts == 0) {
		printf("%s (board %u) has no compatible digital ports.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}
	//get the maximum value for selected port
	maxValue = (int)pow(2, NumBits) - 1;
	GetNameOfPortType(PortNum, portTypeStr);

	/* configure the first digital port (usually FIRSTPORTA) for output
		Parameters:
			BoardNum    :the number used by CB.CFG to describe this board.
			PortNum     :the output port
			Direction   :sets the port for input or output */

	Direction = DIGITALOUT;
	ULStat = cbDConfigPort(BoardNum, PortNum, Direction);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	Sleep(500);
	ExitFlag = FALSE;
    while (ExitFlag == FALSE) {
		/* get a user value to write to the port */
        MoveCursor(Col, Row);
        printf("Enter a value to send to %s (0 to %u or -1 to quit): --> ", portTypeStr, maxValue);
		GetTextCursor(&userCol, &userRow);
		dummy = scanf("%d", &userValue);
        if (userValue < maxValue + 1) {
			MoveCursor(userCol, userRow);
			printf("      ");
		}

        if (userValue >= 0) {
			DataValue = (USHORT)userValue;
			/* write the value to the port
                Parameters:
                    BoardNum    :the number used by CB.CFG to describe this board
                    PortNum     :the output port
                    DataValue   :the value written to the port   */
            ULStat = cbDOut(BoardNum, PortNum, DataValue);

            MoveCursor(Col, Row + 2);
            printf("The value %u was written to %s.    ", DataValue, portTypeStr);
		} else
			ExitFlag = TRUE;
	}

	printf("\n\n");
	DisplayMessage(NOERRORS);

	// Return device to input mode
	Direction = DIGITALIN;
	ULStat = cbDConfigPort(BoardNum, PortNum, Direction);
	return 0;
}

