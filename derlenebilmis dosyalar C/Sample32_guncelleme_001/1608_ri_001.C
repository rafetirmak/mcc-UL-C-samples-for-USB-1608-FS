// ULAI01_1608_demo.c
// UL (cbw) ile USB-1608FS: cbAInScan BACKGROUND demo + canlı durum + kapanmayan konsol
// Derleme (örnek):
//   x86_64-w64-mingw32-gcc -Wall ULAI01_1608_demo.c -IC:\MCC\UL\Include -LC:\MCC\UL\Lib -lcbw64 -o ULAI01_1608_demo.exe

#include <stdio.h>
#include <locale.h>     // <-- Bunu eklemezsen LC_ALL ve setlocale tanınmaz
#include <windows.h>      // HGLOBAL, GlobalLock/Unlock, Sleep
#include "cbw.h"          // Measurement Computing UL


// -------- Kullanıcı ayarları (USB-1608FS güvenli örnek) --------
#define BOARDNUM      0             // InstaCal'de 1608FS'e verdiğiniz numara
#define LOWCHAN       0
#define HIGHCHAN      7             // 8 kanal
#define RANGE1608     BIP10VOLTS    // 1608FS destekli aralıklardan biri
#define PER_CH_RATE   5000L         // kanal başına 5 kS/s  -> toplam 40 kS/s
#define SAMPLES_PC    4096L         // kanal başı örnek sayısı
// ---------------------------------------------------------------

static void print_options(int opts) {
    printf("Options: 0x%X [", opts);
    int first = 1;
    #define PFLAG(f,name) if (opts & (f)) { printf("%s%s", first?"":" | ", name); first=0; }
    PFLAG(BACKGROUND,     "BACKGROUND");
    PFLAG(BLOCKIO,        "BLOCKIO");
    PFLAG(SINGLEIO,       "SINGLEIO");
    PFLAG(BURSTIO,        "BURSTIO");
    PFLAG(CONTINUOUS,     "CONTINUOUS");
    PFLAG(EXTTRIGGER,     "EXTTRIGGER");
    PFLAG(EXTCLOCK,       "EXTCLOCK");
    PFLAG(CONVERTDATA,    "CONVERTDATA");
    PFLAG(NOCALIBRATEDATA,"NOCALIBRATEDATA");
    if (first) printf("none");
    printf("]\n");
}

int main(void)
{
    int    BoardNum   = BOARDNUM;
    int    LowChan    = LOWCHAN;
    int    HighChan   = HIGHCHAN;
    int    channels   = (HighChan - LowChan + 1);
    long   Count      = channels * SAMPLES_PC; // Count mutlaka kanal sayısının katı olmalı
    long   Rate       = PER_CH_RATE;           // UL'de RATE = kanal başı hız
    int    Range      = RANGE1608;

    // Bu demo: arkaplanda halkasal buffer ile tarama
    int    Options    = BACKGROUND; // İsterseniz +BLOCKIO / +CONTINUOUS vb. ekleyebilirsiniz

// türkçe karakterler doğru basılsın
    SetConsoleOutputCP(CP_UTF8);   // Konsol çıkışı UTF-8
    SetConsoleCP(CP_UTF8);         // Konsol girişi UTF-8 (isteğe bağlı)
    setlocale(LC_ALL, ".UTF-8");   // C yerel ayarı


    // Hata işleme
    cbErrHandling(PRINTALL, STOPALL);

    // Bilgi başlığı
    printf("=== MCC Universal Library DEMO ===\n");
    printf("Komut   : cbAInScan (Analog In Scan)\n");
    printf("Cihaz   : USB-1608FS (Board #%d)\n", BoardNum);
    printf("Kanallar: %d..%d  (toplam %d kanal)\n", LowChan, HighChan, channels);
    printf("Aralik  : ");
    switch (Range) {
        case BIP10VOLTS:  printf("±10 V\n"); break;
        case BIP5VOLTS:   printf("±5 V\n");  break;
        case BIP2VOLTS:   printf("±2 V\n");  break;
        case BIP1VOLTS:   printf("±1 V\n");  break;
        default:          printf("kod=%d\n", Range); break;
    }
    printf("Hiz     : %ld S/s (kanal basi)  -> toplam ~%ld S/s\n", Rate, Rate * channels);
    printf("Sayim   : %ld ornek (toplam)  -> kanal basi %ld ornek\n", Count, Count / channels);
    print_options(Options);
    printf("----------------------------------\n");

    // Bellek ayır
    HGLOBAL MemHandle = cbWinBufAlloc(Count);
    if (MemHandle == 0) {
        printf("cbWinBufAlloc basarisiz!\n");
        printf("\nBitti... Cikmak icin Enter'a basin."); getchar();
        return 1;
    }

    // İsteğe bağlı: ham veriye erişmek için kilitle
    unsigned short* Data = (unsigned short*)GlobalLock(MemHandle);
    if (!Data) {
        printf("GlobalLock basarisiz!\n");
        cbWinBufFree(MemHandle);
        printf("\nBitti... Cikmak icin Enter'a basin."); getchar();
        return 1;
    }

    // Taramayi baslat
    int ULStat = cbAInScan(BoardNum, LowChan, HighChan, Count, &Rate, Range, MemHandle, Options);
    if (ULStat != NOERRORS) {
        printf("cbAInScan hata: %d\n", ULStat);
        GlobalUnlock(MemHandle);
        cbWinBufFree(MemHandle);
        printf("\nBitti... Cikmak icin Enter'a basin."); getchar();
        return 1;
    }

    // Canli durum — RUNNING gösterimi
    short Status = 0;
    long  CurCount = 0, CurIndex = 0;
    const char spinch[4] = {'|','/','-','\\'};
    int sp = 0;

    printf("USB-1608FS streaming basladi. Durdurmak icin bekleyin...\n");
    do {
        ULStat = cbGetIOStatus(BoardNum, &Status, &CurCount, &CurIndex, AIFUNCTION);
        if (ULStat != NOERRORS) {
            printf("\ncbGetIOStatus hata: %d\n", ULStat);
            break;
        }
        // Tek satirda canlı ilerleme + spinner
        printf("\r[%c] RUNNING | Index=%-8ld  Count=%-8ld  (Esc cikis degil; bu demo kendisi tamamlar)",
               spinch[sp&3], CurIndex, CurCount);
        fflush(stdout);
        sp++;
        Sleep(50);
    } while (Status == RUNNING);

    // Tamamlandı
    printf("\nDurum   : %s\n", (Status == IDLE) ? "IDLE (tamamlandi)" : "DURDU");
    printf("Gerceklesen hiz (kanal basi): %ld S/s\n", Rate);

    // Ornek veri: ch0 ilk 8 örnek
    int samplesPerChan = (int)(Count / channels);
    int show = samplesPerChan < 8 ? samplesPerChan : 8;
    printf("ch0 ilk %d ornek (ham 16-bit):\n", show);
    for (int i = 0; i < show; ++i) {
        unsigned short raw = Data[i * channels + 0];
        printf("  ch0[%d] = %u\n", i, raw);
    }

    // Temizlik
    GlobalUnlock(MemHandle);
    cbWinBufFree(MemHandle);

    // Konsol kapanmasin
    printf("\nBitti... Cikmak icin Enter'a basin.");
    getchar();
    return 0;
}
