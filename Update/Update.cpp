// Update.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "Update.h"
#include "Message.h"

#define WS_UPDATEWINDOW (WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_CLIPCHILDREN)

// Global Variables:
HINSTANCE hInst;								// current instance

CRITICAL_SECTION g_ProgressCriticalSection;
bool g_bUpdateProgress = false;
int g_ProgressTotal = 100;
int g_ProgressCurrent = 0;
TCHAR g_szCurrentFile[256] = {0};

HFONT hFont;
HICON hLogoIcon;

HWND hMessageLabel;
HWND hProgressBar;

int iDPIX;
int iDPIY;

// Forward declarations of functions included in this code module:
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);
int _cdecl Main(HWND hMessage, HWND hProgress);

#define GDPIX(coord) (coord*iDPIX/96)
#define GDPIY(coord) (coord*iDPIY/96)

#define MUTEX_NAME "GTAC-Updater-{45640430-19E1-48cf-B658-0ADDC33BBD3F}"

void WaitForUpdater(void)
{
	HANDLE hMutex;
Retry:
	hMutex = CreateMutex(NULL, FALSE, TEXT(MUTEX_NAME));
	if ((ERROR_ALREADY_EXISTS == GetLastError()))
	{
		Sleep(10);

		if (hMutex != NULL)
			CloseHandle(hMutex);

		goto Retry;
	}

	if (hMutex != NULL)
		CloseHandle(hMutex);
}

//
//  FUNCTION: MyRegisterClass()
//
//  PURPOSE: Registers the window class.
//
//  COMMENTS:
//
//    This function and its usage are only necessary if you want this code
//    to be compatible with Win32 systems prior to the 'RegisterClassEx'
//    function that was added to Windows 95. It is important to call this function
//    so that the application will get 'well formed' small icons associated
//    with it.
//
ATOM MyRegisterClass(HINSTANCE hInstance, HICON hLargeIcon, HICON hSmallIcon)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= hLargeIcon;
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_BTNFACE+1);
	wcex.lpszMenuName	= nullptr;
	wcex.lpszClassName	= _T("UpdateWindow");
	wcex.hIconSm		= hSmallIcon;

	return RegisterClassEx(&wcex);
}

//
//   FUNCTION: InitInstance(HINSTANCE, int)
//
//   PURPOSE: Saves instance handle and creates main window
//
//   COMMENTS:
//
//        In this function, we save the instance handle in a global variable and
//        create and display the main program window.
//
HWND InitInstance(HINSTANCE hInstance, ATOM ClassAtom)
{
	hInst = hInstance; // Store instance handle in our global variable

	RECT rcRect;
	RECT rcWorkingArea;
	rcRect.left = 0;
	rcRect.top = 0;
	rcRect.right = (LONG)GDPIX(500);
	rcRect.bottom = (LONG)GDPIY(120);
	AdjustWindowRect(&rcRect,WS_UPDATEWINDOW,FALSE);
	SystemParametersInfo(SPI_GETWORKAREA, 0, &rcWorkingArea, 0);

	HWND hWnd = CreateWindowEx(WS_EX_DLGMODALFRAME|WS_EX_TOPMOST, (LPCTSTR)ClassAtom, _T("Update"), WS_UPDATEWINDOW, (rcWorkingArea.left+rcWorkingArea.right)/2-(rcRect.right-rcRect.left)/2, (rcWorkingArea.top+rcWorkingArea.bottom)/2-(rcRect.bottom-rcRect.top)/2, rcRect.right-rcRect.left, rcRect.bottom-rcRect.top, NULL, NULL, hInstance, NULL);

	if (hWnd == nullptr)
	{
		return nullptr;
	}

	ShowWindow(hWnd, SW_SHOW);
	UpdateWindow(hWnd);

	return hWnd;
}

extern FILE* g_pInstallerLog;

bool Delete(const TCHAR* pszFile)
{
	bool bDeleted = DeleteFile(pszFile)!=FALSE;
	_ftprintf_s(g_pInstallerLog, _T("Deleting %s - %s\n"), pszFile, bDeleted?_T("OK"):_T("ERROR"));
	return bDeleted;
}

