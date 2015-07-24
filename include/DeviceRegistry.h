#ifndef DEVICEREGISTRY_H_INCLUDED
#define DEVICEREGISTRY_H_INCLUDED

#include <include/wxMSWHeaders.h>
#include <include/Database.h>

class CDevice;

class CDeviceRegistry
{
public:
    CDeviceRegistry();
    ~CDeviceRegistry();
    bool Init();
    int EnumerateVolumeDevices(wxString& VolumeList);
    int EnumeratePnPDevices(wxArrayString& PnPDeviceIDs, bool RemovedDevicesOnly = false);
    bool IsRegistered(CDevice* pDevice);
    bool RegisterDevice(CDevice* pDevice);
    bool AddDevice(CDevice** ppDeviceToAdd);
    bool RemoveDevice(CDevice* pDevice);
    void Clear();
    bool GetDeviceFromVolumeStr(wxString& VolumeStr, CDevice** ppDevice);
    int GetNumberConnectedDevices();
    void GetConnectedDeviceNames(wxArrayString& Arr);
    wxString GetFriendlyNameFromPnPDevice(PWSTR DeviceID);
    wxString GetManufacturerFromPnPDevice(PWSTR DeviceID);
    wxString GetDescriptionFromPnPDevice(PWSTR DeviceID);

private:
    cmsDatabase                        m_Database;
    CComPtr<IPortableDeviceManager>    m_pPortableDeviceManager;
    wxVector<CDevice*>                 m_ConnectedDevices;
};


#endif // DEVICEREGISTRY_H_INCLUDED
