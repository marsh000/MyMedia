#ifndef DND_H_INCLUDED
#define DND_H_INCLUDED

#include <include/wxMSWHeaders.h>

class CFileDropTarget : public wxFileDropTarget
{
public:
    CFileDropTarget(wxTreeCtrl* pOwner);
    ~CFileDropTarget();
    bool OnDropFiles(wxCoord XCoord, wxCoord YCoord, const wxArrayString &Filenames);

private:
    wxTreeCtrl*              m_pOwner;
};

#endif // DND_H_INCLUDED
