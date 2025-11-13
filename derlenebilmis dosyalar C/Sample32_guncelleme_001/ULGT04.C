/*ULGT04.C****************************************************************

File:                         ULGT04.C

Library Call Demonstrated:    cbGetBoardName()

Purpose:                      Prints a list of all boards installed in
                              the system.  Prints a list of all supported
                              boards.

Other Library Calls:          cbErrHandling(), cbGetConfig()

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
    int		NumBoards, StartY, x, y, BoardNum;
    char	BoardNameStr[BOARDNAMELEN];
    float   RevLevel = (float)CURRENTREVNUM;
    int		BoardType;
	char	ch;

	/* Declare UL Revision Level */                             
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	// Set up error handling

    /* set up the display screen */
    ClearScreen();
    printf("Demonstration of cbGetBoardName()\n\n");
    printf("List of Installed Boards:\n");

    /* Get the number of boards installed in system */
    cbGetConfig(GLOBALINFO, 0, 0, GINUMBOARDS, &NumBoards);

    /* Print out board type name of each installed board */
    for (BoardNum=0; BoardNum<NumBoards; BoardNum++) {
		/* Get board type of each board */
        cbGetConfig(BOARDINFO, BoardNum, 0, BIBOARDTYPE, &BoardType);
        
        /* If a board is installed */
        if (BoardType > 0) {
			cbGetBoardName(BoardNum, BoardNameStr);
            printf("    Board #%d = %s\n", BoardNum, BoardNameStr);
		}
	}
	printf("\n\n");
    printf("                 --- Press any key to continue ---\n");

    ch = _getch();
    ClearScreen();
    printf("List of supported boards:\n");
    GetTextCursor(&x, &y);
    StartY = y;

    /* Get the fisrt board type in list of supported boards */
    cbGetBoardName(GETFIRST, BoardNameStr);
    printf("%s",BoardNameStr);
    y++;

    do
	{
		/* Get each consecutive board type in list */
        cbGetBoardName(GETNEXT, BoardNameStr);
        MoveCursor(x, y);
        printf("%s", BoardNameStr);
        y++;
        if (y > 20) {
			y = StartY;
            x += 26;
			if (x > 70) {
				MoveCursor(0, 20);
				printf("\n\nMore ...");
				DisplayMessage(NOERRORS);
				ch = _getch();
				x = 0;
				ClearScreen();
			}
		}
	} while (BoardNameStr[0] != '\0');
	MoveCursor(0, 24);
	printf("  List of supported devices complete.\n");
	DisplayMessage(NOERRORS);
	return 0;
}

