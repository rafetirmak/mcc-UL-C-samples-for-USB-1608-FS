/*ULDI05.C****************************************************************

File:                         ULDI05.C

Library Call Demonstrated:    cbDBitIn()

Purpose:                      Reads bits from a digital input port.

Demonstration:                Configures the first configurable
                              port (usually FIRSTPORTA) for input and
                              reads the bit value on the port. Unstable bit
                              values are due to the nature of
                              the input ports when left floating.

Other Library Calls:          cbDConfigPort()
                              cbGetConfig() using DIGITALINFO and BOARDINFO
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
	int		BitNum;
    WORD	BitValue;
    float   RevLevel = (float)CURRENTREVNUM;
	int		PortType = PORTIN;
	char	portTypeStr[PORTTYPENAMELEN];
	char	BoardName[BOARDNAMELEN];
	int		bit, dummy;

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
	printf("Demonstration of cbDBitIn() using %s\n\n", BoardName);

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

	printf("Change a bit value read by applying a TTL high or\n");
    printf("TTL low to one of the inputs on %s.\n\n", portTypeStr);
	printf("Press any key to stop reading digital bits.\n\n");

    printf("The first %u bit values are: ", NumBits);
	for (bit = FirstBit; bit < NumBits + FirstBit; bit++)
		printf("%u  ", bit);
	printf("\n");
        
    GetTextCursor(&Col, &Row);

    /* configure the first port (usually FIRSTPORTA) for input
        Parameters:
            BoardNum    :the number used by CB.CFG to describe this board.
            PortNum     :the input port
            Direction   :sets the port for input or output */
    
    Direction = DIGITALIN;
    ULStat = cbDConfigPort(BoardNum, PortNum, Direction);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	Sleep(500);
	while (!_kbhit() && ULStat == NOERRORS) {
		MoveCursor(Col + 27, Row);

        for (BitNum = 0; BitNum < NumBits; BitNum++) {
			/* read the input bits from the ports and display
               Parameters:
                    BoardNum    :the number used by CB.CFG to describe this board
                    PortType    :must be FIRSTPORTA or AUXPORT
                    BitNum      :the input port
                    BitValue    :the value read from the port 
                    
            For boards whose first port is not FIRSTPORTA (such as the USB-ERB08
            and the USB-SSR08) offset the BitNum by FirstBit*/
            ULStat = cbDBitIn(BoardNum, PortNum, FirstBit + BitNum, &BitValue);
			if (ULStat != NOERRORS) {
				DisplayMessage(ULStat);
				return 0;
			}
			printf(" %u ", BitValue);
			Sleep(100);
		}
	}
	dummy = _getch();
	printf("\n");
	DisplayMessage(NOERRORS);
	return 0;
}

