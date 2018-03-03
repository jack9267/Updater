#pragma once

enum _TASKDIALOG_COMMON_BUTTON_FLAGS
{
	TDCBF_OK_BUTTON = 0x0001, // selected control return value IDOK
	TDCBF_YES_BUTTON = 0x0002, // selected control return value IDYES
	TDCBF_NO_BUTTON = 0x0004, // selected control return value IDNO
	TDCBF_CANCEL_BUTTON = 0x0008, // selected control return value IDCANCEL
	TDCBF_RETRY_BUTTON = 0x0010, // selected control return value IDRETRY
	TDCBF_CLOSE_BUTTON = 0x0020  // selected control return value IDCLOSE
};
typedef int TASKDIALOG_COMMON_BUTTON_FLAGS;           // Note: _TASKDIALOG_COMMON_BUTTON_FLAGS is an int