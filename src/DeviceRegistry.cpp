#include <include/DeviceRegistry.h>
#include <include/Device/DeviceBase.h>
#include <include/ErrorLog.h>
#include <include/FilePaths.h>

CDeviceRegistry::CDeviceRegistry()
{

}

CDeviceRegistry::~CDeviceRegistry()
{
    // clear all registered devices
    wxVector<CDevice*>::iterator iter;
    for (iter = m_ConnectedDevices.begin(); iter != m_ConnectedDevices.end(); ++iter)
    {
        if (*iter != NULL)
        {
            delete *iter;
            *iter = NULL;
        }
    }

    // the database d'tor will close the connection
}

bool CDeviceRegistry::Init()
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types
{
    // open the registry database
    if (m_Database.OpenDatabase(cmsPaths::GetRegistryFilePath() ) )
    {
        // create sql table if needed
        wxString SqlStr;
        SqlStr << "CREATE TABLE IF NOT EXISTS main.tab_DeviceList "
               << "("
               << "col_SerialNumber TEXT NOT NULL UNIQUE,"
               << "col_Type INTEGER NOT NULL,"
               << "col_Name TEXT NOT NULL UNIQUE"
               << ")";
        if (m_Database.ProcessStatement(SqlStr) )
        {
//            if (FAILED(CoCreateInstance(CLSID_PortableDeviceManager, // <-- WIN32 API function call
//                                        NULL,
//                                        CLSCTX_INPROC_SERVER,
//                                        IID_PPV_ARGS(&m_pPortableDeviceManager) ) ) )
//            {
                return true;
//            }
//            else
//            {
//                cmsLogError("Could not start Portable Device Manager");
//            }
        }
        else
        {
            wxString s = "Could not start device registry\nSQL: ";
            s << m_Database.GetLastErrorMsg();
            cmsLogError(s);
        }
    }
    return false;
}

int CDeviceRegistry::EnumerateVolumeDevices(wxString& VolumeList)
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types
{
    VolumeList = wxEmptyString;

    // get list of all drives currently attached to this computer
    unsigned long VolumeBitMask = GetLogicalDrives(); // <-- WIN32 API Function Call

    // iterate though all available drive letters and find matches
    for (char Letters = 0; Letters < 26; ++Letters)
    {
        if (VolumeBitMask & 0x1)
        {
            char Volume = Letters + 'A';
            if (Volume != 'C')
            {
                VolumeList << Volume;
            }
        }

        // shift to the next bit in the drive list
        VolumeBitMask = VolumeBitMask >> 1;
    }
    return static_cast<int>(VolumeList.Length() );
}

int CDeviceRegistry::EnumeratePnPDevices(wxArrayString& PnPDeviceIDs, bool RemovedDevicesOnly)
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types
// Adapted from MSDN: http://msdn.microsoft.com/en-us/library/windows/desktop/dd388688(v=vs.85).aspx
{
    if (!m_pPortableDeviceManager)
        return -1;

    // first, refresh the device list
    m_pPortableDeviceManager->RefreshDeviceList();

    // get number of connected devices
    unsigned long NumDevices; // should be DWORD (unsigned long)
    HRESULT hr = m_pPortableDeviceManager->GetDevices(NULL, &NumDevices);
    if (FAILED(hr) )
    {
        return -1;
    }

    // get the device list
    PWSTR* ppAllDeviceIDs = new PWSTR[NumDevices];
    if (ppAllDeviceIDs != NULL)
    {
        hr = m_pPortableDeviceManager->GetDevices(ppAllDeviceIDs, &NumDevices);
        if (SUCCEEDED(hr) )
        {
            for (unsigned long x = 0; x < NumDevices; x++)
            {
                PnPDeviceIDs.push_back(ppAllDeviceIDs[x] );
            }
        }
    }

    // free the array
    for (unsigned long x = 0; x < NumDevices; x++)
    {
        CoTaskMemFree(ppAllDeviceIDs[x] );
        ppAllDeviceIDs[x] = NULL;
    }

    // Delete the array of PWSTR pointers
    delete [] ppAllDeviceIDs;
    ppAllDeviceIDs = NULL;

    return PnPDeviceIDs.GetCount();
}

