#ifndef VOLUMEDEVICE_H_INCLUDED
#define VOLUMEDEVICE_H_INCLUDED

#include <include/wxMSWHeaders.h>
#include <include/Device/DeviceBase.h>

class CMedia;
class wxArrayString;

class CVolumeDevice : public CDevice
{
public:
    CVolumeDevice();
    CVolumeDevice(char Volume, bool IsMaster = false);
    CVolumeDevice(const CVolumeDevice& Other);
    ~CVolumeDevice();
    CVolumeDevice& operator=(const CVolumeDevice& Other);
    bool operator==(const CVolumeDevice& rhs);
    const wxString GetGUID() const;
    void SetGUID(const wxString& x);

    // inherited virtual methods
    bool DoCloseDevice();
    bool DoOpenDevice();
    bool AddFolder(const wxString& FolderName, wxString& ParentFolderPath);
    bool SaveMedia(CMedia* pSourceMedia, const wxString& DestPath);
    CDevice* Clone();
///    bool EnumerateFiles(cmsMediaType LibType, wxString& BasePath, wxArrayString& ObjectPaths, bool Recursive);
///    bool GetMedia(CMedia& Media, const wxString& FilePath);
///    bool RemoveFolder(const wxString& FolderPath);
///    bool RemoveMedia(const wxString& FilePath);

private:
    wxString ParseGuid(wxString Guid);

    wxString                 m_GUID;
};

#endif // VOLUMEDEVICE_H_INCLUDED
