// win-hotkeys.cpp : Defines the entry point for the application.

#include "pch.h"
#include "framework.h"
#include "win-hotkeys.h"
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>
#include <tuple>
#include <tchar.h>
#include <shellapi.h>
#include <strsafe.h>

#define MAX_LOADSTRING 100
#define APPWM_ICONNOTIFY (WM_APP + 1)

// Global Variables:
HINSTANCE hInst; // current instance
WCHAR szTitle[MAX_LOADSTRING]; // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING]; // the main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

VOID startApp(LPCTSTR lpApplicationName);

class HotKeysConfig
{
private:
    std::string filename = "hotkeys.conf";
    std::vector<int> modifiersList;
    std::vector<int> vkList;
    std::vector<std::string> programList;
public:
    HotKeysConfig() {};
    HotKeysConfig(std::string _filename) : filename(_filename) {};
    void load();
    const size_t length();
    std::tuple<int, int, std::string> get(size_t index);
};

std::wstring s2ws(const std::string& s)
{
    int slength = (int)s.length() + 1;
    int len = MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, 0, 0);
    wchar_t* buf = new wchar_t[len];
    MultiByteToWideChar(CP_ACP, 0, s.c_str(), slength, buf, len);
    std::wstring r(buf);
    delete[] buf;
    return r;
}

HotKeysConfig config = HotKeysConfig();
HICON hIcon = static_cast<HICON>(LoadImage(NULL, TEXT("small.ico"), IMAGE_ICON, 0, 0, LR_DEFAULTCOLOR | LR_SHARED | LR_DEFAULTSIZE | LR_LOADFROMFILE));
NOTIFYICONDATA nid = {};


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // Initialize global strings
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINHOTKEYS, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Perform application initialization:
    if (!InitInstance(hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINHOTKEYS));

    config.load();
    for (size_t i = 0; i < config.length(); i++)
    {
        int modifiers;
        int vk;
        std::string program;
        std::tie(modifiers, vk, program) = config.get(i);
        if (!RegisterHotKey(NULL, i+1, modifiers, vk))
        {
            MessageBox(NULL, s2ws("Unable to register hotkeys n°" + std::to_string(i+1) + ".").c_str(), L"Error", MB_ICONERROR | MB_OK);
        }
    }

    MSG msg;
    // Main message loop:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        if (msg.message == WM_HOTKEY)
        {
            int id = (int) msg.wParam - 1;
            std::string program = std::get<2>(config.get(id));
            startApp(s2ws(program).c_str());
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
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINHOTKEYS));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINHOTKEYS);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
    hInst = hInstance; // Store instance handle in our global variable

    HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, hInstance, nullptr);

    if (!hWnd)
    {
        return FALSE;
    }

    SendMessage(hWnd, WM_SETICON, ICON_SMALL, (LPARAM) hIcon);
    //Notification
    nid.cbSize = sizeof(nid);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
    nid.uCallbackMessage = APPWM_ICONNOTIFY;
    nid.hIcon = hIcon;
    // This text will be shown as the icon's tooltip.
    StringCchCopy(nid.szTip, ARRAYSIZE(nid.szTip), L"Win Hotkeys");
    // Show the notification.
    Shell_NotifyIcon(NIM_ADD, &nid);

    return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
    {
        int wmId = LOWORD(wParam);
        // Parse the menu selections:
        switch (wmId)
        {
        case IDM_EXIT:
            DestroyWindow(hWnd);
            break;
        default:
            return DefWindowProc(hWnd, message, wParam, lParam);
        }
    }
    break;
    case WM_PAINT:
    {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        // TODO: Add any drawing code that uses hdc here...
        EndPaint(hWnd, &ps);
    }
    break;
    case WM_DESTROY:
        // Hide the notification.
        nid.hWnd = hWnd;
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;
    case APPWM_ICONNOTIFY:
    {
        if (lParam == WM_RBUTTONUP)
        {
            POINT lpClickPoint;
            HMENU hPopMenu;

            UINT uFlag = MF_BYPOSITION | MF_UNCHECKED | MF_STRING;
            GetCursorPos(&lpClickPoint);
            hPopMenu = CreatePopupMenu();
            InsertMenu(hPopMenu, 0, MF_BYPOSITION | MF_STRING, IDM_EXIT, L"Exit");
            SetForegroundWindow(hWnd);
            TrackPopupMenu(hPopMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_BOTTOMALIGN, lpClickPoint.x, lpClickPoint.y, 0, hWnd, NULL);
        }
        return 0;
    }
    break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}



std::vector<std::string> splitString(std::string s, std::string delimiter)
{
    std::vector<std::string> result;
    size_t pos = 0;
    while ((pos = s.find(delimiter)) != std::string::npos)
    {
        result.push_back(s.substr(0, pos));
        s.erase(0, pos + delimiter.length());
    }
    return result;
}

void HotKeysConfig::load() {
    try {
        std::ifstream infile(this->filename);
        std::string modifiers = "", vk = "", program = "";
        std::getline(infile, modifiers);
        if (modifiers == "")
            std::getline(infile, modifiers);
        while (std::getline(infile, vk) && std::getline(infile, program))
        {
            if (modifiers == "" || vk == "" || program == "")
                return;

            std::vector<std::string> splitModifiers = splitString(modifiers, "+");
            int modifiersParsed = 0;
            for (std::vector<std::string>::iterator it = splitModifiers.begin(); it != splitModifiers.end(); it++) {
                if (*it == "MOD_ALT")
                    modifiersParsed += 0x0001;
                else if (*it == "MOD_CONTROL")
                    modifiersParsed += 0x0002;
                else if (*it == "MOD_NOREPEAT")
                    modifiersParsed += 0x4000;
                else if (*it == "MOD_SHIFT")
                    modifiersParsed += 0x0004;
                else if (*it == "MOD_WIN")
                    modifiersParsed += 0x0008;
            }
            int vkParsed = std::stoi(vk);
            this->modifiersList.push_back(modifiersParsed);
            this->vkList.push_back(vkParsed);
            this->programList.push_back(program);

            std::getline(infile, modifiers);
            if (modifiers == "")
                std::getline(infile, modifiers);
        }
        wchar_t buffer[256];
        wsprintfW(buffer, L"%d", this->length());
    }
    catch (std::exception) {
        MessageBox(NULL, L"Unable to load config.", L"Error", MB_ICONERROR | MB_OK);
    }
}
const size_t HotKeysConfig::length() {
    return this->modifiersList.size();
}
std::tuple<int, int, std::string> HotKeysConfig::get(size_t index) {
    return std::make_tuple<>(this->modifiersList[index], this->vkList[index], this->programList[index]);
}

VOID startApp(LPCTSTR lpApplicationName)
{
    // additional information
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    // set the size of the structures
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    // start the program up
    CreateProcess(lpApplicationName, NULL, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}