#ifndef LIBRARYTREEITEMDATA_H_INCLUDED
#define LIBRARYTREEITEMDATA_H_INCLUDED

#include <include/Device/DeviceBase.h>
#include <include/Library/LibraryBase.h>

class CLibraryTreeItemData : public wxTreeItemData
{
public:
    CLibraryTreeItemData()
    : m_pDevice(NULL), m_pLibrary(NULL)
    {
    }

    CLibraryTreeItemData(CDevice* pDevice, CLibrary* pLibrary)
    : m_pDevice(pDevice), m_pLibrary(pLibrary)
    {
    }

    ~CLibraryTreeItemData()
    {
    }

    CDevice* GetDevice() const
    {
        return m_pDevice;
    }

    CLibrary* GetLibrary() const
    {
        return m_pLibrary;
    }

    void SetDevice(CDevice* pDevice)
    {
        m_pDevice = pDevice;
    }

    void SetLibrary(CLibrary* pLibrary)
    {
        m_pLibrary = pLibrary;
    }

private:
    CDevice*  m_pDevice;
    CLibrary* m_pLibrary;
};

class CListItemData : public wxClientData
{
public:
    CListItemData()
    : m_Id(-1)
    {
    }

    CListItemData(int Id)
    : m_Id(Id)
    {
    }

    ~CListItemData()
    {
    }

    int GetId()
    {
        return m_Id;
    }

    void SetId(int Id)
    {
        m_Id = Id;
    }

private:
    int  m_Id;
};

#endif // LIBRARYTREEITEMDATA_H_INCLUDED
