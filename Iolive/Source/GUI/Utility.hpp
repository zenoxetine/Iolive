#pragma once

#include <string>

namespace Utility {

    std::wstring WOpenFileDialog(const wchar_t* filter = L"All Files (*.*)\0*.*\0");

}