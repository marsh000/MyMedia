#include <include/Library/LibraryBase.h>
#include <include/Device/DeviceBase.h>

CLibrary::CLibrary(wxWindow* pParent, wxWindowID ID)
: wxPanel(pParent, ID),
  m_MediaType(MEDIATYPE_UNKNOWN), m_pDevice(NULL), m_ItemCount(0)
{
    m_pTreelistCtrl = new wxTreeListCtrl(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTL_SINGLE | wxTL_3STATE);
    wxBoxSizer* pSizer = new wxBoxSizer(wxVERTICAL);
    pSizer->Add(m_pTreelistCtrl, wxSizerFlags(1).Border(wxALL, 0).Expand() );
    this->SetSizerAndFit(pSizer);
}

CLibrary::~CLibrary()
{
    m_pDevice = NULL;
}

CDevice* CLibrary::GetDevice() const
{
    return m_pDevice;
}

int CLibrary::GetItemCount() const
{
    return m_ItemCount;
}

wxString CLibrary::GetLibraryPath() const
{
    return m_LibraryPath;
}

wxString CLibrary::GetLibraryName() const
{
    return m_LibraryName;
}

cmsMediaType CLibrary::GetMediaType() const
{
    return m_MediaType;
}

void CLibrary::SetDevice(CDevice* pDevice)
{
    m_pDevice = pDevice;
}

void CLibrary::SetLibraryPath(const wxString& Path)
{
    m_LibraryPath = Path;
}

void CLibrary::SetMediaType(cmsMediaType Type)
{
    m_MediaType = Type;
}
