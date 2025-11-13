/*
    ULFI01_fixed.c  — Minimal, self‑contained replacement for corrupted ULFI01.C
    Purpose: Scan one analog input channel to a streamer file, then print file header info.
    Notes:
      * Uses only cbw.h API calls (no Utilities.h dependency).
      * Targets MCC Universal Library for Windows (32/64-bit).
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../cbw.h"

int main(void)
{
    /* --- configuration --- */
    int   BoardNum   = 0;
    short LowChan    = 0;
    short HighChan   = 0;              /* single channel demo */
    long  Count      = 1000;           /* total samples */
    long  Rate       = 1000;           /* Hz */
    int   Range      = BIP5VOLTS;      /* adjust as needed */
    int   Options    = FOREGROUND | CONVERTDATA;  /* convert to 12/16-bit codes in file */
    char  FileName[260] = "DEMO.DAT";

    /* --- bookkeeping --- */
    int   ULStat     = NOERRORS;
    long  PreTrigCount = 0;
    long  TotalCount   = 0;
    long  FirstPoint   = 0;
    long  NumPoints    = 0;

    /* --- basic error handling behavior: print and stop on error --- */
    cbErrHandling (PRINTALL, DONTSTOP);

    printf("ULFI01_fixed: scanning Board %d, Chan %d..%d, Count=%ld, Rate=%ld Hz, Range=%d -> %s\n",
           BoardNum, LowChan, HighChan, Count, Rate, Range, FileName);

    /* --- perform the file scan --- */
    ULStat = cbFileAInScan(BoardNum, LowChan, HighChan, Count, &Rate, Range, FileName, Options);
    if (ULStat != NOERRORS) {
        printf("cbFileAInScan error: %d\n", ULStat);
        return ULStat;
    }

    printf("Scan complete. %ld data points should be in: %s\n", Count, FileName);

    /* --- read and echo the file header information --- */
    ULStat = cbFileGetInfo(FileName, &LowChan, &HighChan, &PreTrigCount, &TotalCount, &Rate, &Range);
    if (ULStat != NOERRORS) {
        printf("cbFileGetInfo error: %d\n", ULStat);
        return ULStat;
    }

    printf("\nFile header info:\n");
    printf("  Rate      = %ld Hz\n", Rate);
    printf("  LowChan   = %d\n",  LowChan);
    printf("  HighChan  = %d\n",  HighChan);
    printf("  Options   = %d\n",  Options);
    printf("  Range     = %d\n",  Range);
    printf("  FileName  = %s\n",  FileName);
    printf("  TotalCount= %ld\n", TotalCount);
    printf("  PreTrig   = %ld\n", PreTrigCount);

    return 0;
}
