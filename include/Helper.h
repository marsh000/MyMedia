#ifndef HELPER_H_INCLUDED
#define HELPER_H_INCLUDED

#include <include/wxMSWHeaders.h>

namespace cms
{
    wxString sqlEscapeQuotes(const wxString& OriginalStr);
    wxString ReplaceIllegalPathCharsWithUnderscore(wxString& OriginalStr);
    int GetIntFromBool(bool val);
    bool GetBoolFromInt(int val);
}

#endif // HELPER_H_INCLUDED
