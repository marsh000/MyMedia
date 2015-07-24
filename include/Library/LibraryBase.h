#ifndef LIBRARYBASE_H_INCLUDED
#define LIBRARYBASE_H_INCLUDED

#include <include/wxMSWHeaders.h>
#include <include/Media/MediaBase.h>

class CDevice;
class cmsDatabase;

class CLibrary : public wxPanel
{
public:
    CLibrary(wxWindow* pParent, wxWindowID ID);
    virtual ~CLibrary();

    virtual bool InitDatabase() = 0;
    virtual bool LoadLibraryFromDatabase() = 0;
    virtual bool InsertMediaInList(CMedia* pMedia) = 0;
    virtual bool DeleteMedia(CMedia* pMedia) = 0;
    virtual bool GetMedia(int Id, CMedia* pMedia) = 0;
    virtual bool SaveMedia(CMedia* pMedia) = 0;

    CDevice* GetDevice() const;
    int GetItemCount() const;
    wxString GetLibraryPath() const;
    wxString GetLibraryName() const;
    cmsMediaType GetMediaType() const;

    void SetDevice(CDevice* pDevice);
    void SetLibraryPath(const wxString& Path);
    void SetMediaType(cmsMediaType Type);

protected:
    int                      m_ItemCount;
    wxString                 m_LibraryName;
    wxString                 m_LibraryPath;
    cmsMediaType             m_MediaType;
    cmsDatabase*             m_pDatabase;
    CDevice*                 m_pDevice;
    wxTreeListCtrl*          m_pTreelistCtrl;
};

#endif // LIBRARYBASE_H_INCLUDED
