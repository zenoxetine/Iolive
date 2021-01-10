#pragma once

#include <windows.h>
#include <shellapi.h>
#include <string>

namespace WindowsAPI {
	/*
	* Get the horizontal and vertical screen sizes in pixel
	*/
	static void GetDesktopResolution(int* horizontal, int* vertical)
	{
		RECT desktop;
		const HWND hDesktop = GetDesktopWindow();
		GetWindowRect(hDesktop, &desktop);

		*horizontal = desktop.right;
		*vertical = desktop.bottom;
	}

	/*
	* Get mouse x and y position
	*/
	static bool GetMousePosition(int* x, int* y)
	{
		POINT mousePoint;
		if (GetCursorPos(&mousePoint))
		{
			*x = mousePoint.x;
			*y = mousePoint.y;
			return true;
		}
		else
		{
			return false;
		}
	}

	/*
	* Open Windows file dialog
	* \return absolute file path, and return empty wstring if dialog got canceled
	*/
	static std::wstring WOpenFileDialog(const wchar_t* filter = L"All Files (*.*)\0*.*\0", HWND owner = NULL)
	{
		OPENFILENAMEW ofn;
		ZeroMemory(&ofn, sizeof(ofn));

		WCHAR filePath[MAX_PATH] = L"";

		ofn.lStructSize = sizeof(OPENFILENAME);
		ofn.hwndOwner = owner;
		ofn.lpstrFilter = filter;
		ofn.lpstrFile = filePath;
		ofn.nMaxFile = MAX_PATH;
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
		ofn.lpstrDefExt = L"";

		std::wstring filePathWStr;

		if (GetOpenFileNameW(&ofn))
		{
			filePathWStr = filePath;
		}

		return filePathWStr;
	}

	static void OpenUrlInBrowser(const char* url)
	{
		ShellExecute(0, 0, url, 0, 0, SW_SHOW);
	}
} // namespace WindowsAPI