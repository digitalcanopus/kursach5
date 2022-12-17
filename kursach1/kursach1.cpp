#pragma comment(lib, "Msimg32.lib")
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "kursach1.h"
#include <Windows.h>
#include <commdlg.h>
#include <string>
#include <stdio.h>
#include <vector>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <cmath>

//#include "Image.h"

#ifndef UNICODE
#define UNICODE
#endif 

#define MAX_LOADSTRING 100
#define BUF 32768

#define BCKG_NAME L"C:\\Users\\Lenovo\\OneDrive\\Рабочий стол\\курсач((( 5\\three.bmp" 
#define BCKG_ED_NAME L"C:\\Users\\Lenovo\\OneDrive\\Рабочий стол\\курсач((( 5\\four.bmp"
#define BCKG_SC_NAME L"C:\\Users\\Lenovo\\OneDrive\\Рабочий стол\\курсач((( 5\\five.bmp"

#define BTN_ENC_DEC 1
#define BTN_ENC 2
#define BTN_DEC 3
#define BTN_SIGN_CHECK 4
#define BTN_SIGN 5
#define BTN_CHECK 6
#define BTN_HIDE_EXTRACT 7
#define BTN_HIDE 8
#define BTN_EXTRACT 9

using namespace std;

static char Buf[BUF];
static unsigned char sBuf[BUF];
wchar_t filename[260] = L"";
OPENFILENAME ofn = { 0 };
static unsigned char ByteArr[BUF] = { 0 };
static int p, q, b, n, p1, q1, d, e;

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void SetOpenFileParams(HWND);
LPVOID ReadFromFile(LPWSTR);
vector<int> IntReadFromFile(LPWSTR);
void IntWriteToFile(int[BUF / 4], LPWSTR);
void WriteToFile(unsigned char[BUF], LPWSTR, int);
DWORD WINAPI Encipher();
DWORD WINAPI Decipher();
DWORD WINAPI Sign();
DWORD WINAPI Check();
DWORD WINAPI Hide();
DWORD WINAPI Extract();

BOOL InputValidationED(int, int, int, int&);
BOOL InputValidationS(int, int, int, int&);
BOOL IsPrime(int);
void ClearBuf(char(&)[BUF]);
void ClearByteArr(unsigned char(&)[BUF]);
void IntToString(int*, char(&)[BUF], int);
void ExtendedEuclideanAlgorithm(int, int, int&, int&);
long FastExponentiation(long, int, int);
int GetGCD(int, int);
int MultInverse(int, int);
int GetDigest(char*, int, int);
void ByteToBit(unsigned char, unsigned char(&)[8]);
unsigned char BitsToByte(unsigned char[8]);
void SaveBMP(unsigned char[54], unsigned char*, int);

HBITMAP hBitmap, hBitmapED, hBitmapSC, hBitmapHE;
HWND hWndMain;
HINSTANCE HInstance;
HWND hWndED, hWndSC, hWndHE;
int NCmdShow;
HANDLE hMutexOF = NULL, hMutex = NULL;
HWND hPTEdit, hCTEdit;
HWND hHEdit, hSEdit;
HWND hSTextEdit;
DWORD bytesRead;

void RegisterEncDecWinClass();
void RegisterSignCheckWinClass();
void RegisterHideExtractWinClass();
LRESULT CALLBACK EncDecWinProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK SignCheckWinProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK HideExtractWinProc(HWND, UINT, WPARAM, LPARAM);
void CreateEncDecWin();
void CreateSignCheckWin();
void CreateHideExtractWin();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HInstance = hInstance;
    NCmdShow = nCmdShow;

    hBitmap = (HBITMAP)LoadImage(NULL, BCKG_NAME, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBitmapED = (HBITMAP)LoadImage(NULL, BCKG_ED_NAME, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);
    hBitmapSC = (HBITMAP)LoadImage(NULL, BCKG_SC_NAME, IMAGE_BITMAP, 0, 0, LR_LOADFROMFILE);

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KURSACH1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HWND hBtnED = CreateWindowEx(0, L"button", L"Encipher/Decipher", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        165, 50, 150, 40, hWndMain, reinterpret_cast<HMENU>(BTN_ENC_DEC), hInstance, NULL);
    HWND hBtnSC = CreateWindowEx(0, L"button", L"Sign/Check sign", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        165, 120, 150, 40, hWndMain, reinterpret_cast<HMENU>(BTN_SIGN_CHECK), hInstance, NULL);
    HWND hBtnHE = CreateWindowEx(0, L"button", L"Steganography", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        165, 190, 150, 40, hWndMain, reinterpret_cast<HMENU>(BTN_HIDE_EXTRACT), hInstance, NULL);
    HWND hwndStatic = CreateWindow(L"static", L"Created by Bozhena Marchik, 2022", WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 315, 230, 20, hWndMain, NULL, hInstance, NULL);

    RegisterEncDecWinClass();
    RegisterSignCheckWinClass();
    RegisterHideExtractWinClass();

    ShowWindow(hWndED, SW_HIDE);
    ShowWindow(hWndSC, SW_HIDE);
    ShowWindow(hWndHE, SW_HIDE);

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_KURSACH1));

    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}


ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_KURSACH1));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    //wcex.hbrBackground  = (HBRUSH)(COLOR_GRAYTEXT + 13); 
    wcex.hbrBackground = CreatePatternBrush(hBitmap);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_KURSACH1);
    wcex.lpszClassName  = L"MainWindowClass";
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   hWndMain = CreateWindowEx(0, L"MainWindowClass", L"Super kursach", WS_OVERLAPPEDWINDOW | WS_BORDER | WS_POPUP | WS_VISIBLE | WS_CHILD,
       0, 0, 500, 400, NULL, NULL, hInstance, NULL);

   if (!hWndMain)
   {
      return FALSE;
   }

   ShowWindow(hWndMain, nCmdShow);
   UpdateWindow(hWndMain);

   return TRUE;
}

