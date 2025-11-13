// ULAI01_1608g.c — USB-1608FS canlı grafik (C + Win32 GDI, Python yok)
// - Tek kanal (CH0), cbAIn ile örnek alır
// - Her TIMER_MS ms'de volt okur ve pencerede Polyline ile çizer
// - 1 V aralıklı yatay kılavuz çizgileri ve volt etiketleri içerir
// - Dış bağımlılık: MCC Universal Library (cbw64), Windows GDI/User32

#include <windows.h>
#include <tchar.h>
#include <strsafe.h>
#include <math.h>
#include "cbw.h"

// ----------- Kullanıcı Ayarları -----------
#define BOARDNUM     0            // InstaCal'de USB-1608FS kart numarası
#define CHANNEL      0            // Hangi kanal çizilecek? (0..7)
#define RANGE1608    BIP10VOLTS   // ±10V; gerekirse BIP5VOLTS/BIP2VOLTS/BIP1VOLTS seç
#define TIMER_MS     20           // Ekran güncelleme periyodu (ms) ~50 Hz
#define BUF_LEN      1200         // Ekranda tutulan nokta sayısı (genişlik kadar önerilir)
// -----------------------------------------

static float g_buf[BUF_LEN];
static int   g_head   = 0;
static int   g_width  = 900;
static int   g_height = 400;
static HWND  g_hwnd   = NULL;

// Volt ekseni alt/üst sınırlar (RANGE'e göre ayarlanır)
static float g_vmin = -10.f, g_vmax = +10.f;

static void InitRangeLimits(void) {
    switch (RANGE1608) {
    case BIP10VOLTS: g_vmin = -10.f; g_vmax = +10.f; break;
    case BIP5VOLTS:  g_vmin =  -5.f; g_vmax =  +5.f; break;
    case BIP2VOLTS:  g_vmin =  -2.f; g_vmax =  +2.f; break;
    case BIP1VOLTS:  g_vmin =  -1.f; g_vmax =  +1.f; break;
    default:         g_vmin = -10.f; g_vmax = +10.f; break; // güvenli varsayılan
    }
}

// Volt -> Y piksel ölçekleme
static int VoltToY(float v) {
    if (v < g_vmin) v = g_vmin;
    if (v > g_vmax) v = g_vmax;
    float t = (v - g_vmin) / (g_vmax - g_vmin); // 0..1
    return (int)((1.0f - t) * (g_height - 1));
}

static void PushSample(float v) {
    g_buf[g_head] = v;
    g_head = (g_head + 1) % BUF_LEN;
}

// 1 V adımlı kılavuz ve etiketler
static void DrawVoltGrid(HDC mdc) {
    HPEN penThin = CreatePen(PS_DOT, 1, RGB(80,80,80));
    HPEN penZero = CreatePen(PS_SOLID, 2, RGB(120,120,120));
    HGDIOBJ old  = SelectObject(mdc, penThin);

    int vStart = (int)ceilf(g_vmin);
    int vEnd   = (int)floorf(g_vmax);

    for (int vv = vStart; vv <= vEnd; ++vv) {
        int y = VoltToY((float)vv);

        // 0 V çizgisi kalın, diğerleri ince
        if (vv == 0) SelectObject(mdc, penZero);
        else         SelectObject(mdc, penThin);

        MoveToEx(mdc, 0, y, NULL);
        LineTo(mdc, g_width, y);

        // Sol kenara "+n V" etiketi
        TCHAR lab[16];
        StringCchPrintf(lab, 16, TEXT("%+d V"), vv);
        SetBkMode(mdc, TRANSPARENT);
        SetTextColor(mdc, RGB(170,170,170));
        TextOut(mdc, 6, y - 8, lab, (int)_tcslen(lab));
    }

    SelectObject(mdc, old);
    DeleteObject(penThin);
    DeleteObject(penZero);
}

