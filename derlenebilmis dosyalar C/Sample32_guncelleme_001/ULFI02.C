/*ULFI02.C=================================================================

File:                         ULFI02.C

Library Call Demonstrated:    File Operations:
                              cbFileRead()
                              cbFileGetInfo()

Purpose:                      Read information & data from a streamer file.

Demonstration:                Creates a file and scans analog data to the
                              file. Displays the data in the file and the
                              information in the file header.

Other Library Calls:          cbFileAInScan()
                              cbErrHandling()

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
    long	TestPoints = 1000;
    long	FirstPoint, NumPoints;
    short	HighChan, LowChan;
	int		defaultChan = 0;
	int		NumAIChans;
	int		Options = FOREGROUND;
	int		Range = BIP5VOLTS;
    long	PreTrigCount, Count, TotalCount, Rate;
    char*	FileName;
	HGLOBAL	MemHandle;
    WORD*	DataBuffer = NULL;
	BOOL	HighResAD = FALSE;
	int		DefaultTrig = TRIG_POS_EDGE;
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
	printf(" Demonstration of File Operations \n");
	printf(" cbFileRead() and cbFileGetInfo() using %s\n\n", BoardName);

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
	MemHandle = cbWinBufAlloc(TestPoints);
	DataBuffer = (WORD*)MemHandle;

	if (!MemHandle) {	   /* Make sure it is a valid pointer */
		printf("\n Out of memory\n");
		return 0;
	}

    Count = TestPoints;
    FileName = "DEMO.DAT";
    Rate = 1000;                      /* sampling rate (samples per second) */
    Options = CONVERTDATA;

    printf("Collecting %4ld data points...", Count);

    /* Collect the values with cbFileAInScan()
    Parameters:
          BoardNum   :the number used by CB.CFG to describe this board
          LowChan    :first A/D channel of the scan
          HighChan   :last A/D channel of the scan
          Count      :the total number of A/D samples to collect
          Rate       :sample rate in samples per second
          Range       :the Range for the board
          FileName   :the filename for the collected data values
          Options    :data collection options */

    ULStat = cbFileAInScan(BoardNum, LowChan, HighChan, Count, &Rate, Range, FileName, Options);
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
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

    printf("\nThe information in the file header is:\n");
    printf("     Streamer File Name        =  %s\n", FileName);
    printf("     Low Channel               =     %1u\n", LowChan);
    printf("     High Channel              =     %1u\n", HighChan);
    printf("     No. of Pretrigger samples =  %4ld\n", PreTrigCount);
    printf("     No. of Samples            =  %4ld\n", TotalCount);
    printf("     Collection Rate(Hz)       =  %4ld\n", Rate);
    printf("     Range                     =   %3u\n", Range);

  /* Show the data using cbFileRead()
  Parameters:
      FileName      :the filename containing the data
      NumPoints     :the number of data values to read from the file
      FirstPoint    :index of the first data value to read
      DataBuffer[]   :array to read data into  */

     NumPoints = 5;     /* read the first five points of the file */
     FirstPoint = 0;     /* start at the first point after the trigger*/
     ULStat = cbFileRead(FileName, FirstPoint, &NumPoints, DataBuffer);
	 if (ULStat != NOERRORS) {
		 DisplayMessage(ULStat);
		 return 0;
	 }

    /* display the data values read from the file */
    printf("\nThe first 5 data values in the file are:\n");
    for (I = (int)FirstPoint; I < (int)NumPoints; I++)
			printf("     %2u) %4u  \n", I, DataBuffer[I]);
	
	DisplayMessage(NOERRORS);
	ULStat = cbWinBufFree(MemHandle);
	return 0;
}

