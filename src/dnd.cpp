#include <include/dnd.h>

CFileDropTarget::CFileDropTarget(wxTreeCtrl* pOwner)
{
    m_pOwner = pOwner;
}

CFileDropTarget::~CFileDropTarget()
{

}

bool CFileDropTarget::OnDropFiles(wxCoord XCoord, wxCoord YCoord, const wxArrayString &Filenames)
{

    return true;
}
