/*ULDI01.C****************************************************************

File:                         ULDI01.C

Library Call Demonstrated:    cbDIn()

Purpose:                      Reads a digital input port.

Demonstration:                Configures the first digital port for 
                              input and reads the value on the port.

Other Library Calls:          cbDConfigPort()
                              cbErrHandling()

Special Requirements:         Board 0 must have a digital input port.

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
    int		PortNum, Direction;
	int		NumPorts, NumBits, FirstBit;
	int		ProgAbility;
    WORD	DataValue;
    float   RevLevel = (float)CURRENTREVNUM;
	int		PortType = PORTIN;
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
	printf("Demonstration of cbDIn() using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumPorts = FindPortsOfType(BoardNum, PortType, &ProgAbility,
		&PortNum, &NumBits, &FirstBit);
	if (NumPorts == 0) {
		printf("%s (board %u) has no compatible digital ports.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}
	if (NumBits > 8) NumBits = 8;
	GetNameOfPortType(PortNum, portTypeStr);

    printf("Press any key to stop reading digital inputs.\n\n");
    printf("\nChange the value read by applying a TTL high or");
    printf("\nTTL low to one of the inputs on %s.\n\n", portTypeStr);

    GetTextCursor(&Col, &Row);

    /* configure first valid port for digital input
        Parameters:
            BoardNum    :the number used by CB.CFG to describe this board.
            PortNum     :the input port (AUXPORT, FIRSTPORTA, etc)
            Direction   :sets the port for input or output */
    PortNum = PortNum;
    Direction = DIGITALIN;
    ULStat = cbDConfigPort(BoardNum, PortNum, Direction);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

    while (!_kbhit()) {
		/* Read the digital input bits and display
           Parameters:
                BoardNum    :the number used by CB.CFG to describe this board
                PortNum     :the input port
                DataValue   :the value read from the port   */
        ULStat = cbDIn(BoardNum, PortNum, &DataValue);
		if (ULStat != NOERRORS) {
			DisplayMessage(ULStat);
			return 0;
		}
        /* display the value collected from the port */
		Sleep(100);
        MoveCursor(Col, Row);
        printf("%s port Value: %u         ", portTypeStr, DataValue);

	}
	dummy = _getch();
	printf("\n");
	DisplayMessage(NOERRORS);
	return 0;
}
 