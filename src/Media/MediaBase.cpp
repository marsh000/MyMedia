#include <include/Media/MediaBase.h>

cmsFileType CMedia::GetFileType(const wxString& Path)
{
    wxString ext = Path.AfterLast('.');
    if (ext == "m4a" || ext == "M4A" || ext == "mp4" || ext == "MP4" || ext == "aac" || ext == "AAC")
        return FILETYPE_M4A;
    else if (ext == "mp3")
        return FILETYPE_MP3;
    else
        return FILETYPE_UNKNOWN;
}

wxString CMedia::GetFileTypeStr(const wxString& Path)
{
    wxString ext = Path.AfterLast('.');
    if (ext == "m4a" || ext == "M4A" || ext == "mp4" || ext == "MP4" || ext == "aac" || ext == "AAC")
        return "m4a";
    else if (ext == "mp3" || ext == "MP3")
        return "mp3";
    else
        return wxEmptyString;
}

