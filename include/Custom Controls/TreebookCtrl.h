#ifndef TREEBOOKCTRL_H_INCLUDED
#define TREEBOOKCTRL_H_INCLUDED

#include <include/wxMSWHeaders.h>

class cmsTreebookEvent;
class cmsTreebookPage;
class cmsTreebookUserData;
class CMusicPlayer;
class CSong;

class cmsTreebookCtrl : public wxPanel
{
public:
    cmsTreebookCtrl(wxWindow* pParent, wxWindowID Id = wxID_ANY);
    virtual ~cmsTreebookCtrl();

    bool AppendChildPage(cmsTreebookPage* pChildPage, cmsTreebookPage* pParentPage, const wxString& Label, bool ShowIt = false);
    cmsTreebookPage* AppendEmptyChildPage(cmsTreebookPage* pParentPage, const wxString& Label, bool ShowIt = false);
    cmsTreebookPage* AppendEmptyPage(const wxString& Label, bool ShowIt = false);
    bool AppendPage(cmsTreebookPage* pPage, const wxString& Label, bool ShowIt = false);
    void CloseCurrentPage();
    void DeleteAllPages();
    bool DeleteChildPages(cmsTreebookPage* pPage, bool Recursive = false);
    bool DeletePage(cmsTreebookPage* pPage);
    void ExpandAllPages();
    void ExpandPage(const cmsTreebookPage* pPage);
    cmsTreebookPage* GetCurrentPage() const;
    cmsTreebookPage* GetFirstTopLevelPage() const;
    wxString GetPageLabel(cmsTreebookPage* pPage) const;
    cmsTreebookPage* GetFirstChild(cmsTreebookPage* pParent) const;
    cmsTreebookPage* GetNextSibling(cmsTreebookPage* pPage) const;
    cmsTreebookPage* GetPageParent(cmsTreebookPage* pChildPage);
    wxTreeCtrl* GetTreeCtrl() const;
    cmsTreebookUserData* GetUserData(cmsTreebookPage* pPage) const;
    bool IsTopLevelPage(cmsTreebookPage* pPage);
    bool ChangePage(cmsTreebookPage* pPage);
    bool PlaySong(CSong* pSong);
    void SetImageList(wxImageList* pImageList);
    void SetPageImage(cmsTreebookPage* pPage, int ImageId);
    void SetPageLabel(cmsTreebookPage* pPage, const wxString& Label);
    cmsTreebookUserData* SetUserData(cmsTreebookPage* pPage, cmsTreebookUserData* pData);

protected:
    void OnTreeItemDoubleClick(wxTreeEvent& Event);
    void OnTreeItemRightClick(wxTreeEvent& Event);
    void OnTreeItemLeftClick(wxTreeEvent& Event);

private:
    wxTreeCtrl*              m_pTreeCtrl;
    wxBoxSizer*              m_pSizer;
    wxSizerFlags             m_PageSizerFlags;
    CMusicPlayer*            m_pMusicPlayerCtrl;
    cmsTreebookPage*         m_pCurrentPage;
};

class cmsTreebookUserData
{
public:
    cmsTreebookUserData()
    {
    }

    virtual ~cmsTreebookUserData()
    {
    }
};

class cmsTreebookPage : public wxPanel, wxTreeItemData
{
friend class cmsTreebookCtrl;
public:
    cmsTreebookPage(cmsTreebookCtrl* pParent, wxWindowID Id)
    : wxPanel(pParent, Id, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL | wxBORDER_THEME),
      m_pUserData(NULL)
    {
    }

    virtual ~cmsTreebookPage()
    {
        if (m_pUserData != NULL)
        {
            delete m_pUserData;
            m_pUserData = NULL;
        }
    }

    wxString GetLabel() const
    {
        return m_Label;
    }

    cmsTreebookUserData* GetUserData() const
    {
        return m_pUserData;
    }