DWORD WINAPI UpdateThread(LPVOID lpThreadParameter)
{
	g_pInstallerLog = nullptr;
	_tfopen_s(&g_pInstallerLog, _T("update.log"), _T("wt"));

	int Result = Main(hMessageLabel, hProgressBar);
	if (Result == 0)
	{
		if (PathFileExists(_T("removed.txt")))
		{
			_fputts(_T("\n"), g_pInstallerLog);

			FILE* pRemovedFile = nullptr;
			_tfopen_s(&pRemovedFile, _T("removed.txt"), _T("rt"));
			if (pRemovedFile != nullptr)
			{
				TCHAR szLine[1024];
				while (_fgetts(szLine, ARRAYSIZE(szLine), pRemovedFile))
				{
					if (PathFileExists(szLine))
					{
						if (!Delete(szLine))
							Result = EXIT_FAILURE;
					}
				}
				fclose(pRemovedFile);
			}

			Delete(_T("removed.txt"));
		}

		Delete(_T("update.7z"));
	}

	if (g_pInstallerLog != nullptr)
		fclose(g_pInstallerLog);

	if (Result == 0)
		Sleep(1000);

	return Result;
}

// The compiler wants to repeat this function for some reason...
__declspec(noinline) bool OpenLauncher(void)
{
	// Remove any temp files...
	if (PathFileExists(_T("update.xml")))
		DeleteFile(_T("update.xml"));
	if (PathFileExists(_T("removed.txt")))
		DeleteFile(_T("removed.txt"));

	if ((int)ShellExecute(NULL, _T("open"), _T("Launcher.exe"), NULL, NULL, SW_SHOW) > 32)
		return true;

	// If we cannot open the launcher executable, let's direct the user to the website...
	//ShellExecute(NULL, _T("open"), _T("https://gtaconnected.com/downloads/"), NULL, NULL, SW_SHOW);

	return false;
}

// Show the window for testing purposes
//#define WINDOW_TEST

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	WaitForUpdater();

#ifndef WINDOW_TEST
	if (!PathFileExists(_T("update.7z")))
	{
		OpenLauncher();
		return EXIT_SUCCESS;
	}
#endif

	MSG msg;

	INITCOMMONCONTROLSEX icc;
	icc.dwSize = sizeof(icc);
	icc.dwICC = ICC_PROGRESS_CLASS|ICC_STANDARD_CLASSES;
	InitCommonControlsEx(&icc);

	HICON hLargeIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_UPDATE), IMAGE_ICON, GetSystemMetrics(SM_CXICON), GetSystemMetrics(SM_CYICON), LR_DEFAULTCOLOR);
	HICON hSmallIcon = (HICON)LoadImage(hInstance, MAKEINTRESOURCE(IDI_UPDATE), IMAGE_ICON, GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), LR_DEFAULTCOLOR);

	hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);

	HDC hDC = GetDC(NULL);
	iDPIX = GetDeviceCaps(hDC, LOGPIXELSX);
	iDPIY = GetDeviceCaps(hDC, LOGPIXELSY);
	ReleaseDC(NULL,hDC);

	ATOM ClassAtom = MyRegisterClass(hInstance, hLargeIcon, hSmallIcon);
	if (ClassAtom == 0)
		return EXIT_FAILURE;

	HWND hWnd = InitInstance(hInstance, ClassAtom);
	if (hWnd == nullptr)
		return EXIT_FAILURE;

	bool bQuit = false;
	int iResult;

	InitializeCriticalSectionAndSpinCount(&g_ProgressCriticalSection, 2000);

#ifndef WINDOW_TEST
	HANDLE hThread = CreateThread(nullptr, 0, &UpdateThread, nullptr, 0, nullptr);