void CreateEncDecWin() {

    hWndED = CreateWindowEx(0, L"EncDecWindowClass", L"Encipher/Decipher", WS_OVERLAPPEDWINDOW | WS_BORDER | WS_POPUP | WS_VISIBLE | WS_CHILD,
        200, 300, 400, 480, NULL, NULL, HInstance, NULL);

    EnableWindow(hWndED, TRUE);
    ShowWindow(hWndED, SW_SHOWDEFAULT);
    UpdateWindow(hWndED);
}

void CreateSignCheckWin() {

    hWndSC = CreateWindowEx(0, L"SignCheckWindowClass", L"Sign/Check sign", WS_OVERLAPPEDWINDOW | WS_BORDER | WS_POPUP | WS_VISIBLE | WS_CHILD,
        600, 300, 400, 410, NULL, NULL, HInstance, NULL);

    EnableWindow(hWndSC, TRUE);
    ShowWindow(hWndSC, SW_SHOWDEFAULT);
    UpdateWindow(hWndSC);
}

void CreateHideExtractWin() {

    hWndHE = CreateWindowEx(0, L"HideExtractWindowClass", L"Steganography", WS_OVERLAPPEDWINDOW | WS_BORDER | WS_POPUP | WS_VISIBLE | WS_CHILD,
        1000, 300, 400, 400, NULL, NULL, HInstance, NULL);

    EnableWindow(hWndHE, TRUE);
    ShowWindow(hWndHE, SW_SHOWDEFAULT);
    UpdateWindow(hWndHE);
}

void RegisterEncDecWinClass() { //окно шифрования/дешифрования

    WNDCLASSEX wcexed;
    memset(&wcexed, 0, sizeof(wcexed));	   
    wcexed.cbSize = sizeof(WNDCLASSEX);
    wcexed.style = CS_HREDRAW | CS_VREDRAW;
    wcexed.lpfnWndProc = EncDecWinProc;
    wcexed.hIcon = LoadIcon(HInstance, MAKEINTRESOURCE(IDI_KURSACH1));
    wcexed.hInstance = HInstance;
    wcexed.hbrBackground = CreatePatternBrush(hBitmapED);
    wcexed.lpszClassName = L"EncDecWindowClass";
    RegisterClassEx(&wcexed);
}

void RegisterSignCheckWinClass() { //окно постановки/проверки ЭЦП

    WNDCLASSEX wcexsc;
    memset(&wcexsc, 0, sizeof(wcexsc));
    wcexsc.cbSize = sizeof(WNDCLASSEX);
    wcexsc.style = CS_HREDRAW | CS_VREDRAW;
    wcexsc.lpfnWndProc = SignCheckWinProc;
    wcexsc.hIcon = LoadIcon(HInstance, MAKEINTRESOURCE(IDI_KURSACH1));
    wcexsc.hInstance = HInstance;
    wcexsc.hbrBackground = CreatePatternBrush(hBitmapSC);
    wcexsc.lpszClassName = L"SignCheckWindowClass";
    RegisterClassEx(&wcexsc);
}

void RegisterHideExtractWinClass() { //окно стеганографии

    WNDCLASSEX wcexhe;
    memset(&wcexhe, 0, sizeof(wcexhe));
    wcexhe.cbSize = sizeof(WNDCLASSEX);
    wcexhe.style = CS_HREDRAW | CS_VREDRAW;
    wcexhe.lpfnWndProc = HideExtractWinProc;
    wcexhe.hIcon = LoadIcon(HInstance, MAKEINTRESOURCE(IDI_KURSACH1));
    wcexhe.hInstance = HInstance;
    wcexhe.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcexhe.lpszClassName = L"HideExtractWindowClass";
    RegisterClassEx(&wcexhe);
}

