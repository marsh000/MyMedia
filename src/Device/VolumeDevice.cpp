#include <include/Device/VolumeDevice.h>
#include <include/DirTraverser.h>
#include <include/ErrorLog.h>
#include <include/Helper.h>
#include <include/Media/MediaBase.h>
#include <include/Media/Song.h>

/* default c'tor */
CVolumeDevice::CVolumeDevice()
: CDevice()
{
    this->IsOK(false);
    this->SetDeviceType(DEVTYPE_UNKNOWN);
}

/* c'tor */
CVolumeDevice::CVolumeDevice(char Volume, bool IsMaster)
: CDevice()
{
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types

    // get device information
    WCHAR VolumeWChar[4] = { Volume, ':', '\\', '\0' };
    this->SetManufacturer("Unknown");
    this->SetDescription(VolumeWChar);

    unsigned long SerialNumber = 0;
    if (GetVolumeInformation(VolumeWChar, 0, 0, &SerialNumber, 0, 0, 0, 0) ) // <-- WIN32 Function Call
    {
        wxString SerialNumberStr = wxEmptyString;
        SerialNumberStr << SerialNumber;
        this->SetSerialNumber(SerialNumberStr);
    }
    else
    {
        cmsLogError("Unable to get serial number from device");
        this->SetSerialNumber("0");
    }

    if (IsMaster)
    {
        this->SetDeviceName("Master");
        this->SetDeviceType(DEVTYPE_MASTER);
        this->SetDeviceBasePath(cmsPaths::GetMasterLibraryDirPath() );
    }
    else
    {
        this->SetDeviceName(this->GetSerialNumber() );
        this->SetDeviceType(DEVTYPE_VOLUME);
        this->SetDeviceBasePath(VolumeWChar);
    }

    WCHAR VolumeGuid[50];
    GetVolumeNameForVolumeMountPoint(VolumeWChar, VolumeGuid, sizeof(VolumeGuid) ); // <-- WIN32 Function Call
    this->SetGUID(this->ParseGuid(VolumeGuid) );

    if (this->OpenDevice() )
    {
        this->IsOK(true);
    }
    else
    {
        this->IsOK(false);
    }
}

/* copy c'tor */
CVolumeDevice::CVolumeDevice(const CVolumeDevice& Other)
{
    *this = Other;
}

/* d'tor */
CVolumeDevice::~CVolumeDevice()
{

}

CVolumeDevice& CVolumeDevice::operator=(const CVolumeDevice& Other)
{
    this->IsOK(Other.IsOK() );
    this->SetDeviceName(Other.GetDeviceName() );
    this->SetManufacturer(Other.GetManufacturer() );
    this->SetDescription(Other.GetDescription() );
    this->SetDeviceBasePath(Other.GetDeviceBasePath() );
    this->SetGUID(Other.GetGUID() );
    this->SetSerialNumber(Other.GetSerialNumber() );
    return *this;
}

bool CVolumeDevice::operator==(const CVolumeDevice& rhs)
{
    return this->GetSerialNumber() == rhs.GetSerialNumber();
}

const wxString CVolumeDevice::GetGUID() const
{
    return m_GUID;
}

void CVolumeDevice::SetGUID(const wxString& x)
{
    m_GUID = x;
}

bool CVolumeDevice::AddFolder(const wxString& FolderName, wxString& ParentFolderPath)
{
    wxString Path = wxEmptyString;
    if (ParentFolderPath == wxEmptyString)
    {
        Path << this->GetDeviceBasePath();
    }
    else
    {
        Path << ParentFolderPath;
    }
    Path << FolderName << "\\";

    if (!wxDir::Make(Path, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL) )
    {
        return false;
    }
    ParentFolderPath = Path;
    return true;
}

bool CVolumeDevice::DoCloseDevice()
{
    return true;
}

bool CVolumeDevice::DoOpenDevice()
{
    return true;
}

bool CVolumeDevice::SaveMedia(CMedia* pSourceMedia, const wxString& DestPath)
{
    // check and see if the file already exists
    if (!wxFileExists(DestPath) )
    {
        if (wxCopyFile(pSourceMedia->GetFullPath(), DestPath, false) )
        {
            return true;
        }
    }
    return false;
}

CDevice* CVolumeDevice::Clone()
{
    CVolumeDevice* pNewDevice = new CVolumeDevice(*this);
    return pNewDevice;
}
/**
bool CVolumeDevice::EnumerateFiles(cmsMediaType LibType, wxString& BasePath, wxArrayString& ObjectPaths, bool Recursive)
{
    switch(LibType)
    {
    case MEDIATYPE_MUSIC:
        {
            wxDir FileSearcher(BasePath);
            if (FileSearcher.IsOpened() )
            {
                cmsDirTraverser Traverser(ObjectPaths);
                FileSearcher.Traverse(Traverser, wxEmptyString, wxDIR_FILES | wxDIR_DIRS);
            }
            else
            {
                return false;
            }
        }
        break;

    default:
        wxFAIL_MSG("Unsupported Library Type");
    }
    return true;
}
**/
/**
bool CVolumeDevice::GetMedia(CMedia& Media, const wxString& FilePath)
{
    if (Media.GetMetaDataFromFile(FilePath) )
        return true;

    return false;
}
**/
/**
bool CVolumeDevice::RemoveFolder(const wxString& FolderPath)
{
    return true;
}
**/
/**
bool CVolumeDevice::RemoveMedia(const wxString& FilePath)
{
    // check if the file exists
    if (wxFileExists(FilePath) )
    {
        // attempt to delete the file
        if (wxRemoveFile(FilePath) )
        {
            return true;
        }
    }
    return false;
}
**/

wxString CVolumeDevice::ParseGuid(wxString Guid)
{
    wxString Str = Guid.AfterFirst('{');
    Str = Str.BeforeLast('}');
    return Str;
}