#endif

	while (!bQuit)
	{
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE) != FALSE)
		{
			if (msg.message == WM_QUIT)
			{
				iResult = (int)msg.wParam;
				bQuit = true;
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

#ifndef WINDOW_TEST
		if (WaitForSingleObject(hThread, 0) == WAIT_OBJECT_0)
		{
			DWORD dwExitCode = EXIT_FAILURE;
			GetExitCodeThread(hThread, &dwExitCode);
			if (dwExitCode == 0)
				DestroyWindow(hWnd);
			else
			{
				if (Message::Show(_T("The update failed, would you like to try again?\n\nSee update.log for more information."), _T("Update"), Message::Buttons::RetryCancel, Message::Icons::Error, hWnd) == Message::Results::Retry)
				{
					CloseHandle(hThread);

					hThread = CreateThread(nullptr, 0, &UpdateThread, nullptr, 0, nullptr);
				}
				else
				{
					DestroyWindow(hWnd);
				}
			}
		}
#endif

		if (TryEnterCriticalSection(&g_ProgressCriticalSection))
		{
			if (g_bUpdateProgress)
			{
				SendMessage(hProgressBar,PBM_SETRANGE,0,MAKELPARAM(0,g_ProgressTotal));
				SendMessage(hProgressBar,PBM_SETPOS,g_ProgressCurrent,0);
				SetWindowText(hMessageLabel, g_szCurrentFile);
				g_bUpdateProgress = false;
			}
			LeaveCriticalSection(&g_ProgressCriticalSection);
		}

		Sleep(1);
	}

#ifndef WINDOW_TEST
	CloseHandle(hThread);
#endif

	DeleteCriticalSection(&g_ProgressCriticalSection);

	if (hLargeIcon != nullptr)
		DestroyIcon(hLargeIcon);
	if (hSmallIcon != nullptr)
		DestroyIcon(hSmallIcon);
	if (hLogoIcon != nullptr)
		DestroyIcon(hLogoIcon);

	UnregisterClass((LPCTSTR)ClassAtom, hInstance);

	OpenLauncher();

	return (int) msg.wParam;
}

//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE:  Processes messages for the main window.
//
//  WM_COMMAND	- process the application menu
//  WM_PAINT	- Paint the main window
//  WM_DESTROY	- post a quit message and return
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
		{
#ifndef WINDOW_TEST
			EnableMenuItem(GetSystemMenu(hWnd, FALSE), SC_CLOSE, MF_BYCOMMAND|MF_GRAYED);
#endif

			unsigned int uiWidth = GDPIX(500);
			unsigned int uiHeight = GDPIY(120);
			unsigned int uiPadding = GDPIX(10);
			unsigned int uiProgressHeight = GDPIY(25);
			unsigned int uiLabelHeight = GDPIY(13);
			unsigned int uiIconSize = GDPIX(32);

			hLogoIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_7Z), IMAGE_ICON, GDPIX(32), GDPIY(32), LR_DEFAULTCOLOR);

			HWND hStaticIcon2 = CreateWindow(WC_STATIC, nullptr, WS_CHILD | WS_VISIBLE | SS_ICON | SS_REALSIZECONTROL, uiPadding, uiPadding, uiIconSize, uiIconSize, hWnd, NULL, hInst, NULL);
			SendMessage(hStaticIcon2, STM_SETIMAGE, (WPARAM)IMAGE_ICON, (LPARAM)hLogoIcon);

			hMessageLabel = CreateWindow(WC_STATIC,_T("Updating..."),WS_CHILD | WS_VISIBLE | SS_LEFT, uiPadding, uiHeight-uiProgressHeight-(uiPadding*2)-uiLabelHeight, uiWidth-(uiPadding*2), uiLabelHeight,hWnd,NULL,hInst,NULL);
			SendMessage(hMessageLabel, WM_SETFONT, (WPARAM)hFont, TRUE);

			hProgressBar = CreateWindow(PROGRESS_CLASS, NULL, WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | WS_TABSTOP, uiPadding, uiHeight-uiProgressHeight-uiPadding, uiWidth-(uiPadding*2), uiProgressHeight, hWnd, NULL, hInst, NULL);

			SendMessage(hProgressBar, PBM_SETRANGE, 0, MAKELPARAM(0,100));
			SendMessage(hProgressBar, PBM_SETPOS, 0, 0);
		}
		break;
#ifndef WINDOW_TEST
	case WM_CLOSE:
		{
			return 0;
		}
		break;
#endif
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
