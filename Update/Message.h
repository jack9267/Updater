#pragma once

#include "TaskDialogIcons.h"
#include "TaskDialogCommonButtonFlags.h"

namespace Message
{
	typedef int Result;

	namespace Results
	{
		const Result OK = IDOK;
		const Result Cancel = IDCANCEL;
		const Result Retry = IDRETRY;
		const Result Yes = IDYES;
		const Result No = IDNO;
	}

	extern Result Show(const wchar_t* pszContent, const wchar_t* pszTitle, UINT uiType, DWORD dwCommonButtons, Result CancelResult, PCWSTR pszMainIcon, HWND hWnd = nullptr);
	extern Result Show(const char* pszContent, const char* pszTitle, UINT uiType, DWORD dwCommonButtons, Result CancelResult, PCWSTR pszMainIcon, HWND hWnd = nullptr);

	struct tButtons
	{
		UINT uiType;
		DWORD dwCommonButtons;
		Result CancelResult;
	};

	namespace Buttons
	{
		const tButtons OK = { MB_OK , TDCBF_OK_BUTTON, Results::OK };
		const tButtons OKCancel = { MB_OKCANCEL , TDCBF_OK_BUTTON | TDCBF_CANCEL_BUTTON, Results::Cancel };
		const tButtons YesNoCancel = { MB_YESNOCANCEL , TDCBF_YES_BUTTON | TDCBF_NO_BUTTON | TDCBF_CANCEL_BUTTON, Results::Cancel };
		const tButtons YesNo = { MB_YESNO, TDCBF_YES_BUTTON | TDCBF_NO_BUTTON, Results::No };
		const tButtons RetryCancel = { MB_RETRYCANCEL, TDCBF_RETRY_BUTTON | TDCBF_CANCEL_BUTTON, Results::Cancel };
	}

	struct tIcon
	{
		UINT uiType;
		PCWSTR pszMainIcon;
	};

	namespace Icons
	{
		const tIcon None = { 0, nullptr };
		const tIcon Warning = { MB_ICONWARNING, TD_WARNING_ICON };
		const tIcon Error = { MB_ICONERROR , TD_ERROR_ICON };
		const tIcon Information = { MB_ICONINFORMATION , TD_INFORMATION_ICON };
	}

	template<typename T> inline Result Show(const T* pszContent, const T* pszTitle, const tButtons& Buttons, const tIcon& Icon, HWND hWnd = nullptr)
	{
		return Show(pszContent, pszTitle, Buttons.uiType | Icon.uiType, Buttons.dwCommonButtons, Buttons.CancelResult, Icon.pszMainIcon, hWnd);
	}

	template<typename T> inline void Alert(const T* pszContent, const T* pszTitle, HWND hWnd = nullptr)
	{
		Message::Show(pszContent, pszTitle, Message::Buttons::OK, Message::Icons::Information, hWnd);
	}

	template<typename T> inline bool Confirm(const T* pszContent, const T* pszTitle, HWND hWnd = nullptr)
	{
		return Message::Show(pszContent, pszTitle, Message::Buttons::OKCancel, Message::Icons::Information, hWnd) == IDOK;
	}
}