    bool HasUserData() const
    {
        if (m_pUserData == NULL)
            return false;
        return true;
    }

    virtual bool RefreshPage()
    {
        wxFAIL_MSG("Cannot use base class here!");
        return true;
    }

    void SetLabel(const wxString& Label)
    {
        m_Label = Label;
    }

    cmsTreebookUserData* SetUserData(cmsTreebookUserData* pNewData)
    {
        cmsTreebookUserData* pOldData = m_pUserData;
        m_pUserData = pNewData;
        return pOldData;
    }

protected:
    wxTreeItemId GetTreeItemId() const
    {
        return m_TreeItemId;
    }

    void SetTreeItemId(wxTreeItemId& ItemId)
    {
        m_TreeItemId = ItemId;
    }

private:
    wxString                 m_Label;
    cmsTreebookUserData*     m_pUserData;
    wxTreeItemId             m_TreeItemId;
};

class cmsTreebookEvent : public wxNotifyEvent
{
public:
    cmsTreebookEvent() : wxNotifyEvent(wxEVT_NULL, 0), m_pPage(NULL), m_pOldPage(NULL)
    {
    }

    cmsTreebookEvent(wxEventType EventType, int ID) : wxNotifyEvent(EventType, ID), m_pPage(NULL), m_pOldPage(NULL)
    {
    }

    cmsTreebookEvent(const cmsTreebookEvent& Event) : wxNotifyEvent(Event), m_pPage(NULL), m_pOldPage(NULL)
    {
    }

    virtual cmsTreebookEvent* Clone() const
    {
        return new cmsTreebookEvent(*this);
    }

    virtual ~cmsTreebookEvent()
    {
    }

    cmsTreebookPage* GetPage() const
    {
        return m_pPage;
    }

    cmsTreebookPage* GetOldPage() const
    {
        return m_pOldPage;
    }

    void SetPage(cmsTreebookPage* pPage)
    {
        m_pPage = pPage;
    }

    void SetOldPage(cmsTreebookPage* pOldPage)
    {
        m_pOldPage = pOldPage;
    }

private:
    cmsTreebookPage*         m_pPage;
    cmsTreebookPage*         m_pOldPage;

    wxDECLARE_DYNAMIC_CLASS(cmsTreebookEvent);
};

wxDECLARE_EVENT(cmsEVT_TREEBOOK_PAGE_CLOSED, cmsTreebookEvent);
wxDECLARE_EVENT(cmsEVT_TREEBOOK_PAGE_CLOSING, cmsTreebookEvent);
wxDECLARE_EVENT(cmsEVT_TREEBOOK_PAGE_RIGHT_CLICK, cmsTreebookEvent);
wxDECLARE_EVENT(cmsEVT_TREEBOOK_PAGE_CHANGED, cmsTreebookEvent);
wxDECLARE_EVENT(cmsEVT_TREEBOOK_PAGE_CHANGING, cmsTreebookEvent);

#define EVT_TREEBOOK_PAGE_CLOSED(func, id) wx__DECLARE_EVT1(cmsEVT_TREEBOOK_PAGE_CLOSED, id, &func)
#define EVT_TREEBOOK_PAGE_CLOSING(func, id) wx__DECLARE_EVT1(cmsEVT_TREEBOOK_PAGE_CLOSING, id, &func)
#define EVT_TREEBOOK_PAGE_RIGHT_CLICK(func, id) wx__DECLARE_EVT1(cmsEVT_TREEBOOK_PAGE_RIGHT_CLICK, id, &func)
#define EVT_TREEBOOK_PAGE_CHANGED(func, id) wx__DECLARE_EVT1(cmsEVT_TREEBOOK_PAGE_CHANGED, id, &func)
#define EVT_TREEBOOK_PAGE_CHANGING(func, id) wx__DECLARE_EVT1(cmsEVT_TREEBOOK_PAGE_CHANGING, id, &func)

#endif // TREEBOOKCTRL_H_INCLUDED