bool CDeviceRegistry::IsRegistered(CDevice* pDevice)
{
    // check if device is in the registry database
    wxString SqlStr = "SELECT col_SerialNumber FROM main.tab_DeviceList";
    cmsSQLQueryId* pQueryId = NULL;
    if (!m_Database.BeginQuery(SqlStr, &pQueryId) )
    {
        return false;
    }
    bool IsRegistered = false;
    while (m_Database.GetNextRowFromQuery(pQueryId) == SQLITE_ROW)
    {
        wxString SerialNumber = m_Database.GetItem(pQueryId, 0);
        if (SerialNumber == pDevice->GetSerialNumber() )
        {
            IsRegistered = true;
            break;
        }
    }
    m_Database.EndQuery(&pQueryId);
    return IsRegistered;
}

bool CDeviceRegistry::RegisterDevice(CDevice* pDevice)
{
    // add device to database registry
    wxString Str = wxEmptyString;
    Str << "INSERT INTO main.tab_DeviceList"
        << "(col_SerialNumber,col_Type,col_Name) VALUES ("
        << "'" << pDevice->GetSerialNumber() << "',"
        << pDevice->GetDeviceType() << ","
        << "'" << pDevice->GetDeviceName() << "')";
    if (!m_Database.ProcessStatement(Str) )
    {
        return false;
    }

    return true;
/*
    switch (pDevice->GetType() )
    {
    case DEVTYPE_MASTER:
        {
            wxDir::Make(cmsPaths::GetMusicDir(), wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
        break;

    case DEVTYPE_VOLUME:
        {
            wxString DirName = pDevice->GetVolume() + "Music\\";
            wxDir::Make(DirName, wxS_DIR_DEFAULT, wxPATH_MKDIR_FULL);
        }
        break;

    case DEVTYPE_PNP_PARENT:
        break;

    case DEVTYPE_PNP_CHILD:
        {
            if (pDevice->GetPnPInternalStorageMusicID() == wxEmptyString)
            {
                if (pDevice->GetPnPInternalStorageRootID() != wxEmptyString)
                {
                    wxString ID;
                    if (cmsSUCCESS == pDevice->CreateFolder("Music",
                                                            pDevice->GetPnPInternalStorageRootID(),
                                                            &ID) )
                    {
                        pDevice->SetPnPInternalStorageMusicID(ID);
                    }
                }
            }
            if (pDevice->GetPnPExternalStorageMusicID() == wxEmptyString)
            {
                if (pDevice->GetPnPEnternalStorageRootID() != wxEmptyString)
                {
                    wxString ID;
                    if (cmsSUCCESS == pDevice->CreateFolder("Music",
                                                            pDevice->GetPnPEnternalStorageRootID(),
                                                            &ID) )
                    {
                        pDevice->SetPnPEnternalStorageMusicID(ID);
                    }
                }
            }
        }
        break;

    default:
        return false;
    }

    return true;
*/
}

bool CDeviceRegistry::AddDevice(CDevice** ppDeviceToAdd)
{
    wxASSERT_MSG(*ppDeviceToAdd != NULL, "Attempt to add a NULL device");

    m_ConnectedDevices.push_back(*ppDeviceToAdd);

    return true;
}

bool CDeviceRegistry::RemoveDevice(CDevice* pDevice)
{
    wxVector<CDevice*>::iterator iter;
    for (iter = m_ConnectedDevices.begin(); iter != m_ConnectedDevices.end(); ++iter)
    {
        // delete the device from the device managers list
        (*iter)->CloseDevice();
        delete *iter;
        *iter = NULL;
        m_ConnectedDevices.erase(iter);
        pDevice = NULL;
        return true;
    }
    return false;
}

void CDeviceRegistry::Clear()
{
    wxVector<CDevice*>::iterator iter;
    for (iter = m_ConnectedDevices.begin(); iter != m_ConnectedDevices.end(); ++iter)
    {
        // delete the device from the device managers list
        (*iter)->CloseDevice();
        delete *iter;
        *iter = NULL;
    }
    m_ConnectedDevices.clear();
}

bool CDeviceRegistry::GetDeviceFromVolumeStr(wxString& VolumeStr, CDevice** ppDevice)
{
    wxVector<CDevice*>::iterator iter;
    for (iter = m_ConnectedDevices.begin(); iter != m_ConnectedDevices.end(); ++iter)
    {
//        wxString str;
//        str << "iter = " << (*iter)->GetDeviceBasePath() << " , VolumeStr = " << VolumeStr;
//        wxMessageBox(str);
        if ( (*iter)->GetDeviceBasePath() == VolumeStr)
        {
            *ppDevice = *iter;
            return true;
        }
    }
    *ppDevice = NULL;
    return false;
}

