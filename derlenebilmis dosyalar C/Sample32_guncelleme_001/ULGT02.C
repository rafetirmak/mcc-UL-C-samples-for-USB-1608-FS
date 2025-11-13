/*ULGT02.C****************************************************************

File:                         ULGT02.C

Library Call Demonstrated:    cbGetStatus()

Purpose:                      Returns the status of a background
                              operation currently running.

Demonstration:                Uses an analog input scan for a background
                              operation. Updates the status of the data
                              collection until <ESC> is pressed.

Other Library Calls:          cbAInScan()
                              cbStopBackground()
                              cbErrHandling()

Special Requirements:         Board 0 must have an A/D converter.
                              Analog signal on an input channel.


Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

#define ESC_KEY 27

 int main(void)
{
	/* Variable Declarations */
    int		Row, Col, j, NumLines, ExitFlag;
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
    int		LowChan = 0;
    int		HighChan = 0;
	int		NumAIChans;
    int		Range = BIP5VOLTS;
    short	Status = IDLE;
    long	CurCount;
    long	CurIndex;
    long	Count = 100;
    long	Rate = 10;
    char*	StatusText;
    int		PressedKey;
	HGLOBAL	MemHandle = 0;
	WORD*	ADData = NULL;
	DWORD*	ADData32 = NULL;
	int		Options = FOREGROUND;
	float   revision = (float)CURRENTREVNUM;
	BOOL	HighResAD = FALSE;
	int		ADRes;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		ChannelType = ANALOGINPUT;
	char	BoardName[BOARDNAMELEN];

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&revision);

	InitUL();	// Set up error handling

	// get the name of the board
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the display screen */
	ClearScreen();
	printf("Demonstration of cbGetStatus() using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &LowChan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("%s (board %u) does not have analog input channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}
	/*
	   Check if the resolution of A/D is higher than 16 bit.
	   If it is, use high resolution library functions.
	*/
	if (ADRes > 16) HighResAD = TRUE;

	/*  set aside memory to hold data */
	if (HighResAD) {
		MemHandle = cbWinBufAlloc32(Count);
		ADData32 = (DWORD*)MemHandle;
	}
	else {
		MemHandle = cbWinBufAlloc(Count);
		ADData = (WORD*)MemHandle;
	}

	if (!MemHandle) {	/* Make sure it is a valid pointer */
		printf("\n Out of memory\n");
		return 0;
	}

    printf("Press <ESC> to quit. <Spacebar> to stop/restart data collection.\n\n");
    printf("%ld data points are being collected in the background with cbAInScan().\n", Count);
    printf("\nData Points Collected:\n\n");
    printf("Point #   Value     Status:\n");
    printf("-------   ------\n");
    GetTextCursor(&Col, &Row);

    /* Collect the values with cbAInScan() in BACKGROUND mode
        Parameters:
             BoardNum    :the number used by CB.CFG to describe this board
             LowChan     :low channel of the scan
             HighChan    :high channel of the scan
             Count       :the total number of A/D samples to collect
             Rate        :sample rate in samples per second
             Range        :the Range for the board
             ADData[]    :the array for the collected data values
             Options     :data collection options */

    Options = CONVERTDATA + BACKGROUND;

CollectData:
    ULStat = cbAInScan(BoardNum, LowChan, HighChan, Count, &Rate,
                        Range, MemHandle, Options);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

    StatusText = "Running";
    MoveCursor(Col + 27, Row - 2);
    printf(StatusText);

    /* During the BACKGROUND operation, check the status, print every ten values */
    ExitFlag = FALSE;
    while (!ExitFlag) {
		/* check the status of the current background operation
        Parameters:
            BoardNum  :the number used by CB.CFG to describe this board
            Status    :current status of the operation (IDLE or RUNNING)
            CurCount  :current number of samples collected
            CurIndex  :index to the last data value transferred 
            FunctionType: A/D operation (AIFUNCTIOM)*/
        ULStat = cbGetStatus(BoardNum, &Status, &CurCount, &CurIndex, AIFUNCTION);
		if (ULStat != NOERRORS) {
			DisplayMessage(ULStat);
			return 0;
		}
		if (CurCount > 0) {
			MoveCursor(Col + 25, Row - 4);
            printf(" %lu  ", CurCount);
            if (!(CurIndex % 10)) {      /* display every tenth data point */
				if (CurIndex > 0) {
					MoveCursor(Col, Row - 1 + (int) CurIndex / 10);
					if (HighResAD)
						printf("   %3u     %4u ", (int)CurIndex, ADData32[(int)CurIndex - 1]);
					else
						printf("   %3u     %4u ",  (int) CurIndex, ADData[(int) CurIndex - 1]);
				}
			}
		}

        PressedKey = 0;
        if (_kbhit())
            PressedKey = _getch();

        /* check to see if <ESC> was pressed */
        if (PressedKey == ESC_KEY)
            ExitFlag = TRUE;

        /* check to see if <Spacebar> was pressed */
		NumLines = 0;
        if (PressedKey == ' ') {
			if (Status == RUNNING) {
				ULStat = cbStopBackground(BoardNum,AIFUNCTION);
				StatusText = "Idle   ";
                MoveCursor(Col + 27, Row - 2);
                printf(StatusText);
                NumLines = (int) CurIndex/10;
			}

            if (Status == IDLE) {
				/* clear any old numbers */
                for (j = 0; j < NumLines + 1; j++) {
					MoveCursor(Col, Row + j);
					printf("                ");
				}
				MoveCursor(Col + 25, Row - 4);
				printf("      ");
				goto CollectData;
			}
		}
		if (Status == IDLE) {
			StatusText = "Idle   ";
            MoveCursor(Col + 27, Row - 2);
            printf(StatusText);
			MoveCursor(Col, Row + (int) CurIndex / 10);
			if (CurIndex > 0) {
				if (HighResAD)
					printf("   %3u     %4u ", (int)CurIndex, ADData32[(int)CurIndex - 1]);
				else
					printf("   %3u     %4u ",  (int) CurIndex, ADData[(int) CurIndex - 1]);
			}
			MoveCursor(1, 20);
			if (CurCount < 100)
				printf("Interrupted by user...\n"); 
			else
				printf("Complete... \n"); 
			printf(" Press <ESC> or <Spacebar>.\n");
			DisplayMessage(NOERRORS);
			PressedKey = _getch();
			if (PressedKey == ESC_KEY)
				ExitFlag = TRUE;
			else
				PressedKey = ' ';
			/* clear any old numbers */
            for (j = 0; j < 12; j++) {
				MoveCursor(Col, Row + j);
				printf("                           ");
			}
			printf("\n                               ");
			MoveCursor(Col + 25, Row - 4);
 			printf("      ");
			if (PressedKey == ' ') {
				ULStat = cbStopBackground(BoardNum, AIFUNCTION);
				Sleep(100);
				goto CollectData;
			}
		}
		Sleep(100);
	}

    /* the BACKGROUND operation must be explicitly stopped
       Parameters:
          BoardNum		:the number used by CB.CFG to describe this board 
          FunctionType	:A/D operation (AIFUNCTION)*/  
    ULStat = cbStopBackground(BoardNum, AIFUNCTION);
	//ch = _getch();
	ClearScreen();
    printf("\n\n Background operation stopped.\n\n");
	DisplayMessage(NOERRORS);
	return 0;
}