static void DrawScene(HDC hdc) {
    // Double-buffer: titremesiz çizim
    HDC mdc = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, g_width, g_height);
    HGDIOBJ oldbmp = SelectObject(mdc, bmp);

    // Arka plan
    HBRUSH bg = CreateSolidBrush(RGB(20,20,20));
    RECT rc = {0,0,g_width,g_height};
    FillRect(mdc, &rc, bg);
    DeleteObject(bg);

    // 1 V adımlı kılavuz (0 V kalın)
    DrawVoltGrid(mdc);

    // Dalga kalemi
    HPEN wave = CreatePen(PS_SOLID, 2, RGB(0,200,255));
    HGDIOBJ oldpen = SelectObject(mdc, wave);

    // Ekrana sığacak kadar nokta çiz (en yeni sağda)
    int maxPts = (g_width < BUF_LEN) ? g_width : BUF_LEN;
    if (maxPts < 2) maxPts = 2;
    POINT *pts = (POINT*)malloc(sizeof(POINT)*maxPts);
    if (pts) {
        for (int i = 0; i < maxPts; ++i) {
            int idx = (g_head - 1 - i);
            while (idx < 0) idx += BUF_LEN;
            float v = g_buf[idx];
            int x = g_width - 1 - i;
            int y = VoltToY(v);
            pts[maxPts-1 - i].x = x;
            pts[maxPts-1 - i].y = y;
        }
        Polyline(mdc, pts, maxPts);
        free(pts);
    }

    // Başlık satırı (ASCII kullanıldı; kodlama sorunlarını önler)
    SetBkMode(mdc, TRANSPARENT);
    SetTextColor(mdc, RGB(220,220,220));
    TCHAR title[256];
    // Örnek: +/-10 V şeklinde gösterim
    float span = (g_vmax >= 0 ? g_vmax : -g_vmax); // mutlak üst sınır
    StringCchPrintf(title, 256,
        TEXT("USB-1608FS Live (Board #%d, CH%d, +/-%.0f V) - %d ms / ornek"),
        BOARDNUM, CHANNEL, span, TIMER_MS);
    TextOut(mdc, 10, 10, title, (int)_tcslen(title));

    // Ekrana aktar
    BitBlt(hdc, 0, 0, g_width, g_height, mdc, 0, 0, SRCCOPY);

    // Temizlik
    SelectObject(mdc, oldpen);
    DeleteObject(wave);
    SelectObject(mdc, oldbmp);
    DeleteObject(bmp);
    DeleteDC(mdc);
}

static void ReadAndUpdate(void) {
    USHORT raw;
    float  v;
    int ul = cbAIn(BOARDNUM, CHANNEL, RANGE1608, &raw);
    if (ul != NOERRORS) { PushSample(0.0f); InvalidateRect(g_hwnd, NULL, FALSE); return; }
    ul = cbToEngUnits(BOARDNUM, RANGE1608, raw, &v);
    if (ul != NOERRORS) { PushSample(0.0f); InvalidateRect(g_hwnd, NULL, FALSE); return; }
    PushSample(v);
    InvalidateRect(g_hwnd, NULL, FALSE);
}

LRESULT CALLBACK WndProc(HWND h, UINT msg, WPARAM w, LPARAM l) {
    switch (msg) {
    case WM_CREATE:
        SetTimer(h, 1, TIMER_MS, NULL);
        return 0;
    case WM_SIZE:
        g_width  = LOWORD(l);
        g_height = HIWORD(l);
        if (g_width  < 100) g_width  = 100;
        if (g_height < 100) g_height = 100;
        return 0;
    case WM_TIMER:
        if (w == 1) ReadAndUpdate();
        return 0;
    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(h, &ps);
        DrawScene(hdc);
        EndPaint(h, &ps);
        return 0;
    }
    case WM_DESTROY:
        KillTimer(h, 1);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProc(h, msg, w, l);
}

int APIENTRY _tWinMain(HINSTANCE hInst, HINSTANCE, LPTSTR, int nShow) {
    // UL hata yönetimi
    cbErrHandling(PRINTALL, STOPALL);

    // Range'e göre ölçek sınırlarını kur
    InitRangeLimits();

    // Buffer'ı 0V ile doldur
    for (int i = 0; i < BUF_LEN; ++i) g_buf[i] = 0.0f;

    // Pencere sınıfı
    const TCHAR *cls = TEXT("UL1608_LiveGraph");
    WNDCLASS wc = {0};
    wc.style         = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc   = WndProc;
    wc.hInstance     = hInst;
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    wc.lpszClassName = cls;
    RegisterClass(&wc);

    // Pencere (başlık sabit; ayrıntılar çizim üst yazısında gösteriliyor)
    g_hwnd = CreateWindow(cls,
        TEXT("USB-1608FS — Canli Grafik"),
        WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT,
        g_width, g_height, NULL, NULL, hInst, NULL);
    ShowWindow(g_hwnd, nShow);
    UpdateWindow(g_hwnd);

    // Mesaj döngüsü
    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0) > 0) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}
