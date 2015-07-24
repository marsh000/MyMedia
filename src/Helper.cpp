#include <include/Helper.h>

namespace cms
{
    wxString sqlEscapeQuotes(const wxString& OriginalStr)
    {
        wxString NewStr = OriginalStr;
        NewStr.Replace("'", "''");
        return NewStr;
    }

    wxString ReplaceIllegalPathCharsWithUnderscore(wxString& OriginalStr)
    {
        const char IllegalChars[] = "<>:\"/\\|?*.";
        const int NumIllegalChars = (sizeof(IllegalChars) * sizeof(char) ) - 1;
        wxString NewStr = OriginalStr;
        for (int x = 0; x < NumIllegalChars; x++)
        {
            NewStr.Replace(IllegalChars[x], "_");
        }
        return NewStr;
    }

    int GetIntFromBool(bool val)
    {
        return (val) ? 1 : 0;
    }

    bool GetBoolFromInt(int val)
    {
        return (val > 0) ? true : false;
    }
}
