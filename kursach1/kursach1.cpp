#pragma comment(lib, "Msimg32.lib")
#define _CRT_SECURE_NO_WARNINGS

#include "framework.h"
#include "kursach1.h"
#include <Windows.h>
#include <commdlg.h>
#include <string>
#include <stdio.h>

#define MAX_LOADSTRING 100
#define BUF 2048

#define BCKG_NAME L"C:\\Users\\Lenovo\\Downloads\\ClpYqHIejcs.bmp" //L"C:\\Users\\Lenovo\\OneDrive\\Рабочий стол\\осасп2\\лаб1\\lab1.3\\penguin.bmp"

#define WS_DATAENTRYWINDOW WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_DLGFRAME | WS_BORDER | WS_POPUP | WS_VISIBLE | WS_CHILD
#define BTN_ENC_DEC 1
#define BTN_ENC 2
#define BTN_DEC 3
#define BTN_SIGN_CHECK 4
#define BTN_SIGN 5
#define BTN_CHECK 6
#define BTN_HIDE_EXTRACT 7
#define BTN_HIDE 8
#define BTN_EXTRACT 9
#define STATIC_MAINWIN 21

using namespace std;

static char Buf[BUF];
wchar_t filename[260] = L"";
OPENFILENAME ofn = { 0 };
static unsigned char ByteArr[BUF] = { 0 };

HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

void SetOpenFileParams(HWND);
LPVOID ReadFromFile(LPWSTR);
void WriteToFile(unsigned char [BUF], LPWSTR);
void ClearBufs(unsigned char (&)[BUF], char (&)[BUF]);
DWORD WINAPI Encipher();
DWORD WINAPI Decipher();
DWORD WINAPI Sign();
DWORD WINAPI Check();
DWORD WINAPI Hide();
DWORD WINAPI Extract();

HBITMAP hBitmap;
HWND hWndMain;
HINSTANCE HInstance;
HWND hWndED, hWndSC, hWndHE;
int NCmdShow;
HANDLE hMutex = NULL;

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
    HWND hwndStatic = CreateWindow(L"static", L"Created by Bozhena Marchik, 2022", WS_CHILD | WS_VISIBLE | WS_BORDER, 5, 315, 230, 20, hWndMain, (HMENU)STATIC_MAINWIN, hInstance, NULL);

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
    wcex.hbrBackground  = (HBRUSH)(COLOR_GRAYTEXT + 13); 
    //wcex.hbrBackground = CreatePatternBrush(hBitmap);
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
        200, 300, 400, 400, NULL, NULL, HInstance, NULL);

    EnableWindow(hWndED, TRUE);
    ShowWindow(hWndED, SW_SHOWDEFAULT);
    UpdateWindow(hWndED);
}

void CreateSignCheckWin() {

    hWndSC = CreateWindowEx(0, L"SignCheckWindowClass", L"Sign/Check sign", WS_OVERLAPPEDWINDOW | WS_BORDER | WS_POPUP | WS_VISIBLE | WS_CHILD,
        600, 300, 400, 400, NULL, NULL, HInstance, NULL);

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
    wcexed.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcexed.lpszClassName = L"EncDecWindowClass";
    RegisterClassEx(&wcexed);
}

void RegisterSignCheckWinClass() { //окно шифрования/дешифрования

    WNDCLASSEX wcexsc;
    memset(&wcexsc, 0, sizeof(wcexsc));
    wcexsc.cbSize = sizeof(WNDCLASSEX);
    wcexsc.style = CS_HREDRAW | CS_VREDRAW;
    wcexsc.lpfnWndProc = SignCheckWinProc;
    wcexsc.hIcon = LoadIcon(HInstance, MAKEINTRESOURCE(IDI_KURSACH1));
    wcexsc.hInstance = HInstance;
    wcexsc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcexsc.lpszClassName = L"SignCheckWindowClass";
    RegisterClassEx(&wcexsc);
}

void RegisterHideExtractWinClass() { //окно шифрования/дешифрования

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

    HWND hEncBtn, hDecBtn;
    RECT winRect;

    HANDLE threadEnc, threadDec;

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
        hEncBtn = CreateWindow(L"button", L"encipher", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 50, 70, 30, hWndED, reinterpret_cast<HMENU>(BTN_ENC), NULL, NULL);
        hDecBtn = CreateWindow(L"button", L"decipher", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 50, 70, 30, hWndED, reinterpret_cast<HMENU>(BTN_DEC), NULL, NULL);
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) 
        {
        case BTN_ENC:
            if (GetOpenFileName(&ofn)) {
                threadEnc = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Encipher, NULL, NULL, NULL);
            }
        break;

        case BTN_DEC:
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

    HWND hSignBtn, hCheckBtn;
    RECT winRect;

    HANDLE threadSign, threadCheck;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        GetWindowRect(hWndSC, &winRect);
        SetFocus(hWndSC);
        SetWindowPos(hWndSC, HWND_TOP, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, SWP_NOSIZE | SWP_NOMOVE);
        UpdateWindow(hWndSC);
        break;
    case WM_CREATE:
        SetOpenFileParams(hWndED);
        hSignBtn = CreateWindow(L"button", L"sign file", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 50, 50, 80, 30, hWndSC, reinterpret_cast<HMENU>(BTN_SIGN), NULL, NULL);
        hCheckBtn = CreateWindow(L"button", L"check sign", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 150, 50, 80, 30, hWndSC, reinterpret_cast<HMENU>(BTN_CHECK), NULL, NULL);
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case BTN_SIGN:
            if (GetOpenFileName(&ofn)) {
                threadSign = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Sign, NULL, NULL, NULL);
            }
            break;

        case BTN_CHECK:
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
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam))
        {
        case BTN_SIGN:
            if (GetOpenFileName(&ofn)) {
                threadHide = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)Hide, NULL, NULL, NULL);
            }
            break;

        case BTN_CHECK:
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
            //SetFocus(hWnd);
            //EnableWindow(hWndMain, FALSE);
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

    DWORD bytes;
    ReadFile(LoadFile, Buf, BUF, &bytes, NULL); //текст в буфере Buf
    
    CloseHandle(LoadFile);
    return Buf;
}

