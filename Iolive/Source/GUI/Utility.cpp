#include <windows.h>
#include "Utility.hpp"

namespace Utility {
    std::wstring WOpenFileDialog(const wchar_t* filter, HWND owner)
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
} // namespace Utility