LRESULT CALLBACK EncDecWinProc(HWND hWndED, UINT message, WPARAM wParam, LPARAM lParam) {

    HWND hPStatic, hQStatic, hBStatic, hNStatic;
    static HWND hPEdit, hQEdit, hBEdit, hNEdit;
    HWND hEncBtn, hDecBtn;
    RECT winRect;
    HANDLE threadEnc, threadDec;

    char buf[11] = { 0 };
    int msbED;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        GetWindowRect(hWndED, &winRect);
        SetFocus(hWndED);
        SetWindowPos(hWndED, HWND_TOP, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, SWP_NOSIZE | SWP_NOMOVE);
        UpdateWindow(hWndED);
        break;
    case WM_CREATE:
        SetOpenFileParams(hWndED);
        hPStatic = CreateWindow(L"static", L"P =", WS_CHILD | WS_VISIBLE, 120, 50, 40, 20, hWndED, NULL, NULL, NULL);
        hPEdit = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 145, 50, 100, 20, hWndED, NULL, NULL, NULL);
        hQStatic = CreateWindow(L"static", L"Q =", WS_CHILD | WS_VISIBLE, 120, 90, 40, 20, hWndED, NULL, NULL, NULL);
        hQEdit = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 145, 90, 100, 20, hWndED, NULL, NULL, NULL);
        hBStatic = CreateWindow(L"static", L"B =", WS_CHILD | WS_VISIBLE, 120, 130, 40, 20, hWndED, NULL, NULL, NULL);
        hBEdit = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 145, 130, 100, 20, hWndED, NULL, NULL, NULL);
        hNStatic = CreateWindow(L"static", L"N =", WS_CHILD | WS_VISIBLE, 120, 170, 40, 20, hWndED, NULL, NULL, NULL);
        hNEdit = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 145, 170, 100, 20, hWndED, NULL, NULL, NULL);
        hEncBtn = CreateWindow(L"button", L"Encipher", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 85, 220, 80, 30, hWndED, reinterpret_cast<HMENU>(BTN_ENC), NULL, NULL);
        hDecBtn = CreateWindow(L"button", L"Decipher", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 215, 220, 80, 30, hWndED, reinterpret_cast<HMENU>(BTN_DEC), NULL, NULL);
        hPTEdit = CreateWindow(L"edit", L"plaintext", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_LEFT, 50, 280, 280, 60, hWndED, NULL, NULL, NULL);
        hCTEdit = CreateWindow(L"edit", L"ciphertext", WS_CHILD | WS_VISIBLE | WS_BORDER | WS_VSCROLL | ES_MULTILINE | ES_LEFT, 50, 360, 280, 60, hWndED, NULL, NULL, NULL);
        break;
    case WM_COMMAND: //191, 199, 45
    {     
        GetWindowTextA(hPEdit, buf, 10);
        p = atoi((const char*)buf);
        GetWindowTextA(hQEdit, buf, 10);
        q = atoi((const char*)buf);
        GetWindowTextA(hBEdit, buf, 10);
        b = atoi((const char*)buf);

        switch (LOWORD(wParam)) 
        {
        case BTN_ENC:
            if (p == 0 || q == 0 || b == 0) {
                msbED = MessageBox(hWndED, L"Enter P, Q, B: natural numbers", L"error", MB_OK);
                //SetWindowTextA(hPEdit, "");
                break;
            }
            if (!InputValidationED(p, q, b, n)) 
                break;

            _itoa(n, buf, 10);
            SetWindowTextA(hNEdit, buf);
            if (GetOpenFileName(&ofn)) {
                threadEnc = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Encipher, NULL, NULL, NULL);
            }
        break;

        case BTN_DEC:
            if (p == 0 || q == 0 || b == 0) {
                msbED = MessageBox(hWndED, L"Enter P, Q, B: natural numbers", L"error", MB_OK);
                break;
            }
            if (!InputValidationED(p, q, b, n))
                break;

            _itoa(n, buf, 10);
            SetWindowTextA(hNEdit, buf);
            if (GetOpenFileName(&ofn)) {
                threadDec = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Decipher, NULL, NULL, NULL);
            }
        break;
        }

        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWndED, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWndED);
            break;
        default:
            return DefWindowProc(hWndED, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWndED, &ps);
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
        EndPaint(hWndED, &ps);
    }
    break;
    default:
        return DefWindowProc(hWndED, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK SignCheckWinProc(HWND hWndSC, UINT message, WPARAM wParam, LPARAM lParam) {

    HWND hPStatic, hQStatic, hDStatic, hEStatic;
    static HWND hPEdit, hQEdit, hDEdit, hEEdit;
    HWND hSignBtn, hCheckBtn;
    RECT winRect;
    HANDLE threadSign, threadCheck;

    char buf[11] = { 0 };
    int msbSC;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        GetWindowRect(hWndSC, &winRect);
        SetFocus(hWndSC);
        SetWindowPos(hWndSC, HWND_TOP, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, SWP_NOSIZE | SWP_NOMOVE);
        UpdateWindow(hWndSC);
        break;
    case WM_CREATE:
        SetOpenFileParams(hWndSC);
        hPStatic = CreateWindow(L"static", L"P =", WS_CHILD | WS_VISIBLE, 120, 50, 40, 20, hWndSC, NULL, NULL, NULL);
        hPEdit = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 145, 50, 100, 20, hWndSC, NULL, NULL, NULL);
        hQStatic = CreateWindow(L"static", L"Q =", WS_CHILD | WS_VISIBLE, 120, 90, 40, 20, hWndSC, NULL, NULL, NULL);
        hQEdit = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 145, 90, 100, 20, hWndSC, NULL, NULL, NULL);
        hDStatic = CreateWindow(L"static", L"D =", WS_CHILD | WS_VISIBLE, 120, 130, 40, 20, hWndSC, NULL, NULL, NULL);
        hDEdit = CreateWindow(L"edit", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 145, 130, 100, 20, hWndSC, NULL, NULL, NULL);
        hEStatic = CreateWindow(L"static", L"E =", WS_CHILD | WS_VISIBLE, 120, 170, 40, 20, hWndSC, NULL, NULL, NULL);
        hEEdit = CreateWindow(L"static", L"", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 145, 170, 100, 20, hWndSC, NULL, NULL, NULL);
        hSignBtn = CreateWindow(L"button", L"Sign file", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 85, 220, 80, 30, hWndSC, reinterpret_cast<HMENU>(BTN_SIGN), NULL, NULL);
        hCheckBtn = CreateWindow(L"button", L"Check sign", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 215, 220, 80, 30, hWndSC, reinterpret_cast<HMENU>(BTN_CHECK), NULL, NULL);
        hHEdit = CreateWindow(L"edit", L"file hash", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 50, 280, 280, 20, hWndSC, NULL, NULL, NULL);
        hSEdit = CreateWindow(L"edit", L"signature", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 50, 320, 280, 20, hWndSC, NULL, NULL, NULL);
        break;
    case WM_COMMAND: //13, 17, 7
    {
        GetWindowTextA(hPEdit, buf, 10);
        p1 = atoi((const char*)buf);
        GetWindowTextA(hQEdit, buf, 10);
        q1 = atoi((const char*)buf);
        GetWindowTextA(hDEdit, buf, 10);
        d = atoi((const char*)buf);

        switch (LOWORD(wParam))
        {
        case BTN_SIGN:
            if (p1 == 0 || q1 == 0 || d == 0) {
                msbSC = MessageBox(hWndSC, L"Enter P, Q, D: natural numbers", L"error", MB_OK);
                break;
            }
            if (!InputValidationS(p1, q1, d, e))
                break;

            _itoa(e, buf, 10);
            SetWindowTextA(hEEdit, buf);
            if (GetOpenFileName(&ofn)) {
                threadSign = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Sign, NULL, NULL, NULL);
            }
            break;

        case BTN_CHECK:
            if (p1 == 0 || q1 == 0 || d == 0) {
                msbSC = MessageBox(hWndSC, L"Enter P, Q, B: natural numbers", L"error", MB_OK);
                break;
            }
            if (!InputValidationS(p1, q1, d, e))
                break;

            _itoa(e, buf, 10);
            SetWindowTextA(hEEdit, buf);
            if (GetOpenFileName(&ofn)) {
                threadCheck = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Check, NULL, NULL, NULL);
            }
            break;
        }

        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWndSC, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWndSC);
            break;
        default:
            return DefWindowProc(hWndSC, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWndSC, &ps);
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
        EndPaint(hWndSC, &ps);
    }
    break;
    default:
        return DefWindowProc(hWndSC, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK HideExtractWinProc(HWND hWndHE, UINT message, WPARAM wParam, LPARAM lParam) {

    HWND hHideBtn, hExtractBtn;
    RECT winRect;

    HANDLE threadHide, threadExtract;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        GetWindowRect(hWndHE, &winRect);
        SetFocus(hWndHE);
        SetWindowPos(hWndHE, HWND_TOP, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, SWP_NOSIZE | SWP_NOMOVE);
        UpdateWindow(hWndHE);
        break;
    case WM_CREATE:
        SetOpenFileParams(hWndHE);
        hHideBtn = CreateWindow(L"button", L"hide", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 50, 80, 30, hWndHE, reinterpret_cast<HMENU>(BTN_HIDE), NULL, NULL);
        hExtractBtn = CreateWindow(L"button", L"extract", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 50, 80, 30, hWndHE, reinterpret_cast<HMENU>(BTN_EXTRACT), NULL, NULL);
        hSTextEdit = CreateWindow(L"edit", L"text", WS_CHILD | WS_VISIBLE | WS_BORDER | ES_LEFT, 50, 100, 300, 20, hWndHE, NULL, NULL, NULL);
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case BTN_HIDE:
            if (GetOpenFileName(&ofn)) {
                threadHide = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Hide, NULL, NULL, NULL);
            }
            break;

        case BTN_EXTRACT:
            if (GetOpenFileName(&ofn)) {
                threadExtract = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Extract, NULL, NULL, NULL);
            }
            break;
        }

        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWndHE, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWndHE);
            break;
        default:
            return DefWindowProc(hWndHE, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWndHE, &ps);
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
        EndPaint(hWndHE, &ps);
    }
    break;
    default:
        return DefWindowProc(hWndHE, message, wParam, lParam);
    }
    return 0;
}

