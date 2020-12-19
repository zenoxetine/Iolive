#include "Utility.hpp"
#include <windows.h>

namespace Utility {
    std::wstring WOpenFileDialog(const wchar_t* filter)
    {
        OPENFILENAMEW ofn;
        ZeroMemory(&ofn, sizeof(ofn));
        
        WCHAR filePath[MAX_PATH] = L"";

        ofn.lStructSize = sizeof(OPENFILENAME);
        ofn.hwndOwner = NULL;
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
}