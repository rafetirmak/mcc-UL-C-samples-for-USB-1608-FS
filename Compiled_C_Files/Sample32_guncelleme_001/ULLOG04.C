/*ULLOG03.C****************************************************************

File:                         ULLOG03.C

Library Call Demonstrated:    cbLogConvertFile()

Purpose:                      Reads a binary log file and converts
                              the data to CSV format.

Demonstration:                Read binary data, convert it, and 
                              store the data in a CSV format file.

Other Library Calls:          cbLogReadFile()
                              cbErrHandling()
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
	int			ULStat;
	int			FileNumber;
	int			Version;
	int			Size;
	int			SampleInterval;
	int			SampleCount;
	int			StartDate;
	int			StartTime;
	int			StartSample = 0;
	char		SourceDirectory[MAX_PATH];
	char*		pPublicDir;
	char*		ptrToExtension;
	char		DestinationFilename[MAX_PATH];
	char		AbsolutePath[MAX_PATH];
	char		Filename[MAX_PATH];
	int			Delimiter;
    float		RevLevel = (float)CURRENTREVNUM;
	char		ch;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	// Set up error handling

    /* set up the display screen */
    ClearScreen();
    printf("Demonstration of cbLogConvertFile()\n\n");

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
	cbLogSetPreferences(TIMEFORMAT_12HOUR, TIMEZONE_LOCAL, FAHRENHEIT);


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

	/* Get the destination directory */
	pPublicDir = getenv("PUBLIC");
	strcpy(DestinationFilename, Filename);
	ptrToExtension = strrchr(DestinationFilename, '.');
	strcpy(ptrToExtension, ".CSV");

	pPublicDir = _fullpath(AbsolutePath, Filename, MAX_PATH);

	/* Get the file information for first binary log file in the directory
		Parameters:
			FileName        :the filename to retrieve information from
			Version         :receives the version information
			Size			:receives the file size information
	*/
	printf("Calling cbLogGetFileInfo() to get file info for file: \n  %s\n\n", AbsolutePath);
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
	printf("  Calling cbLogGetSampleInfo() to get sample info from the file: \n");
	ULStat = cbLogGetSampleInfo(Filename, &SampleInterval, &SampleCount, &StartDate, &StartTime);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	StartSample = 0;
	Delimiter = DELIMITER_COMMA;


	/* convert the log file to a .CSV file for Excel */
	printf("  Calling cbLogConvertFile() to convert data from the file: \n");
	ULStat = cbLogConvertFile(Filename, DestinationFilename, StartSample, SampleCount, Delimiter);
	if (ULStat != NOERRORS) {
		DisplayMessage(ULStat);
		return 0;
	}

	printf("\n\nThe file has been converted and saved in the file \n   %s\n", DestinationFilename);
	DisplayMessage(NOERRORS);
	return 0;
}


