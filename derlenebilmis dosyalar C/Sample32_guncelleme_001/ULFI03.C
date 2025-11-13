/*ULFI03.C=================================================================

File:                         ULFI03.C

Library Call Demonstrated:    File Operations:
                              cbFilePretrig()
                              cbFileRead()
                              cbFileGetInfo()

Purpose:                      Stream data continuously to a streamer file
                              until a trigger is received, continue data
                              streaming until total number of samples minus
                              the number of pretrigger samples is reached.

Demonstration:                Creates a file and scans analog data to the
                              file continuously, overwriting previous data.
                              When a trigger is received, acquisition stops
                              after (TotalCount& - PreTrigCount&) samples
                              are stored. Displays the data in the file and
                              the information in the file header. Prints
                              data from Pretrigger-4 to Pretrigger+6.

Other Library Calls:          cbErrHandling()

Special Requirements:         Board 0 must have an A/D converter.

Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
========================================================================== */


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

 int main(void)
{
	/* Variable Declarations */
    int		I;
    int		ULStat = NOERRORS;
    int		BoardNum = 0;
    long	TotalCount = 2000;
    long	PreTrigCount = 200;
    long	Rate = 1000;
    long	FirstPoint, NumPoints;
    short	HighChan, LowChan;
	int		defaultChan = 0;
	int		NumAIChans;
	int		Options = FOREGROUND;
	int		Range = BIP5VOLTS;
    char*	FileName;
	HGLOBAL	MemHandle;
	WORD*	DataBuffer = NULL;
	BOOL	HighResAD = FALSE;
	int		DefaultTrig = TRIG_POS_EDGE;
	float	RevLevel = (float)CURRENTREVNUM;
	float	engUnits;
	int		ChannelType = ANALOGINPUT;
	char	BoardName[BOARDNAMELEN];
	char	ch;

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
	printf("Demonstration of File Operations cbFilePretrig() \n");
	printf("and cbFileGetInfo() using %s device.\n\n", BoardName);

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

	LowChan = defaultChan;
	HighChan = defaultChan;
	/*  set aside memory to hold data */
	if (HighResAD) {
		printf("\nThis function is not compatible with devices with resolution greater than 16-bits.\n");
		printf("\n  The %s resolution is %u-bits.\n", BoardName, ADRes);
		DisplayMessage(NOERRORS);
		return 0;
	}
	MemHandle = cbWinBufAlloc(TotalCount);
	DataBuffer = (WORD*)MemHandle;

	if (!MemHandle) {	   /* Make sure it is a valid pointer */
		printf("\n Out of memory\n");
		return 0;
	}
	if (DefaultTrig == TRIGABOVE) {
		//The default trigger configuration for most devices is
		//rising edge digital trigger, but some devices do not
		//support this type for pretrigger functions.
		short MidScale;
		MidScale = (short)(pow(2, ADRes) / 2) - 1;
		ULStat = cbSetTrigger(BoardNum, DefaultTrig, MidScale, MidScale);
		if (ULStat != NOERRORS) {
			DisplayMessage(ULStat);
			return 0;
		}
		ULStat = cbToEngUnits(BoardNum, Range, MidScale, &engUnits);
		printf("Waiting for trigger on analog input above %fV and acquiring data.", engUnits);
	}

    printf("The trigger input line (D0) must be held low before this demo is started.\n\n");
    while (!_kbhit())
        {;}
	ch = _getch();

    printf("\n\nRelease D0 when ready:\n\n");

    FileName = "DEMO.DAT";
    LowChan = 0;
    HighChan = 1;
    Options = 0;
    Range = BIP5VOLTS;

    printf("Collecting %4ld data points...", TotalCount);

    /* Collect the values with cbFileAInScan()
       Parameters:
            BoardNum     :the number used by CB.CFG to describe this board
            LowChan      :first A/D channel of the scan
            HighChan     :last A/D channel of the scan
            PreTrigCount :the total number of A/D samples to collect
            TotalCount   :the total number of samples to store to file
            Rate         :sample rate in samples per second
            Range         :the Range for the board
            FileName   :the filename for the collected data values
            Options    :data collection options */

    ULStat = cbFilePretrig(BoardNum, LowChan, HighChan, 
		&PreTrigCount, &TotalCount, &Rate, Range, FileName, Options);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

    printf("which were placed in the file: %s.\n", FileName);

    /* show the information in the file header with cbFileGetInfo()
       Parameters:
          FileName      :the filename containing the data
          LowChan       :first A/D channel of the scan
          HighChan      :last A/D channel of the scan
          PreTrigCount  :the number of pretrigger samples in the file
          TotalCount    :the total number of A/D samples in the file
          Rate          :sample rate in samples per second
          Range          :the Range at which the samples were collected */

    ULStat = cbFileGetInfo(FileName, &LowChan, &HighChan, &PreTrigCount, &TotalCount, &Rate, &Range);

    printf("\nThe information in the file header is:\n");
    printf("     Streamer File Name        =  %s\n", FileName);
    printf("     Low Channel               =     %1u\n", LowChan);
    printf("     High Channel              =     %1u\n", HighChan);
    printf("     No. of Pretrigger samples =  %4ld\n", PreTrigCount);
    printf("     No. of Samples            =  %4ld\n", TotalCount);
    printf("     Collection Rate(Hz)       =  %4ld\n", Rate);
    printf("     Range                     =   %3u\n", Range);


    /* show the data using cbFileRead()
       Parameters:
          FileName      :the filename containing the data
          NumPoints     :the number of data values to read from the file
          FirstPoint    :index of the first data value to read
          DataBuffer[]   :array to read data into  */

    NumPoints = 10;     /* read 10 points in the file */
    FirstPoint = PreTrigCount-4;     /* start at the trigger - 4 */
    ULStat = cbFileRead(FileName, FirstPoint, &NumPoints, DataBuffer);

    /* display the data values read from the file */
    printf("\nThe five points before and after the trigger are:\n");
    for (I = 0; I < (int)NumPoints; I++)
        printf("     %2u) %4u  \n", I, DataBuffer[I]);

	DisplayMessage(NOERRORS);
	ULStat = cbWinBufFree(MemHandle);
	return 0;
}

