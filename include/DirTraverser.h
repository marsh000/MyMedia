#ifndef DIRTRAVERSER_H_INCLUDED
#define DIRTRAVERSER_H_INCLUDED

#include <include/wxMSWHeaders.h>

class cmsDirTraverser : public wxDirTraverser
{
public:
    cmsDirTraverser(wxArrayString& Files, bool iTunes = false);
    ~cmsDirTraverser();
    virtual wxDirTraverseResult OnFile(const wxString& Filename);
    virtual wxDirTraverseResult OnDir(const wxString& DirName);

private:
    wxArrayString*      m_pFiles;
    bool                m_iTunes;
};


#endif // DIRTRAVERSER_H_INCLUDED
