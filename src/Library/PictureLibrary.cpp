#include <include/Library/PictureLibrary.h>
#include <include/Device/DeviceBase.h>

CPictureLibrary::CPictureLibrary(wxWindow* pParent, wxWindowID ID, cmsDatabase* pDatabase, CDevice* pDevice)
: CLibrary(pParent, ID)
{
    wxASSERT_MSG(pDevice, "pDevice must point to a valid Device");

    m_LibraryName = "Pictures";
    m_pDatabase = pDatabase;
    m_pDevice = pDevice;
    m_MediaType = MEDIATYPE_PICTURE;
    wxString Path = pDevice->GetDeviceBasePath();
    if (pDevice->GetDeviceType() == DEVTYPE_MASTER)
    {
        Path << "My Pictures\\MyMedia\\";
    }
    this->SetLibraryPath(Path);

    // set up columns
}

CPictureLibrary::~CPictureLibrary()
{

}

bool CPictureLibrary::InitDatabase()
{

    return true;
}

bool CPictureLibrary::LoadLibraryFromDatabase()
{

    return true;
}

bool CPictureLibrary::InsertMediaInList(CMedia* pMedia)
{

    return true;
}

bool CPictureLibrary::DeleteMedia(CMedia* pMedia)
{

    return true;
}

bool CPictureLibrary::GetMedia(int Id, CMedia* pMedia)
{

    return true;
}

bool CPictureLibrary::SaveMedia(CMedia* pMedia)
{

    return true;
}
