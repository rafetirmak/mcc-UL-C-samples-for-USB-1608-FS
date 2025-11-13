/*ULLOG02.C****************************************************************

File:                         ULLOG02.C

Library Call Demonstrated:    cbLogGetFileInfo()

Purpose:                      Retrieves the header information from
                              a binary log file.

Demonstration:                Displays all of the header information 
                              for a binary log file.

Other Library Calls:          cbErrHandling()
                              cbGetFilename()

Special Requirements:         None.

Copyright (c) 1995-2020, Measurement Computing Corp.
All Rights Reserved.
***************************************************************************/


/* Include files */
#include "..\cbw.h"
#include "Utilities.h"

 int main(void)
{
	/* Variable Declarations */
	int			ULStat;
	int			FileNumber;
	int			Version;
	int			Size;
	int			SampleInterval;
	int			SampleCount;
	int			StartDate;
	int			StartTime;
	int*		ChannelNumbers = NULL;
	int*		Units = NULL;
	int			AICount;
	int			CJCCount;
	int			DIOCount;
	char		SourceDirectory[MAX_PATH];
	char*		ch;
	char		AbsolutePath[MAX_PATH];
	char		Filename[MAX_PATH];
	char*		postfix;
    float		RevLevel = (float)CURRENTREVNUM;
	char		res;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	// Set up error handling

	/* set up the display screen */
    ClearScreen();
    printf("Demonstration of how to get information from binary log files\n\n");

	/* Get the path to ther binary log files */
	ch = getenv("PUBLIC");
	strcpy(SourceDirectory, ch);
	strcat(SourceDirectory, "\\Documents\\Measurement Computing\\DAQ\\");

	printf("  Before starting, copy the file 'LOG00000.BIN' \n");
	printf("  from the Universal Library installation directory to\n");
	printf("  %s \n", SourceDirectory);
	DisplayMessage(NOERRORS);
	res = _getch();
	ClearScreen();

    /* Get the name of the first binary log file in the source directory()
        Parameters:
            FileNumber          :the nth file in the directory; or GETFIRST or GETNEXT
            SourceDirectory[]   :full path to directory containing binary log files
            Filename[]          :receives the full path to the binary log file
	*/

	FileNumber = GETFIRST;
	ULStat = cbLogGetFileName(FileNumber, SourceDirectory, Filename);
	if (ULStat != NOERRORS) {
		if ((ULStat == DOSFILENOTFOUND) | (ULStat == DOSPATHNOTFOUND)
			| (ULStat == DOSBADDRIVE)) {
			printf(" Could find files in directory '%s'", SourceDirectory);
			DisplayMessage(ULStat);
			return 0;
		} else {
			DisplayMessage(ULStat);
			return 0;
		}
	} else {
		ch = _fullpath(AbsolutePath, Filename, MAX_PATH);

		/* Get the file information for first binary log file in the directory
			Parameters:
				FileName        :the filename to retrieve information from
				Version         :receives the version information
				Size			:receives the file size information
		*/
		printf("Calling cbLogGetFileInfo() to get file info for file: \n\n");

		ULStat = cbLogGetFileInfo(Filename, &Version, &Size);
		if (ULStat == NOERRORS) {
			/* Display the file header information */
			printf("Filename:  %s\n", AbsolutePath);
			printf("  File version:                    %d\n", Version);
			printf("  Size of binary file:             %d\n", Size);
		}
	}

	if (ULStat == NOERRORS) {
		/* Get the sample information for first binary log file in the directory
			Parameters:
				FileName        :the filename to retrieve information from
				SampleInterval  :receives the time between samples
				SampleCount		:receives the number of samples in the file
				StartDate		:receives the start date
				StartTime       :recveives the start time
		*/
		printf("\n\n");
		printf("Calling cbLogGetSampleInfo() to get sample info:\n\n");

		ULStat = cbLogGetSampleInfo(Filename, &SampleInterval, &SampleCount, &StartDate, &StartTime);
		if (ULStat != NOERRORS) {
			DisplayMessage(NOERRORS);
			return 0;
		} else {
			/* Display the sample header information */
			switch(StartTime >> 24)
			{
			case 0:
				postfix = "AM";
				break;
			case 1:
				postfix = "PM";
				break;
			case 0xff:
			default:
				postfix = "";
			}

			printf("  Sample interval:                 %d\n", SampleInterval);
			printf("  Number of samples (rows):        %d\n", SampleCount);
			printf("  StartDate:                       %d/%d/%d\n", (StartDate >> 8) & 0xff,		// month 
				                                                    StartDate & 0xff, 				// day
																	(StartDate >> 16) & 0xffff);	// year

			printf("  StartTime:                       %d:%d:%d %s\n", (StartTime >> 16) & 0xff,	// hours
				                                                    (StartTime >> 8) & 0xff, 		// minutes
																	StartTime & 0xff,				// seconds
																	postfix);						
		}
	}
	DisplayMessage(NOERRORS);
	res = _getch();
	ClearScreen();

	/* Get the AI channel count for first binary log file in the directory
			Parameters:
				FileName        :the filename to retrieve information from
				AICount			:receives the number of AI Channels
	*/
	printf("For file %s\n\n", AbsolutePath);
	printf("Calling cbLogGetAIChannelCount() to get the number of channels logged: \n\n");

	ULStat = cbLogGetAIChannelCount(Filename, &AICount);
	if (ULStat != NOERRORS) {
		DisplayMessage(NOERRORS);
		return 0;
	}
	printf("  Number of analog channels:       %d\n\n", AICount);


	/* Get the AI information for first binary log file in the directory
			Parameters:
				FileName        :the filename to retrieve information from
				ChannelMask		:receives the channel mask
				UnitMask		:receives the unit mask
	*/

	printf("\nCalling cbLogGetAIInfo() to get AI info for file: \n\n");

	ChannelNumbers = (int*)malloc(AICount * sizeof(int));
	Units = (int*)malloc(AICount * sizeof(int));


	ULStat = cbLogGetAIInfo(Filename, ChannelNumbers, Units);
	if (ULStat != NOERRORS) {
		DisplayMessage(NOERRORS);
		return 0;
	} else {
		int i;
		char*	UnitStr;

		/* Display the AI header information */
		for (i=0; i< AICount; i++) {
			if (Units[i] == UNITS_RAW)
				UnitStr = "Raw";
			else
				UnitStr = "Temperature";
			printf("  Channel %d units:                 %s\n", ChannelNumbers[i], UnitStr);
		}
	}
	

	/* Get the CJC information for first binary log file in the directory
			Parameters:
				FileName        :the filename to retrieve information from
				CJCCount		:receives the number of CJC Channels
	*/
	printf("\n\n");
	printf("Calling cbLogGetCJCInfo() to get CJC info for file: \n\n");

	ULStat = cbLogGetCJCInfo(Filename, &CJCCount);
	if (ULStat == NOERRORS) {
		/* Display the AI header information */
		printf("  Number of CJC channels:          %d\n", CJCCount);
	}

	/* Get the DIO information for first binary log file in the directory
			Parameters:
				FileName        :the filename to retrieve information from
				DIOCount		:receives the number of DIO Channels
	*/
	printf("\n\n");
	printf("Calling cbLogGetDIOInfo() to get DIO info for file: \n\n");

	ULStat = cbLogGetDIOInfo(Filename, &DIOCount);
	if (ULStat != NOERRORS) {
		DisplayMessage(NOERRORS);
		return 0;
	} else {
		/* Display the AI header information */
		printf("  Number of DIO channels:          %d\n", DIOCount);
	}
	printf("\n\nFile read complete. To end program,");
	DisplayMessage(NOERRORS);
	return 0;
}

