#pragma once

#include <windef.h>
#include <string>

namespace Utility {

    std::wstring WOpenFileDialog(const wchar_t* filter = L"All Files (*.*)\0*.*\0", HWND owner = NULL);

} // namespace Utility