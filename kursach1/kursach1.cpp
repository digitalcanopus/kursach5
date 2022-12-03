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

#define WS_DATAENTRYWINDOW WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX | WS_DLGFRAME | WS_BORDER | WS_POPUP | WS_VISIBLE | WS_CHILD
#define BTN_ENC_DEC 1
#define BTN_ENC 2
#define BTN_DEC 3

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
HWND hBtnED;
HWND hWndMain;
HINSTANCE HInstance;
HWND hWndED;
int NCmdShow;

ATOM RegisterEncDecWindow(HINSTANCE);
void RegisterEncDecWinClass();
LRESULT CALLBACK EncDecWinProc(HWND, UINT, WPARAM, LPARAM);
void CreateEncDecWin();

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    HInstance = hInstance;
    NCmdShow = nCmdShow;

    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_KURSACH1, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    hBtnED = CreateWindowEx(0, L"button", L"Encipher/Decipher", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON | WS_TABSTOP,
        50, 50, 150, 40, hWndMain, reinterpret_cast<HMENU>(BTN_ENC_DEC), hInstance, NULL);

    RegisterEncDecWinClass();

    ShowWindow(hWndED, SW_HIDE);

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
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_KURSACH1);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}


BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance;

   hWndMain = CreateWindowW(szWindowClass, L"Super kursach", WS_OVERLAPPEDWINDOW,
      CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

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
        100, 100, 800, 600, NULL, NULL, HInstance, NULL);

    EnableWindow(hWndED, TRUE);
    ShowWindow(hWndED, SW_SHOWDEFAULT);
    UpdateWindow(hWndED);
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

LRESULT CALLBACK EncDecWinProc(HWND hWndED, UINT message, WPARAM wParam, LPARAM lParam) {

    HWND hEncBtn, hDecBtn;
    BOOL flag = TRUE;
    int i;
    int msb;

    RECT winRect;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        GetWindowRect(hWndED, &winRect);
        SetFocus(hWndED);
        //SetForegroundWindow(hWndED);
        SetWindowPos(hWndED, HWND_TOP, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, SWP_NOSIZE | SWP_NOMOVE);
        UpdateWindow(hWndED);
        break;
    case WM_CREATE:
        SetOpenFileParams(hWndED);
        hEncBtn = CreateWindow(L"button", L"encipher", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 10, 10, 70, 30, hWndED, reinterpret_cast<HMENU>(BTN_ENC), NULL, NULL);
        hDecBtn = CreateWindow(L"button", L"decipher", WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, 100, 10, 70, 30, hWndED, reinterpret_cast<HMENU>(BTN_DEC), NULL, NULL);
        break;
    case WM_COMMAND:
    {
        switch (LOWORD(wParam)) {
        case BTN_ENC:

            if (GetOpenFileName(&ofn)) {
                ReadFromFile(ofn.lpstrFile);

                i = 0;
                while (i < sizeof(Buf) && flag) {
                    if (Buf[i] == '\0') {
                        flag = FALSE;
                        continue;
                    }

                    ByteArr[i] = Buf[i];
                    //encipher

                    i++;
                }

                msb = MessageBox(hWndED, L"enciphered. save to file?", L"result", MB_OK | MB_OKCANCEL);
                if (msb == IDOK) {
                    if (GetSaveFileName(&ofn)) {
                        WriteToFile(ByteArr, ofn.lpstrFile);
                    }
                }

                ClearBufs(ByteArr, Buf);
            }

            break;

        case BTN_DEC:

            if (GetOpenFileName(&ofn)) {
                ReadFromFile(ofn.lpstrFile);

                i = 0;
                while (i < sizeof(Buf) && flag) {
                    if (Buf[i] == '\0') {
                        flag = FALSE;
                        continue;
                    }

                    ByteArr[i] = Buf[i];
                    //decipher

                    i++;
                }

                msb = MessageBox(hWndED, L"deciphered. save to file?", L"result", MB_OK | MB_OKCANCEL);
                if (msb == IDOK) {
                    if (GetSaveFileName(&ofn)) {
                        WriteToFile(ByteArr, ofn.lpstrFile);
                    }
                }

                ClearBufs(ByteArr, Buf);
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
    //case WM_DESTROY:
        //PostQuitMessage(0);
        //break;
    default:
        return DefWindowProc(hWndED, message, wParam, lParam);
    }
    return 0;
}


LRESULT CALLBACK WndProc(HWND hWndMain, UINT message, WPARAM wParam, LPARAM lParam)
{
    RECT winRect;
    BOOL d;

    switch (message)
    {
    case WM_LBUTTONDOWN:
        GetWindowRect(hWndMain, &winRect);
        SetFocus(hWndMain);
        //d = SetForegroundWindow(hWndMain);
        d = SetWindowPos(hWndMain, HWND_TOP, winRect.left, winRect.top, winRect.right - winRect.left, winRect.bottom - winRect.top, SWP_NOSIZE | SWP_NOMOVE);
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
