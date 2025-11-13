/* ULFI01.C=================================================================

File:                         ULFI01.C

Library Call Demonstrated:    cbFileAInScan()

Purpose:                      Scan a range of A/D channels and store the
                              data in a disk file.

Demonstration:                Collects data points from one analog input
                              channel and saves them in a file.

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must an A/D converter.

Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
=========================================================================*/


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

 int main(void)
{
	/* Variable Declarations */
    int		BoardNum = 0;
    int		ULStat = NOERRORS;
    short	LowChan = 0;
	short	HighChan = 0;
	int		defaultChan = 0;
	int		NumAIChans;
	int		Range = BIP5VOLTS;
    long	Count, TotalCount;
    long	PreTrigCount;
    long	NumPoints = 1000;
    long	Rate;
    int		Options = FOREGROUND;
	BOOL	HighResAD = FALSE;
	int		DefaultTrig = TRIG_POS_EDGE;
	char*	FileName;
    float   RevLevel = (float)CURRENTREVNUM;
	int		ChannelType = ANALOGINPUT;
	char	BoardName[BOARDNAMELEN];

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
	printf("Demonstration of cbFileAInScan() using %s\n\n", BoardName);

	// Determine if device is compatible with this example
	NumAIChans = FindAnalogChansOfType(BoardNum, ChannelType, &ADRes, &Range, &defaultChan, &DefaultTrig);
	if (NumAIChans == 0) {
		printf("%s (board %u) does not have analog input channels.\n", BoardName, BoardNum);
		DisplayMessage(NOERRORS);
		return 0;
	}

	LowChan = defaultChan;
	HighChan = defaultChan;
	/*
	Check if the resolution of A/D is higher than 16 bit.
	  If it is, use high resolution library functions.
	*/
	if (ADRes > 16) HighResAD = TRUE;
	/*  set aside memory to hold data */
	if (HighResAD) {
		printf("\nThis function is not compatible with devices with resolution greater than 16-bits.\n");
		printf("\n  The %s resolution is %u-bits.\n", BoardName, ADRes);
		DisplayMessage(NOERRORS);
		return 0;
	}

    /* set up the collection parameters */
    Count = NumPoints;
    FileName = "DEMO.DAT";
    Rate = 1000;                     /* sampling rate (samples per second) */
    Options = CONVERTDATA;

    printf("Collecting %4ld data points...\n\n", NumPoints);
    printf("Rate     = %ld Hz\n", Rate);
    printf("LowChan  = %u\n", defaultChan);
    printf("HighChan = %u\n", defaultChan);
    printf("Options  = %u\n", Options);
    printf("Range     = %u\n", Range);
    printf("FileName = %s\n", FileName);
    printf("Count    = %ld\n", Count);

    /* Collect the values with cbFileAInScan()
      Parameters:
        BoardNum	:the number used by CB.CFG to describe this board
          LowChan	:(defaultChan) first A/D channel of the scan
          HighChan  :(defaultChan) last A/D channel of the scan
          Count     :the total number of A/D samples to collect
          Rate      :sample rate in samples per second
          Range     :the Range for the board
          FileName  :the filename for the collected data values
          Options   :data collection options  */
    ULStat = cbFileAInScan(BoardNum, defaultChan, defaultChan, Count, &Rate, Range, FileName, Options);

    printf("\n%4ld data points were placed in the file: %s.\n\n", Count, FileName);
    ULStat = cbFileGetInfo(FileName, &LowChan, &HighChan, &PreTrigCount, &TotalCount, &Rate, &Range);

    printf("Rate     = %ld Hz\n", Rate);
    printf("LowChan  = %u\n", LowChan);
    printf("HighChan = %u\n", HighChan);
    printf("Options  = %u\n", Options);
    printf("Range     = %u\n", Range);
    printf("FileName = %s\n", FileName);
    printf("Count    = %ld\n", TotalCount);
	DisplayMessage(NOERRORS);
	return 0;
}