void WriteToFile(unsigned char Bytes[BUF], LPWSTR path) {
    HANDLE WFile = CreateFile(path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

    WriteFile(WFile, Bytes, sizeof(Bytes), NULL, NULL);
    CloseHandle(WFile);
}

void ClearBufs(unsigned char (&ByteArr)[BUF], char (&Buf)[BUF]) {

    int i = 0;
    BOOL flag = TRUE;
    
    while ((i < sizeof(Buf) || i < sizeof(ByteArr)) && flag) {
        if (Buf[i] == '\0' && ByteArr[i] == '\0') {
            flag = FALSE;
            continue;
        }
        Buf[i] = '\0';
        ByteArr[i] = '\0';
        i++;
    }
}

DWORD WINAPI Encipher() {

    WaitForSingleObject(hMutex, INFINITE);

    ReadFromFile(ofn.lpstrFile);

    int i = 0;
    BOOL flag = TRUE;
    while (i < sizeof(Buf) && flag) {
        if (Buf[i] == '\0') {
            flag = FALSE;
            continue;
        }

        ByteArr[i] = Buf[i];
        //encipher

        i++;
    }

    int msb = MessageBox(hWndED, L"enciphered. save to file?", L"result", MB_OK | MB_OKCANCEL);
    if (msb == IDOK) {
        if (GetSaveFileName(&ofn)) {
            WriteToFile(ByteArr, ofn.lpstrFile);
        }
    }

    ClearBufs(ByteArr, Buf);
    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Decipher() {

    WaitForSingleObject(hMutex, INFINITE);

    ReadFromFile(ofn.lpstrFile);

    int i = 0;
    BOOL flag = TRUE;
    while (i < sizeof(Buf) && flag) {
        if (Buf[i] == '\0') {
            flag = FALSE;
            continue;
        }

        ByteArr[i] = Buf[i];
        //decipher

        i++;
    }

    int msb = MessageBox(hWndED, L"deciphered. save to file?", L"result", MB_OK | MB_OKCANCEL);
    if (msb == IDOK) {
        if (GetSaveFileName(&ofn)) {
            WriteToFile(ByteArr, ofn.lpstrFile);
        }
    }

    ClearBufs(ByteArr, Buf);
    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Sign() {

    WaitForSingleObject(hMutex, INFINITE);

    ReadFromFile(ofn.lpstrFile);

    int i = 0;
    BOOL flag = TRUE;
    while (i < sizeof(Buf) && flag) {
        if (Buf[i] == '\0') {
            flag = FALSE;
            continue;
        }

        ByteArr[i] = Buf[i];
        //sign

        i++;
    }

    int msb = MessageBox(hWndED, L"signed. save to file?", L"result", MB_OK | MB_OKCANCEL); 
    if (msb == IDOK) {
        if (GetSaveFileName(&ofn)) {
            WriteToFile(ByteArr, ofn.lpstrFile);
        }
    }

    ClearBufs(ByteArr, Buf);
    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Check() {

    WaitForSingleObject(hMutex, INFINITE);

    ReadFromFile(ofn.lpstrFile);

    int i = 0;
    BOOL flag = TRUE;
    while (i < sizeof(Buf) && flag) {
        if (Buf[i] == '\0') {
            flag = FALSE;
            continue;
        }

        ByteArr[i] = Buf[i];
        //check sign

        i++;
    }

    int msb = MessageBox(hWndED, L"checked. save to file?", L"result", MB_OK | MB_OKCANCEL); //три исхода проверки ЭЦП
    if (msb == IDOK) {
        if (GetSaveFileName(&ofn)) {
            WriteToFile(ByteArr, ofn.lpstrFile);
        }
    }

    ClearBufs(ByteArr, Buf);
    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Hide() {

    WaitForSingleObject(hMutex, INFINITE);

    ReleaseMutex(hMutex);
    ExitThread(0);
}

DWORD WINAPI Extract() {

    WaitForSingleObject(hMutex, INFINITE);

    ReleaseMutex(hMutex);
    ExitThread(0);
}
