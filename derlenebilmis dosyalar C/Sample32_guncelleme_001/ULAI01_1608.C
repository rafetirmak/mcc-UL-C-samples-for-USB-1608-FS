// ULAI01_min.c — ULAI01.C ile aynı davranış (tek örnek, tek kanal, cbAIn), sade demo
// - Tek kanaldan ham okuma + Volt'a dönüşüm (cbToEngUnits)
// - Foreground/polling (arka plan yok), ULAI01 ruhuna sadık
// - UTF-8 konsol (± işareti), ekran kapanmasın diye Enter bekletme

#include <windows.h>
#include <stdio.h>
#include <locale.h>
#include <conio.h>      // _kbhit
#include "cbw.h"        // MCC Universal Library

// --- Kullanıcı ayarları (ULAI01 tarzı tek kanal) ---
#define BOARDNUM    0           // InstaCal'de USB-1608FS kart numaranız
#define CHANNEL     0           // Tek kanal
#define RANGE1608   BIP10VOLTS  // ±10 V (1608FS destekli)
#define READ_DELAY_MS 50        // Okumalar arası bekleme (gözlem için)
#define MAX_SAMPLES  200        // İstersen sınırsız için büyük bir sayı ver

int main(void)
{
    // Konsolu UTF-8 yap (± vb. semboller için)
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
    setlocale(LC_ALL, ".UTF-8");

    // Hata işlemesi: hatayı yazdır ve durdur
    cbErrHandling(PRINTALL, STOPALL);

    // Başlık / demo bilgisi
    printf("=== MCC Universal Library DEMO ===\n");
    printf("Komut  : cbAIn (tek örnek, foreground/polling)\n");
    printf("Cihaz  : USB-1608FS (Board #%d)\n", BOARDNUM);
    printf("Kanal  : %d\n", CHANNEL);
    printf("Aralik : ±10 V (RANGE = BIP10VOLTS)\n");
    printf("----------------------------------\n");
    printf("Not: Her %.0f ms'de bir tek örnek okunacak. "
           "Bir tusa basarsan okuma sonlanir.\n\n", (double)READ_DELAY_MS);

    int    ulstat;
    USHORT raw;         // cbAIn ham 16-bit değer (0..65535)
    float volt;        // mühendislik birimi (Volt)

    int sample = 0;
    
    while (sample < MAX_SAMPLES && !_kbhit()) {
        // Tek örnek oku (ULAI01.C'nin yaptığı şey)
        ulstat = cbAIn(BOARDNUM, CHANNEL, RANGE1608, &raw);
        if (ulstat != NOERRORS) {
            printf("\ncbAIn hata: %d\n", ulstat);
            break;
        }

        // Volt'a dönüşüm (ULAI01 tarzı gösterim)
        ulstat = cbToEngUnits(BOARDNUM, RANGE1608, raw, &volt);
        if (ulstat != NOERRORS) {
            printf("\ncbToEngUnits hata: %d\n", ulstat);
            break;
        }

        printf("Örnek %4d | raw=%5u | Volt=%.6f V\r", sample+1, raw, volt);
        fflush(stdout);

        Sleep(READ_DELAY_MS);
        sample++;
    } // WHILE SONU

    printf("\n\nOkuma bitti (toplam %d örnek).\n", sample);

    // Konsol kapanmasın
    printf("Çıkmak için Enter'a basın...");
    // Eğer döngü tuşla bitti ise buffer'daki tuşu tüket:
    if (_kbhit()) (void)_getch();
    getchar();
    return 0;
}
