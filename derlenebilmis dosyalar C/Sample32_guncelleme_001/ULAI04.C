/*ULAI04.C****************************************************************

File:                         ULAI04.C

Library Call Demonstrated:    cbAInScan() with cbAConvertData

Purpose:                      Demonstrate the conversion of data values
                              after they have been collected.

Demonstration:                Data points are collected as raw data values and
                              (for devices that store channel tags) are converted
                              from 16-bit composite data to channel numbers and
                              12-bit data values using cbAConvertData().

Other Library Calls:          cbAConvertData()
                              cbGetStatus()
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

int main(void)
{
	/* Variable Declarations */
    int		Row, Col, K;
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
    int		LowChan = 0;
    int		HighChan = 0;
	int		NumAIChans;
	int		Range = BIP5VOLTS;
    short	Status = IDLE;
    long	CurCount;
    long	CurIndex;
    long	NumPoints = 10;
    long	Count = 10;
    long	Rate = 3125;
    WORD	ADData[10];
    int		Options = FOREGROUND;
    float	revision = (float)CURRENTREVNUM;
	int		DefaultTrig = TRIG_POS_EDGE;
	int		ChannelType = ANALOGINPUT;
	char	BoardName[BOARDNAMELEN];
	char	ch;

	/* Declare Revision level of the Universal Library */
    ULStat = cbDeclareRevision(&revision);

	InitUL();	// Set up error handling

	// get the name of the board
	if (!GetNameOfBoard(BoardNum, BoardName)) {
		DisplayMessage(NOERRORS);
		return 0;
	}

	/* set up the display screen */
	ClearScreen();
	printf("Demonstration of cbAInScan() in BACKGROUND mode using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &LowChan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("%s (board %u) does not have analog input channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}
	if (ADRes > 16) {
		printf("The resolution of the %s (board %u) is greater than 16-bit,\n", BoardName, BoardNum);
		printf("so the cbAConvertData() function isn't compatible with it.\n");
		DisplayMessage(NOERRORS);
		return 0;
	}

	printf("This example collects data and converts it using cbAConvertData().\n\n");
    printf("Value: \t\t The 16-bit integer read from the board.\n\n");
    printf("Converted value: This separates channel tags from data for devices\n");
	printf("\t\t that support it but is ignored for most devices.\n\n");
    printf("To start collecting data,");
	DisplayMessage(NOERRORS);
	ch = _getch();
	printf("\n");
	GetTextCursor(&Col, &Row);

    /* 
	Collect the values with cbAInScan() in BACKGROUND mode
        Parameters:
             BoardNum    :the number used by CB.CFG to describe this board
             LowChan     :low channel of the scan
             HighChan    :high channel of the scan
             Count       :the total number of A/D samples to collect
             Rate        :sample rate in samples per second
             Range        :the Range for the board
             ADData[]    :the array for the collected data values
             Options     :data collection options 
	*/
    Options = NOCONVERTDATA + BACKGROUND;
    ULStat = cbAInScan(BoardNum, LowChan, HighChan, 
		Count, &Rate, Range, ADData, Options);

    /* During the BACKGROUND operation, check status */
    Status = RUNNING;
    while (!_kbhit() && Status == RUNNING) {
		/* 
		Check the status of the current background operation
        Parameters:
            BoardNum  :the number used by CB.CFG to describe this board
            Status    :current status of the operation (IDLE or RUNNING)
            CurCount  :current number of samples collected
            CurIndex  :index to the last data value transferred 
            FunctionType: A/D operation (AIFUNCTIOM)
		*/
        ULStat = cbGetStatus(BoardNum, &Status, &CurCount, &CurIndex, AIFUNCTION);
	}

    if (Status == IDLE)
        printf(" Data Collection finished.");
    else
        printf(" Data collection terminated by the user.");
    /* 
	The BACKGROUND operation must be explicitly stopped
       Parameters:
          BoardNum    :the number used by CB.CFG to describe this board
          FunctionType: A/D operation (AIFUNCTIOM)
	*/
    ULStat = cbStopBackground(BoardNum, AIFUNCTION);

    /* show the collected data, then show the converted data */
    MoveCursor(Col, Row + 2);
    printf("Point#    Value \n");
    printf("-------   -------");

    /* show the 16-bit values that were collected */
    for (K = 0; K < (int)Count; K++) {
		MoveCursor(Col, Row + 4 + K);
        printf("  %3u     %5u", K, ADData[K]);
	}

    /* use cbAConvertData to convert the 16-bit values to 12-bit values
    Parameters:
            NumPoints   :the number of data values to convert
            ADData[]    :the array holding the 16-bit values, 12-bit values
                         are returned in the same array
            NULL        :channel tags not returned */
    ULStat = cbAConvertData (BoardNum, NumPoints, ADData, NULL);

    /* display the converted data */
    MoveCursor(Col + 22, Row + 2);
    printf("Converted value\n");
    MoveCursor(Col + 22, Row + 3);
    printf("---------------");

    for (K = 0; K < (int)NumPoints; K++) {
		MoveCursor(Col + 25, Row + 4 + K);
        printf("  %u", ADData[K]);
	}

    MoveCursor(Col, Row + 15);
    printf("Data conversion completed with cbConvertData.");

    printf("\n");
	DisplayMessage(NOERRORS);
	return 0;
}

