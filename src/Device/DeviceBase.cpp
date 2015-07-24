#include <include/Device/DeviceBase.h>
#include <include/Database.h>
#include <include/FilePaths.h>

CDevice::CDevice()
{

}

CDevice::~CDevice()
{

}

bool CDevice::CloseDevice()
{
    return DoCloseDevice();
}

bool CDevice::OpenDevice()
{
    return DoOpenDevice();;
}

const wxString CDevice::GetDescription() const
{
    return m_Description;
}

cmsDeviceType CDevice::GetDeviceType() const
{
    return m_DeviceType;
}

const wxString CDevice::GetManufacturer() const
{
    return m_Manufacturer;
}

const wxString CDevice::GetDeviceName() const
{
    return m_DeviceName;
}

const wxString CDevice::GetDeviceBasePath() const
{
    return m_DeviceBasePath;
}

const wxString CDevice::GetSerialNumber() const
{
    return m_SerialNumber;
}

void CDevice::IsOK(const bool OK)
{
    m_IsOK = OK;
}

const bool CDevice::IsOK() const
{
    return m_IsOK;
}

void CDevice::SetDescription(const wxString& x)
{
    m_Description = x;
}

void CDevice::SetDeviceType(const cmsDeviceType x)
{
    m_DeviceType = x;
}

void CDevice::SetManufacturer(const wxString& x)
{
    m_Manufacturer = x;
}

void CDevice::SetDeviceName(const wxString& x)
{
    m_DeviceName = x;
}

void CDevice::SetDeviceBasePath(const wxString& x)
{
    m_DeviceBasePath = x;
}

void CDevice::SetSerialNumber(const wxString& x)
{
    m_SerialNumber = x;
}