LRESULT CALLBACK WndProc(HWND hWndMain, UINT message, WPARAM wParam, LPARAM lParam)
{
    hMutexOF = CreateMutex(NULL, FALSE, NULL);
    hMutex = CreateMutex(NULL, FALSE, NULL);

    RECT winRect;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        GetWindowRect(hWndMain, &winRect);
        SetFocus(hWndMain);
        SetWindowPos(hWndMain, HWND_TOP, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, SWP_NOSIZE | SWP_NOMOVE);
        UpdateWindow(hWndMain);
        break;
    case WM_CREATE:       
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) {
        case BTN_ENC_DEC:
            CreateEncDecWin();
            UpdateWindow(hWndMain);
            break;
        case BTN_SIGN_CHECK:
            CreateSignCheckWin();
            UpdateWindow(hWndMain);
            break;
        case BTN_HIDE_EXTRACT:
            CreateHideExtractWin();
            UpdateWindow(hWndMain);
            break;
        }
        int wmId = LOWORD(wParam);
        switch (wmId)
        {
        case IDM_ABOUT:
            DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWndMain, About);
            break;
        case IDM_EXIT:
            DestroyWindow(hWndMain);
            break;
        default:
            return DefWindowProc(hWndMain, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWndMain, &ps);
        // TODO: Добавьте сюда любой код прорисовки, использующий HDC...
        EndPaint(hWndMain, &ps);
    }
    break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWndMain, message, wParam, lParam);
    }
    return 0;
}


INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}

void SetOpenFileParams(HWND hWnd) {
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = hWnd;
    ofn.lpstrFile = filename;
    ofn.nMaxFile = sizeof(filename);
    ofn.lpstrFilter = L"All\0*.*\0";
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;
}

LPVOID ReadFromFile(LPWSTR path) {
    HANDLE LoadFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    ReadFile(LoadFile, Buf, BUF, &bytesRead, NULL); 
    
    CloseHandle(LoadFile);
    return Buf;
}

vector<int> IntReadFromFile(LPWSTR path) {
    HANDLE LoadFile = CreateFile(path, GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

    DWORD bytes = 0;
    ReadFile(LoadFile, Buf, BUF, &bytes, NULL);
    try
    {
        BOOL flag = TRUE;
        int pos = 0;
        int i = 0;
        int length = BUF;
        unsigned char b[4] = { 0 };
        vector<int> res(bytes);
        while (pos < length && flag)
        {
            if (Buf[pos] == '\0') {
                flag = FALSE;
                continue;
            }
            for (int k = 0; k < 4; k++)
                b[k] = Buf[pos + k];
            memcpy(&res[i], b, 4);
            pos += 4;
            i++;
        }       
        CloseHandle(LoadFile);
        return res;
    }
    catch (...)
    {
        int msbRF = MessageBox(hWndED, L"read from file error.", L"error", MB_OK);
        CloseHandle(LoadFile);
        return vector<int>();
    }
}

void WriteToFile(unsigned char Bytes[BUF], LPWSTR path, int bytesWrite) {
    HANDLE WFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    WriteFile(WFile, Bytes, bytesWrite, NULL, NULL);
    CloseHandle(WFile);
}

void IntWriteToFile(int Data[BUF / 4], LPWSTR path) {
    HANDLE WFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    WriteFile(WFile, Data, bytesRead * 4, NULL, NULL); 
    CloseHandle(WFile);
}

DWORD WINAPI Encipher() { 

    WaitForSingleObject(hMutex, INFINITE);
    ReadFromFile(ofn.lpstrFile);
    int readQu = bytesRead;

    int* IntArr = (int*)calloc(readQu, sizeof(int));
    int count = 0;
    for (int i = 0; i < readQu; i++) {
        IntArr[i] = Buf[i] * (Buf[i] + b) % n;
        count++;
    }

    SetWindowTextA(hPTEdit, Buf);
    char res[BUF] = { 0 };
    IntToString(IntArr, res, count);
    SetWindowTextA(hCTEdit, res);
    
    int msbE = MessageBox(hWndED, L"enciphered. save to file?", L"result", MB_OK | MB_OKCANCEL);
    if (msbE == IDOK) {
        if (GetSaveFileName(&ofn)) {
            IntWriteToFile(IntArr, ofn.lpstrFile);
        }
    }

    free(IntArr);
    ClearBuf(Buf); 
    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Decipher() {

    WaitForSingleObject(hMutex, INFINITE);
    vector<int> originalCryptData = IntReadFromFile(ofn.lpstrFile);

    int D, d1, d2, d3, d4, Mp, Mq, M;
    int yp, yq;
    ExtendedEuclideanAlgorithm(p, q, yp, yq);

    int i = 0;
    int intsCount = 0;
    int arrLen = originalCryptData.size();
    for (int i = 0; i < arrLen; i++) {
        if (originalCryptData[i] != 0)
            intsCount++;
        else continue;
    }
    BOOL flag = TRUE;
    while (i < arrLen && flag) {
        if (originalCryptData[i] == '\0') {
            flag = FALSE;
            continue;
        }

        D = (b * b + 4 * originalCryptData[i]) % n;
        Mp = (int)FastExponentiation(D, (p + 1) / 4, p);
        Mq = (int)FastExponentiation(D, (q + 1) / 4, q);
        long mul1 = p * Mq; mul1 *= yp;
        long mul2 = q * Mp; mul2 *= yq;
        long add = mul1 + mul2;
        long sub = mul1 - mul2;

        d1 = (int)(add % n);
        if ((d1 - b) % 2 == 0)
            M = (-b + d1) / 2 % n;
        else
            M = (-b + n + d1) / 2 % n;

        if ((unsigned char)M == M)
            ByteArr[i] = (unsigned char)M;
        else {
            d2 = n - d1;
            if ((d2 - b) % 2 == 0)
                M = (-b + d2) / 2 % n;
            else
                M = (-b + n + d2) / 2 % n;

            if ((unsigned char)M == M)
                ByteArr[i] = (unsigned char)M;
            else {
                d3 = (int)(sub % n);
                if ((d3 - b) % 2 == 0)
                    M = (-b + d3) / 2 % n;
                else
                    M = (-b + n + d3) / 2 % n;

                if ((unsigned char)M == M)
                    ByteArr[i] = (unsigned char)M;
                else {
                    d4 = n - d3;
                    if ((d4 - b) % 2 == 0)
                        M = (-b + d4) / 2 % n;
                    else
                        M = (-b + n + d4) / 2 % n;

                    if ((unsigned char)M == M)
                        ByteArr[i] = (unsigned char)M;
                    else
                        ByteArr[i] = 0;
                    }
                }
            }
        i++;
    }
    
    int* intCryptData = (int*)calloc(arrLen, sizeof(int));
    for (int i = 0; i < intsCount; i++) 
        intCryptData[i] = originalCryptData[i];

    char res[BUF] = { 0 };
    IntToString(intCryptData, res, arrLen);
    SetWindowTextA(hPTEdit, res);
    SetWindowTextA(hCTEdit, (LPCSTR)ByteArr);

    int msbD = MessageBox(hWndED, L"deciphered. save to file?", L"result", MB_OK | MB_OKCANCEL);
    if (msbD == IDOK) {
        if (GetSaveFileName(&ofn)) {
            WriteToFile(ByteArr, ofn.lpstrFile, intsCount);
        }
    }

    free(intCryptData);
    ClearBuf(Buf);
    ClearByteArr(ByteArr);
    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Sign() {

    WaitForSingleObject(hMutex, INFINITE);
    ReadFromFile(ofn.lpstrFile);
    int readQu = bytesRead;

    int r = p1 * q1;
    int h = GetDigest(Buf, r, readQu);
    int signature = FastExponentiation(h, d, r);

    unsigned char* signedFile = new unsigned char[readQu + 7];
    for (int i = 0; i < readQu; i++)
        signedFile[i] = Buf[i];
    signedFile[readQu] = '\r';
    signedFile[readQu + 1] = '\n';
  
    char b[4] = { 0 };
    _itoa(signature, b, 10);
    for (int j = 0; j < 4; j++)
        signedFile[readQu + 2 + j] = b[j];

    SetWindowTextA(hHEdit, b);
    _itoa(h, b, 10);
    SetWindowTextA(hSEdit, b);

    int msb = MessageBox(hWndSC, L"signed. save to file?", L"result", MB_OK | MB_OKCANCEL); 
    if (msb == IDOK) {
        if (GetSaveFileName(&ofn)) {
            WriteToFile(signedFile, ofn.lpstrFile, readQu + 6);
        }
    }

    delete [] signedFile;
    ClearBuf(Buf);
    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Check() {

    WaitForSingleObject(hMutex, INFINITE);
    ReadFromFile(ofn.lpstrFile);
    int readQu = bytesRead;

    unsigned char b[4] = { 0 };
    int i = readQu - 1;
    while (Buf[i] != '\n') 
        i--;
    int k = 0;
    for (i = i + 1; i < readQu; i++) 
        if (Buf[i] != '\0') {
            b[k] = Buf[i];
            k++;
        }

    int signInFile = atoi((const char*)b);

    int r = p1 * q1;
    char* fileText = new char[readQu - 5];
    for (int k = 0; k < readQu - 6; k++)
        fileText[k] = Buf[k];
        
    int fileDigest = GetDigest(fileText, r, readQu - 6);
    int digest = FastExponentiation(signInFile, e, r);
    int signature = FastExponentiation(digest, d, r);

    char buf[4] = { 0 };
    _itoa(digest, buf, 10);
    SetWindowTextA(hHEdit, buf);
    _itoa(signature, buf, 10);
    SetWindowTextA(hSEdit, buf);

    int msb;
    if (signInFile == 0)
        msb = MessageBox(hWndSC, L"file is not signed", L"no sign", MB_OK);
    else if (digest == fileDigest)
        msb = MessageBox(hWndSC, L"signs coincide", L"ok", MB_OK);
    else 
        msb = MessageBox(hWndSC, L"signs differ", L"diff signs", MB_OK);

    delete[] fileText;
    ClearBuf(Buf);
    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Hide() {

    WaitForSingleObject(hMutex, INFINITE);

    unsigned char info[54] = { 0 };
    ReadFromFile(ofn.lpstrFile);
    for (int i = 0; i < BUF; i++) {
        sBuf[i] = (Buf[i] + 256) % 256;
    }
    int readBytes = bytesRead;
    unsigned char* bytes = new unsigned char[readBytes - 54 + 1];
    for (int i = 0; i < 54; i++) {
        info[i] = sBuf[i];
    }
    for (int j = 54; j < readBytes; j++) {
        bytes[j - 54] = sBuf[j];
    }

    unsigned char buf[100];
    int num = GetWindowTextA(hSTextEdit, (char *)buf, 100);

    unsigned char textBits[100][8] = { 0 };
    int pos = 0;
    for (int i = 0; i < num; i++) {
        unsigned char bits[8];
        ByteToBit(buf[i], bits);

        for (int j = 0; j < 8; j++) {
            bytes[3 * (pos + j)] = bytes[3 * (pos + j)] & 254 | bits[j];
        }

        pos += 8;
    }

    for (int j = 0; j < 8; j++) {
        bytes[3 * (pos + j)] = '\0';
    }


    //Save bmp
    SaveBMP(info, bytes, readBytes);

    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Extract() {

    WaitForSingleObject(hMutex, INFINITE);

    unsigned char info[54] = { 0 };
    ReadFromFile(ofn.lpstrFile);
    for (int i = 0; i < BUF; i++) {
        sBuf[i] = (Buf[i] + 256) % 256;
    }
    int readBytes = bytesRead;
    unsigned char* bytes = new unsigned char[readBytes - 54 + 1];
    for (int i = 0; i < 54; i++) {
        info[i] = sBuf[i];
    }
    for (int j = 54; j < readBytes; j++) {
        bytes[j - 54] = sBuf[j];
    }

    unsigned char text[100] = { 0 };
    for (int i = 0; i < readBytes - 54; i += 24) {
        unsigned char bits[8];
        for (int j = 0; j < 8; j++) {
            bits[j] = bytes[i + j * 3];
        }

        text[i / 24] = BitsToByte(bits);
    }

    SetWindowTextA(hSTextEdit, (char *)text);

    ReleaseMutex(hMutex);
    ExitThread(0);
}

void ClearBuf(char(&Buf)[BUF]) {
    int i = 0;
    BOOL flag = TRUE;

    while (i < sizeof(Buf) && flag) {
        if (Buf[i] == '\0') {
            flag = FALSE;
            continue;
        }
        Buf[i] = '\0';
        i++;
    }
}

void ClearByteArr(unsigned char(&ByteArr)[BUF]) {
    int i = 0;
    BOOL flag = TRUE;

    while (i < sizeof(ByteArr) && flag) {
        if (ByteArr[i] == '\0') {
            flag = FALSE;
            continue;
        }
        ByteArr[i] = '\0';
        i++;
    }
}

void IntToString(int* Text, char(&res)[BUF], int count) {
    for (int i = 0; i < count * 4; i += 4) {
        for (int j = 0; j < 4; j++) {
            res[i + j] = (char)(Text[i / 4] >> j * 8);
            if (res[i + j] == '\0')
                res[i + j] = (char)32;
        }
    }
}

BOOL InputValidationED(int p, int q, int b, int &n) {    
    int msb;
    if (!IsPrime(p)) {
        msb = MessageBox(hWndED, L"P must be a prime number", L"error", MB_OK);
        return FALSE;
    }
    if (!IsPrime(q)) {
        msb = MessageBox(hWndED, L"Q must be a prime number", L"error", MB_OK);
        return FALSE;
    }
    if (p % 4 != 3) {
        msb = MessageBox(hWndED, L"Condition P = 3 mod 4 is not met", L"error", MB_OK);
        return FALSE;
    }
    if (q % 4 != 3) {
        msb = MessageBox(hWndED, L"Condition Q = 3 mod 4 is not met", L"error", MB_OK);
        return FALSE;
    }
    if (q == p) {
        msb = MessageBox(hWndED, L"P and Q must not be equal", L"error", MB_OK);
        return FALSE;
    }
    n = p * q;
    if (n >= INT_MAX) {
        msb = MessageBox(hWndED, L"Too large N. Choose other P and Q", L"error", MB_OK);
        return FALSE;
    }
    if (b >= n) {
        msb = MessageBox(hWndED, L"Condition B < N is not met", L"error", MB_OK);
        return FALSE;
    }
    return TRUE;
}

BOOL InputValidationS(int p, int q, int d, int& e) {
    int msb;
    if (!IsPrime(p)) {
        msb = MessageBox(hWndSC, L"P must be a prime number", L"error", MB_OK);
        return FALSE;
    }
    if (!IsPrime(q)) {
        msb = MessageBox(hWndSC, L"Q must be a prime number", L"error", MB_OK);
        return FALSE;
    }
    int phi_r = (p - 1) * (q - 1);
    if (d < 2) {
        msb = MessageBox(hWndSC, L"D must be greater than 1", L"error", MB_OK);
        return FALSE;
    }
    if (d >= phi_r) {
        msb = MessageBox(hWndSC, L"D must be less than Phi(r)", L"error", MB_OK);
        return FALSE;
    }
    if (GetGCD(d, phi_r) != 1) {
        msb = MessageBox(hWndSC, L"D must be coprime with Phi(r)", L"error", MB_OK);
        return FALSE;
    }
    e = MultInverse(d, phi_r);
    return TRUE;
}

BOOL IsPrime(int x) {
    int i = 2;
    while (i * i <= x) {
        if (x % i == 0)
            return FALSE;
        i++;
    }
    return TRUE;
}

void ExtendedEuclideanAlgorithm(int num1, int num2, int &res1, int &res2) {
    int d0 = num1, d1 = num2;
    int x0 = 1, x1 = 0;
    int y0 = 0, y1 = 1;
    int q, d2, x2, y2;
    while (d1 > 1) {
        q = d0 / d1;
        d2 = d0 % d1;
        x2 = x0 - q * x1;
        y2 = y0 - q * y1;
        d0 = d1;
        d1 = d2;
        x0 = x1;
        x1 = x2;
        y0 = y1;
        y1 = y2;
    }
    res1 = x1;
    res2 = y1;
}

long FastExponentiation(long number, int degree, int mod) {
    long res = 1;
    while (degree != 0) {
        while (degree % 2 == 0) {
            number = number * number % mod;
            degree /= 2;
        }
        res = res * number % mod;
        degree--;
    }
    return res;
}

int GetGCD(int a, int b) {
    int t;
    while (b > 0) {
        t = b;
        b = a % b;
        a = t;
    }
    return (a);
}

int MultInverse(int d, int mod) {
    int g0 = mod, g1 = d, u0 = 1, v0 = 0, u1 = 0, v1 = 1;
    while (g1 != 0) {
        int y = (int)(g0 / g1);
        int gi = g0 - y * g1;
        int ui = u0 - y * u1;
        int vi = v0 - y * v1;
        g0 = g1;
        g1 = gi;
        u0 = u1;
        u1 = ui;
        v0 = v1;
        v1 = vi;
    }
    int x = v0;
    if (x >= 0)
        return x;
    return (x + mod);
}

int GetDigest(char* text, int r, int readQu) {
    int h = 100;
    for (int i = 0; i < readQu; i++)
        h = (h + text[i]) * (h + text[i]) % r;
    return h;
}

void ByteToBit(unsigned char byte, unsigned char (&bits)[8]) {
    BOOL st = TRUE;
    for (int i = 0; i < 8; i++)
    {
        if ((byte >> i & 1) == 1) {
            st = TRUE;
        }
        else st = FALSE;
        bits[i] = st;
    }
}

unsigned char BitsToByte(unsigned char bits[8]) {
    unsigned char byte = 0;
    for (int i = 0; i < 8; i++)
        if (bits[i] & 1)    //  EXACTLY BINARY AND
            byte += (unsigned char)pow(2, i);
    return byte;
}

void SaveBMP(unsigned char info[54], unsigned char* data, int readBytes) {
    int sizeInfo = sizeof(info);
    int sizeData = sizeof(data);
    unsigned char* all = new unsigned char[readBytes + 1];
    int allSize = sizeof(all);
    for (int i = 0; i < 54; i++)
        all[i] = info[i];
    for (int j = 54; j < readBytes; j++)
        all[j] = data[j - 54];
    
    FILE* output = fopen("file.bmp", "wb+");
    fwrite(all, sizeof(unsigned char), readBytes, output);
    fclose(output);
    //delete[] all;
}