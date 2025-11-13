/*ULLOG01.C****************************************************************

File:                         ULLOG01.C

Library Call Demonstrated:    cbLogGetFilename()

Purpose:                      Retrieves the name of a binary log file.

Demonstration:                Displays all of the binary files in a 
                              specified directory.

Other Library Calls:          cbErrHandling()

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
	int		ULStat;
	int		FileNumber;
	int		Count;
	char	SourceDirectory[MAX_PATH];
	char*	ch;
	char	AbsolutePath[MAX_PATH];
	char	Filename[MAX_PATH];
    float   RevLevel = (float)CURRENTREVNUM;
	char	res;

	/* Declare UL Revision Level */
	ULStat = cbDeclareRevision(&RevLevel);

	InitUL();	// Set up error handling

    /* set up the display screen */
    ClearScreen();
    printf("Demonstration of cbLogGetFileName()\n\n");

	/* Get the path to the binary log files */
	ch = getenv("PUBLIC");
	strcpy(SourceDirectory, ch);
	strcat(SourceDirectory, "\\Documents\\Measurement Computing\\DAQ\\");

	printf("  Before starting, copy the file 'LOG00000.BIN' \n");
	printf("  from the Universal Library installation directory to\n");
	printf("  %s \n", SourceDirectory);
	DisplayMessage(NOERRORS);
	res = _getch();
	ClearScreen();

    /* Display the names of all of the files in the source directory()
        Parameters:
            FileNumber  :the nth file in the directory; or GETFIRST or GETNEXT
            Path[]      :full path to directory containing binary log files
            Filename[]  :receives the full path to the binary log file
	*/

	FileNumber = GETFIRST;
	Count = 0;
	while (ULStat == NOERRORS) {
		ULStat = cbLogGetFileName(FileNumber, SourceDirectory, Filename);
		if (ULStat != NOMOREFILES) {
			if (ULStat != NOERRORS) {
				if ((ULStat == DOSFILENOTFOUND) | (ULStat == DOSPATHNOTFOUND)
					| (ULStat == DOSBADDRIVE)) {
						printf(" Could find files in directory\n  '%s'", SourceDirectory);
						DisplayMessage(ULStat);
						return 0;
				} else {
					DisplayMessage(ULStat);
					return 0;
				}
			} else {
				Count++;
				ch = _fullpath(AbsolutePath, Filename, MAX_PATH);
				printf("%s\n", AbsolutePath);
				FileNumber = GETNEXT;
			}
		}
	}
	printf("\nA total of %d files were found\n", Count);
	DisplayMessage(NOERRORS);
	return 0;
}

