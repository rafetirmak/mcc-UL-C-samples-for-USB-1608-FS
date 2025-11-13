/*ULLOG03.C****************************************************************

File:                         ULLOG03.C

Library Call Demonstrated:    cbLogReadFile()

Purpose:                      Reads a binary log file.

Demonstration:                Display the analog, CJC, and DIO data.

Other Library Calls:          cbErrHandling()
                              cbGetFilename()
                              cbGetFileInfo()

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
	int			ULStat = NOERRORS;
	int			TimeFormat;
	int			TimeZone;
	int			Units;
	int			FileNumber;
	int			Version;
	int			Size;
	int			SampleInterval;
	int			SampleCount;
	int			StartDate;
	int			StartTime;
	int			AICount;
	int			CJCCount;
	int			DIOCount;
	int			StartSample = 0;
	int*		Dates = NULL;
	int*		Times = NULL;
	float*		Analog = NULL;
	int*		Dio = NULL;
	float*		Cjc = NULL;
	char*		postfix;
	char		SourceDirectory[MAX_PATH];
	char*		pPublicDir;
	char		AbsolutePath[MAX_PATH];
	char		Filename[MAX_PATH];
	char		ch;
    float		RevLevel = (float)CURRENTREVNUM;
	int			i, j;
	int			index;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	// Set up error handling

	/* set up the display screen */
    ClearScreen();
    printf("Demonstration of cbLogReadFile()\n\n");

	/* Get the path to the binary log files */
	pPublicDir = getenv("PUBLIC");
	strcpy(SourceDirectory, pPublicDir);
	strcat(SourceDirectory, "\\Documents\\Measurement Computing\\DAQ\\");

	printf("  Before starting, copy the file 'LOG00000.BIN' \n");
	printf("  from the Universal Library installation directory to\n");
	printf("  %s \n", SourceDirectory);
	DisplayMessage(NOERRORS);
	ch = _getch();
	ClearScreen();

	/* Set the preferences 
		Parameters
			TimeFormat	:specifies times are 12 or 24 hour format
			TimeZone	:specifies local time of GMT
			Units		:specifies Fahrenheit, Celsius, or Kelvin
	*/
	TimeFormat = TIMEFORMAT_12HOUR;
	TimeZone = TIMEZONE_LOCAL;
	Units = FAHRENHEIT;
	cbLogSetPreferences(TimeFormat, TimeZone, Units);


    /* Get the name of the first binary log file in the source directory()
        Parameters:
            FileNumber  :the nth file in the directory; or GETFIRST or GETNEXT
            Path[]      :full path to directory containing binary log files
            Filename[]  :receives the full path to the binary log file
	*/
	FileNumber = GETFIRST;
	ULStat = cbLogGetFileName(FileNumber, SourceDirectory, Filename);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	_fullpath(AbsolutePath, Filename, MAX_PATH);

	/* Get the file information for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			Version         :receives the version information
			Size			:receives the file size information
	*/
	printf("Calling cbLogGetFileInfo() to get file info from the file. \n   %s\n\n", AbsolutePath);
	ULStat = cbLogGetFileInfo(Filename, &Version, &Size);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	/* Get the sample information for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			SampleInterval  :receives the time between samples
			SampleCount		:receives the number of samples in the file
			StartDate		:receives the start date
			StartTime       :recveives the start time
	*/
	//printf("For file: \n   %s\n\n", AbsolutePath);
	printf("  Calling cbLogGetSampleInfo() to get sample info from the file. \n");
	ULStat = cbLogGetSampleInfo(Filename, &SampleInterval, &SampleCount, &StartDate, &StartTime);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	/* Get the AI channel count for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			AICount			:receives the number of AI Channels
	*/
	printf("  Calling cbLogGetAIChannelCount() to get the number of channels logged: \n");
	ULStat = cbLogGetAIChannelCount(Filename, &AICount);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	/* Get the CJC information for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			CJCCount		:receives the number of CJC Channels
	*/
	printf("  Calling cbLogGetCJCInfo() to get CJC info from the file: \n");
	ULStat = cbLogGetCJCInfo(Filename, &CJCCount);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	/* Get the DIO information for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			DIOCount		:receives the number of DIO Channels
	*/
	printf("  Calling cbLogGetDIOInfo() to get DIO info from the file: \n");
	ULStat = cbLogGetDIOInfo(Filename, &DIOCount);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}


	/* Allocate the Date and Time arrays and read the data */
	/* Get the date/time information for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			StartSample		:first sample to read
			SampleCount		:number of samples to read
			Dates			:receives the sample dates
			Times			:receives the sample times
	*/
	printf("  Calling cbLogReadTimeTags() to get TIMESTAMP information from the file: \n");
	Dates = malloc(SampleCount * sizeof(int));
	Times = malloc(SampleCount * sizeof(int));
	ULStat = cbLogReadTimeTags(Filename, StartSample, SampleCount, Dates, Times);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}
	printf("\n\n  File version:     %10d \t File size:    %10d \n", Version, Size);
	printf("  Sample Interval:  %10d \t Sample Count: %10d \n  "
		"Start Date:       %10d \t Start Time:   %10d \n", 
		SampleInterval, SampleCount, StartDate, StartTime);
	printf("  Analog Channels:  %10d \t CJC Chans:    %10d \n", AICount, CJCCount);
	printf("  Digital Channels: %10d \n", DIOCount);
	DisplayMessage(NOERRORS);
	ch = _getch();
	ClearScreen();


	/* Allocate the Analog array and read the data*/
	/* Get the Analog information for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			StartSample		:first sample to read
			SampleCount		:number of samples to read
			Analog			:receives the analog data
	*/
	printf("Reading the data from the file: \n");
	printf("   %s\n\n", AbsolutePath);
	if (AICount)  {
		printf("  Calling cbLogReadAIChannels() to read the ANALOG data from the file: \n");
		Analog = malloc(SampleCount * AICount * sizeof(float));
		ULStat = cbLogReadAIChannels(Filename, StartSample, SampleCount, Analog);
	}
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}


	/* Allocate the Dio array and read the data */
	/* Get the DIO information for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			StartSample		:first sample to read
			SampleCount		:number of samples to read
			Dio				:receives the dio data
	*/
	if (DIOCount) {
		printf("  Calling cbLogReadAIChannels() to read the DIO data from the file: \n");
		Dio = malloc(SampleCount * DIOCount * sizeof(int));
		ULStat = cbLogReadDIOChannels(Filename, StartSample, SampleCount, Dio);
	}
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}


	/* Allocate the Cjc array and read the data */
	/* Get the CJC information for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			StartSample		:first sample to read
			SampleCount		:number of samples to read
			Cjc				:receives the cjc data
	*/
	if ( CJCCount && (ULStat == NOERRORS) )
	{
		printf("  Calling cbLogReadAIChannels() to read the CJC data from the file: \n");
		Cjc = malloc(SampleCount * CJCCount * sizeof(float));
		ULStat = cbLogReadCJCChannels(Filename, StartSample, SampleCount, Cjc);
	}
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}
	printf("\n\nData collected from the file. Ready to display data.\n");
	DisplayMessage(NOERRORS);
	ch = _getch();
	ClearScreen();

	// display the contents of the log file
	for (i = 0; i < SampleCount; i++) {
		index = i;
		switch (StartTime >> 24)
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
		printf("%d/%d/%d  ", (Dates[index] >> 8) & 0xff,		// month 
			Dates[index] & 0xff,				// day
			(Dates[index] >> 16) & 0xffff);		// year

		printf("%d:%d:%d %s  ", (Times[index] >> 16) & 0xff,	// hours
			(Times[index] >> 8) & 0xff,			// minutes
			Times[index] & 0xff,				// seconds
			postfix);
		printf("\n");

		if (Analog) {
			index = i * AICount;
			for (j=0; j<AICount; j++) {
				printf("%3.3f  ", Analog[index++]);
			}
		}

		if (Cjc) {
			index = i * CJCCount;
			for (j=0; j<CJCCount; j++) {
				printf("%3.1f  ", Cjc[index++]);
			}
		}

		if (Dio) {
			index = i * DIOCount;
			for (j=0; j<DIOCount; j++) {
				printf("%3d  ", Dio[index++]);
			}
		}
		printf("\n");
		if (((i + 1) % 10) == 0) {
			DisplayMessage(NOERRORS);
			ch = _getch();
			ClearScreen();
		}
	}
	printf("\n");
	free(Dates);
	free(Times);

	if (Analog)
		free(Analog);
	if (Dio)
		free(Dio);
	if (Cjc)
		free(Cjc);

	printf("\nDone reading file. To end program,");
	DisplayMessage(NOERRORS);
	return 0;
}

