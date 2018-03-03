#include "stdafx.h"
#include "Message.h"
#include <assert.h>
#include "TaskDialog.h"

Message::Result Message::Show(const wchar_t* pszContent, const wchar_t* pszTitle, UINT uiType, DWORD dwCommonButtons, Result CancelResult, PCWSTR pszMainIcon, HWND hWnd)
{
	TASKDIALOGCONFIG TaskConfig;
	TaskConfig.cbSize = sizeof(TASKDIALOGCONFIG);
	TaskConfig.hwndParent = hWnd;
	TaskConfig.hInstance = nullptr;
	TaskConfig.dwFlags = TDF_SIZE_TO_CONTENT;
	TaskConfig.dwCommonButtons = dwCommonButtons;
	TaskConfig.pszWindowTitle = pszTitle;
	TaskConfig.pszMainIcon = pszMainIcon;
	TaskConfig.pszMainInstruction = nullptr;
	TaskConfig.pszContent = pszContent;
	TaskConfig.cButtons = 0;
	TaskConfig.nDefaultButton = 0;
	TaskConfig.cRadioButtons = 0;
	TaskConfig.pszVerificationText = nullptr;
	TaskConfig.pszExpandedInformation = nullptr;
	TaskConfig.pszExpandedControlText = nullptr;
	TaskConfig.pszCollapsedControlText = nullptr;
	TaskConfig.pszFooterIcon = nullptr;
	TaskConfig.pszFooter = nullptr;
	TaskConfig.pfCallback = nullptr;
	TaskConfig.cxWidth = 0;
	int iButton;
	HRESULT hr;
	if (TaskDialogIndirect(&TaskConfig, &iButton, nullptr, nullptr, &hr))
	{
		if (SUCCEEDED(hr))
			return iButton;
		else
			assert(0);
	}
	int iResult = MessageBoxW(hWnd, pszContent, pszTitle, uiType);
	if (iResult == 0)
	{
#ifdef _DEBUG
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
#endif
		assert(0);
	}
	else
	{
		return iResult;
	}
	return CancelResult;
}

Message::Result Message::Show(const char* pszContent, const char* pszTitle, UINT uiType, DWORD dwCommonButtons, Result CancelResult, PCWSTR pszMainIcon, HWND hWnd)
{
	wchar_t* pszContent2;
	if (pszContent != nullptr)
	{
		int iLength = MultiByteToWideChar(CP_UTF8, 0, pszContent, -1, nullptr, 0);
		if (iLength == 0)
		{
#ifdef _DEBUG
			HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
#endif
			assert(0);
			return CancelResult;
		}
		pszContent2 = new wchar_t[iLength];
		if (MultiByteToWideChar(CP_UTF8, 0, pszContent, -1, pszContent2, iLength) == 0)
		{
#ifdef _DEBUG
			HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
#endif
			assert(0);
			delete[] pszContent2;
			return CancelResult;
		}
	}
	else
	{
		pszContent2 = nullptr;
	}
	Result Result;
	__try
	{
		wchar_t* pszTitle2;
		if (pszTitle != nullptr)
		{
			int iLength = MultiByteToWideChar(CP_UTF8, 0, pszTitle, -1, nullptr, 0);
			if (iLength == 0)
			{
#ifdef _DEBUG
				HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
#endif
				assert(0);
				return CancelResult;
			}
			pszTitle2 = new wchar_t[iLength];
			if (MultiByteToWideChar(CP_UTF8, 0, pszTitle, -1, pszTitle2, iLength) == 0)
			{
#ifdef _DEBUG
				HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
#endif
				assert(0);
				delete[] pszTitle2;
				return CancelResult;
			}
		}
		else
		{
			pszTitle2 = nullptr;
		}
		__try
		{
			Result = Show(pszContent2, pszTitle2, uiType, dwCommonButtons, CancelResult, pszMainIcon, hWnd);
		}
		__finally
		{
			if (pszTitle2 != nullptr)
				delete[] pszTitle2;
		}
	}
	__finally
	{
		if (pszContent2 != nullptr)
			delete[] pszContent2;
	}
	return Result;
}