#include "stdafx.h"
#include "TaskDialog.h"
#include <assert.h>

typedef HRESULT(FAR WINAPI *TASKDIALOGINDIRECTPROC)(_In_ const TASKDIALOGCONFIG *pTaskConfig, _Out_opt_ int *pnButton, _Out_opt_ int *pnRadioButton, _Out_opt_ BOOL *pfVerificationFlagChecked);

bool TaskDialogIndirect(const TASKDIALOGCONFIG *pTaskConfig, int *pnButton, int *pnRadioButton, BOOL *pfVerificationFlagChecked, HRESULT* phr)
{
	HMODULE hComctl32 = LoadLibrary(TEXT("Comctl32.dll"));
	if (hComctl32 == nullptr)
	{
#ifdef _DEBUG
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
#endif
		assert(0);
		return false;
	}
	HRESULT hr;
	__try
	{
		TASKDIALOGINDIRECTPROC pfnTaskDialogIndirect = (TASKDIALOGINDIRECTPROC)GetProcAddress(hComctl32, "TaskDialogIndirect");
		if (pfnTaskDialogIndirect == nullptr)
		{
			DWORD dw = GetLastError();
			if (dw != ERROR_PROC_NOT_FOUND)
			{
#ifdef _DEBUG
				HRESULT hr = HRESULT_FROM_WIN32(dw);
#endif
				assert(0);
			}
			return false;
		}
		hr = pfnTaskDialogIndirect(pTaskConfig, pnButton, pnRadioButton, pfVerificationFlagChecked);
	}
	__finally
	{
		if (!FreeLibrary(hComctl32))
		{
#ifdef _DEBUG
			HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
#endif
			assert(0);
		}
	}
	*phr = hr;
	return true;
}

typedef HRESULT(FAR WINAPI *TASKDIALOGPROC)(_In_opt_ HWND hwndOwner, _In_opt_ HINSTANCE hInstance, _In_opt_ PCWSTR pszWindowTitle, _In_opt_ PCWSTR pszMainInstruction, _In_opt_ PCWSTR pszContent, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons, _In_opt_ PCWSTR pszIcon, _Out_opt_ int *pnButton);

bool TaskDialog(HWND hwndOwner, HINSTANCE hInstance, PCWSTR pszWindowTitle, PCWSTR pszMainInstruction, PCWSTR pszContent, TASKDIALOG_COMMON_BUTTON_FLAGS dwCommonButtons, PCWSTR pszIcon, int *pnButton, HRESULT* phr)
{
	HMODULE hComctl32 = LoadLibrary(TEXT("Comctl32.dll"));
	if (hComctl32 == nullptr)
	{
#ifdef _DEBUG
		HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
#endif
		assert(0);
		return false;
	}
	HRESULT hr;
	__try
	{
		TASKDIALOGPROC pfnTaskDialog = (TASKDIALOGPROC)GetProcAddress(hComctl32, "TaskDialog");
		if (pfnTaskDialog == nullptr)
		{
			DWORD dw = GetLastError();
			if (dw != ERROR_PROC_NOT_FOUND)
			{
#ifdef _DEBUG
				HRESULT hr = HRESULT_FROM_WIN32(dw);
#endif
				assert(0);
			}
			return false;
		}
		hr = pfnTaskDialog(hwndOwner, hInstance, pszWindowTitle, pszMainInstruction, pszContent, dwCommonButtons, pszIcon, pnButton);
	}
	__finally
	{
		if (!FreeLibrary(hComctl32))
		{
#ifdef _DEBUG
			HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
#endif
			assert(0);
		}
	}
	*phr = hr;
	return true;
}