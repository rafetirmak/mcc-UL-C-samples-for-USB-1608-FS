/*ULDO02.C****************************************************************

File:                         ULDO02.C

Library Call Demonstrated:    cbDBitOut()

Purpose:                      Sets the state of a single digital output bit.

Demonstration:                Configures the first compatible digital
                              ports (usually FIRSTPORTA) for output and
                              writes a bit value to the port.

Other Library Calls:          cbDConfigPort()
                              cbGetConfig() using DIGITALINFO and BOARDINFO
                              cbErrHandling()

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
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
    int		PortNum, Direction;
	int		NumPorts, NumBits, FirstBit;
	int		ProgAbility;
	int		BitNum;
	int		userBit;
    int		userLevel;
    WORD	BitValue = 0;
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
	printf("Demonstration of cbDBitOut() using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumPorts = FindPortsOfType(BoardNum, PortType, &ProgAbility,
		&PortNum, &NumBits, &FirstBit);
	if (NumPorts == 0) {
		printf("%s (board %u) has no compatible digital ports.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}
	GetNameOfPortType(PortNum, portTypeStr);

	GetTextCursor(&Col, &Row);

    /* configure the first port for digital output
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

	/* get a user value to write to the port */
    userBit = NumBits;
    while (userBit > NumBits - 1) {
		MoveCursor(Col, Row);
        printf("Enter a bit number to set (0 to %u) --> ", NumBits - 1);
        dummy = scanf("%i", &userBit);
	}
	userLevel = 2;
    while (userLevel > 1) {
		MoveCursor(Col, Row+2);
        printf("Set high (1) or low (0)? --> ");
        dummy = scanf("%i", &userLevel);
	}

	/* write the specified value to the specified bit of the first port
	            Parameters:
                BoardNum    :the number used by CB.CFG to describe this board
                PortType    :must be FIRSTPORTA or AUXPORT
                BitNum      :the number of the bit to be set to BitValue
                BitValue    :the value written to the port   
	 */

	BitNum = userBit;
	BitValue = (WORD)userLevel;
	ULStat = cbDBitOut(BoardNum, PortNum, FirstBit + BitNum, BitValue);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	MoveCursor(Col, Row + 4);
	printf("Bit %u of %s was set to logic level %u.\n\n", FirstBit + BitNum, portTypeStr, BitValue);

	DisplayMessage(NOERRORS);

	// Return device to input mode
	Direction = DIGITALIN;
	ULStat = cbDConfigPort(BoardNum, PortNum, Direction);
	return 0;
}