int CDeviceRegistry::GetNumberConnectedDevices()
{
    return static_cast<int>(m_ConnectedDevices.size() );
}

void CDeviceRegistry::GetConnectedDeviceNames(wxArrayString& Arr)
{
    wxVector<CDevice*>::iterator iter;
    for (iter = m_ConnectedDevices.begin(); iter != m_ConnectedDevices.end(); ++iter)
    {
        Arr.Add( (*iter)->GetDeviceName() );
    }
}

wxString CDeviceRegistry::GetFriendlyNameFromPnPDevice(PWSTR DeviceID)
{
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types
    DWORD FriendlyNameLength = 0;
    wxString RetStr;

    if (FAILED(m_pPortableDeviceManager->GetDeviceFriendlyName(DeviceID, NULL, &FriendlyNameLength) ) )
    {
        cmsLogError("Failed to get number of characters for device friendly name");
    }
    else if (FriendlyNameLength > 0)
    {
        PWSTR FriendlyName = new WCHAR[FriendlyNameLength];
        if (FriendlyName != NULL)
        {
            ZeroMemory(FriendlyName, FriendlyNameLength * sizeof(WCHAR) );
            if (SUCCEEDED(m_pPortableDeviceManager->GetDeviceFriendlyName(DeviceID, FriendlyName, &FriendlyNameLength) ) )
            {
                RetStr =  FriendlyName;
            }
            else
            {
                cmsLogError("Failed to get device friendly name");
            }

            delete[] FriendlyName;
            FriendlyName = NULL;
        }
        else
        {
            cmsLogError("Failed to allocate memory for the device friendly name string");
        }
    }
    else
    {
        cmsLogError("The device did not provide a friendly name.");
    }

    return RetStr;
}

wxString CDeviceRegistry::GetManufacturerFromPnPDevice(PWSTR DeviceID)
{
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types
    DWORD ManufacturerLength = 0;
    wxString RetStr;

    if (FAILED(m_pPortableDeviceManager->GetDeviceManufacturer(DeviceID, NULL, &ManufacturerLength) ) )
    {
        cmsLogError("Failed to get number of characters for device friendly name");
    }
    else if (ManufacturerLength > 0)
    {
        PWSTR Manufacturer = new WCHAR[ManufacturerLength];
        if (Manufacturer != NULL)
        {
            ZeroMemory(Manufacturer, ManufacturerLength * sizeof(WCHAR) );
            if (SUCCEEDED(m_pPortableDeviceManager->GetDeviceManufacturer(DeviceID, Manufacturer, &ManufacturerLength) ) )
            {
                RetStr =  Manufacturer;
            }
            else
            {
                cmsLogError("Failed to get device friendly name");
            }

            delete[] Manufacturer;
            Manufacturer = NULL;
        }
        else
        {
            cmsLogError("Failed to allocate memory for the device friendly name string");
        }
    }
    else
    {
        cmsLogError("The device did not provide a friendly name.");
    }

    return RetStr;
}

wxString CDeviceRegistry::GetDescriptionFromPnPDevice(PWSTR DeviceID)
{
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types
    DWORD DescriptionLength = 0;
    wxString RetStr;

    if (FAILED(m_pPortableDeviceManager->GetDeviceDescription(DeviceID, NULL, &DescriptionLength) ) )
    {
        cmsLogError("Failed to get number of characters for device friendly name");
    }
    else if (DescriptionLength > 0)
    {
        PWSTR Description = new WCHAR[DescriptionLength];
        if (Description != NULL)
        {
            ZeroMemory(Description, DescriptionLength * sizeof(WCHAR) );
            if (SUCCEEDED(m_pPortableDeviceManager->GetDeviceDescription(DeviceID, Description, &DescriptionLength) ) )
            {
                RetStr =  Description;
            }
            else
            {
                cmsLogError("Failed to get device friendly name");
            }

            delete[] Description;
            Description = NULL;
        }
        else
        {
            cmsLogError("Failed to allocate memory for the device friendly name string");
        }
    }
    else
    {
        cmsLogError("The device did not provide a friendly name.");
    }

    return RetStr;
}

