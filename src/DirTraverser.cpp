#include <include/DirTraverser.h>
#include <include/Media/MediaBase.h>

cmsDirTraverser::cmsDirTraverser(wxArrayString& Files, bool iTunes)
: m_pFiles(&Files), m_iTunes(iTunes)
{

}

cmsDirTraverser::~cmsDirTraverser()
{

}

wxDirTraverseResult cmsDirTraverser::OnFile(const wxString& Filename)
{
    wxString Type = CMedia::GetFileTypeStr(Filename);
    if (Type == "m4a" || Type == "m4p" || Type == "aac" || Type == "mp3")
    {
        m_pFiles->Add(Filename);
    }
    return wxDIR_CONTINUE;
}

wxDirTraverseResult cmsDirTraverser::OnDir(const wxString& DirName)
{
    if (!m_iTunes)
    {
        wxString x = wxStandardPaths::Get().GetDocumentsDir() + "My Music\\iTunes";
        if (DirName == x)
        {
            return wxDIR_IGNORE;
        }
    }
    return wxDIR_CONTINUE;
}
