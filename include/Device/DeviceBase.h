#ifndef DEVICEBASE_H_INCLUDED
#define DEVICEBASE_H_INCLUDED

#include <include/Media/MediaBase.h>

enum cmsDeviceType
{
    DEVTYPE_UNKNOWN     = 0x0000,
    DEVTYPE_MASTER      = 0x0001,
    DEVTYPE_VOLUME      = 0x0002,
    DEVTYPE_PNP_PARENT  = 0x0004,
    DEVTYPE_PNP_CHILD   = 0x0008,
    DEVTYPE_CD          = 0x0010,
    DEVTYPE_DVD         = 0x0020,
    DEVTYPE_CAMERA      = 0x0040,
    DEVTYPE_PHONE       = 0x0080,
    DEVTYPE_TABLET      = 0x0100,
    DEVTYPR_SDCARD      = 0x0200
};

class cmsDatabase;

class CDevice
{
public:
    CDevice();
    virtual ~CDevice();

    virtual bool DoCloseDevice() = 0;
    virtual bool DoOpenDevice() = 0;
    virtual bool AddFolder(const wxString& FolderName, wxString& ParentFolderPath) = 0;
    virtual bool SaveMedia(CMedia* pSourceMedia, const wxString& DestPath) = 0;
    virtual CDevice* Clone() = 0;
///    virtual bool EnumerateFiles(cmsMediaType LibType, wxString& BasePath, wxArrayString& ObjectPaths, bool Recursive) = 0;
///    virtual bool GetMedia(CMedia& Media, const wxString& FilePath) = 0;
///    virtual bool RemoveFolder(const wxString& FolderPath) = 0;
///    virtual bool RemoveMedia(const wxString& FilePath) = 0;

    bool CloseDevice();
    bool OpenDevice();

    const wxString GetDescription() const;
    cmsDeviceType GetDeviceType() const;
    const wxString GetManufacturer() const;
    const wxString GetDeviceName() const;
    const wxString GetDeviceBasePath() const;
    const wxString GetSerialNumber() const;
    cmsDatabase* GetDatabase() const;

    void IsOK(const bool OK);
    const bool IsOK() const;

    void SetDescription(const wxString& x);
    void SetDeviceType(const cmsDeviceType x);
    void SetManufacturer(const wxString& x);
    void SetDeviceName(const wxString& x);
    void SetDeviceBasePath(const wxString& x);
    void SetSerialNumber(const wxString& x);
    void SetDatabase(cmsDatabase* pDatabase);

private:
    bool                     m_IsOK;
    wxString                 m_DeviceName;
    wxString                 m_Manufacturer;
    wxString                 m_Description;
    cmsDeviceType            m_DeviceType;
    wxString                 m_SerialNumber;
    wxString                 m_DeviceBasePath;
};

#endif // DEVICEBASE_H_INCLUDED
