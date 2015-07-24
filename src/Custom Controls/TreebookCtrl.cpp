#include <include/Custom Controls/TreebookCtrl.h>
#include <include/Custom Controls/Media Players/MusicPlayer.h>

wxIMPLEMENT_DYNAMIC_CLASS(cmsTreebookEvent, wxNotifyEvent);

wxDEFINE_EVENT(cmsEVT_TREEBOOK_PAGE_CLOSED, cmsTreebookEvent);
wxDEFINE_EVENT(cmsEVT_TREEBOOK_PAGE_CLOSING, cmsTreebookEvent);
wxDEFINE_EVENT(cmsEVT_TREEBOOK_PAGE_RIGHT_CLICK, cmsTreebookEvent);
wxDEFINE_EVENT(cmsEVT_TREEBOOK_PAGE_CHANGED, cmsTreebookEvent);
wxDEFINE_EVENT(cmsEVT_TREEBOOK_PAGE_CHANGING, cmsTreebookEvent);


cmsTreebookCtrl::cmsTreebookCtrl(wxWindow* pParent, wxWindowID Id)
: wxPanel(pParent, Id), m_pCurrentPage(NULL)
{
    // create the controls
    m_pTreeCtrl = new wxTreeCtrl(this, wxID_ANY, wxDefaultPosition, wxSize(250, 1), wxTR_HAS_BUTTONS | wxTR_SINGLE | wxTR_NO_LINES | wxTR_HIDE_ROOT);
    m_pTreeCtrl->AddRoot("");

    m_pMusicPlayerCtrl = new CMusicPlayer(this, wxID_ANY);
    if (!m_pMusicPlayerCtrl->InitializeDisplay() )
    {

    }

    m_pCurrentPage = NULL;

    // position the controls
    wxBoxSizer* pLeftSizer = new wxBoxSizer(wxVERTICAL);
    pLeftSizer->Add(m_pTreeCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3) );
    pLeftSizer->Add(m_pMusicPlayerCtrl, wxSizerFlags(1).Expand().Border(wxALL, 3) );

    m_pSizer = new wxBoxSizer(wxHORIZONTAL);
    m_PageSizerFlags = wxSizerFlags(6).Expand().Border(wxTOP | wxBOTTOM | wxRIGHT, 3);
    m_pSizer->Add(pLeftSizer, wxSizerFlags(1).Expand().Border(wxALL, 0) );
    m_pSizer->AddStretchSpacer(6);
    this->SetSizerAndFit(m_pSizer);

    // wxTreeCtrl events
    this->Bind(wxEVT_TREE_SEL_CHANGED, &cmsTreebookCtrl::OnTreeItemLeftClick, this, m_pTreeCtrl->GetId() );
    this->Bind(wxEVT_TREE_ITEM_ACTIVATED, &cmsTreebookCtrl::OnTreeItemDoubleClick, this, m_pTreeCtrl->GetId() );
    this->Bind(wxEVT_TREE_ITEM_RIGHT_CLICK, &cmsTreebookCtrl::OnTreeItemRightClick, this, m_pTreeCtrl->GetId() );
}

cmsTreebookCtrl::~cmsTreebookCtrl()
{
    this->DeleteAllPages();
}

bool cmsTreebookCtrl::AppendChildPage(cmsTreebookPage* pChildPage, cmsTreebookPage* pParentPage, const wxString& Label, bool ShowIt)
{
    wxASSERT_MSG(pParentPage != NULL, "Parent must be a valid page");
    wxASSERT_MSG(pChildPage != NULL, "Child must be a valid page");

    this->Freeze();
    bool Success = false;
    wxTreeItemId ItemId = m_pTreeCtrl->AppendItem(pParentPage->GetTreeItemId(), Label, -1, -1, pChildPage);
    if (ItemId.IsOk() )
    {
        pChildPage->SetTreeItemId(ItemId);
        pChildPage->SetLabel(Label);
        m_pSizer->Add(pChildPage, m_PageSizerFlags);
        m_pSizer->Hide(pChildPage);
        m_pSizer->Layout();
        if (ShowIt)
        {
            this->ChangePage(pChildPage);
        }
        Success = true;
    }
    this->Thaw();
    return Success;
}

cmsTreebookPage* cmsTreebookCtrl::AppendEmptyChildPage(cmsTreebookPage* pParentPage, const wxString& Label, bool ShowIt)
{
    wxASSERT_MSG(pParentPage != NULL, "Parent must be a valid page");

    cmsTreebookPage* pChildPage = new cmsTreebookPage(this, wxID_ANY);
    if (this->AppendChildPage(pChildPage, pParentPage, Label, ShowIt) )
    {
        return pChildPage;
    }
    delete pChildPage;
    return NULL;
}

cmsTreebookPage* cmsTreebookCtrl::AppendEmptyPage(const wxString& Label, bool ShowIt)
{
    cmsTreebookPage* pPage = new cmsTreebookPage(this, wxID_ANY);
    if (this->AppendPage(pPage, Label, ShowIt) )
    {
        return pPage;
    }
    delete pPage;
    return NULL;
}

bool cmsTreebookCtrl::AppendPage(cmsTreebookPage* pPage, const wxString& Label, bool ShowIt)
{
    wxASSERT_MSG(pPage != NULL, "pPage must be a valid page");

    this->Freeze();
    bool Success = false;
    if (pPage == NULL)
    {
        pPage = new cmsTreebookPage(this, wxID_ANY);
    }
    wxTreeItemId ItemId = m_pTreeCtrl->AppendItem(m_pTreeCtrl->GetRootItem(), Label, -1, -1, pPage);
    if (ItemId.IsOk() )
    {
        pPage->SetTreeItemId(ItemId);
        pPage->SetLabel(Label);
        m_pSizer->Add(pPage, m_PageSizerFlags);
        m_pSizer->Hide(pPage);
        m_pSizer->Layout();
        if (ShowIt)
        {
            this->ChangePage(pPage);
        }
        Success = true;
    }
    this->Thaw();
    return Success;
}

void cmsTreebookCtrl::CloseCurrentPage()
{
    if (m_pCurrentPage != NULL)
    {
        this->Freeze();
        m_pSizer->Hide(m_pCurrentPage);
        m_pCurrentPage = NULL;
        m_pSizer->Show( (size_t)1);
        m_pSizer->Layout();
        this->Thaw();
    }
}

void cmsTreebookCtrl::DeleteAllPages()
{
    this->Freeze();
    wxTreeItemIdValue Cookie;
    wxTreeItemId ItemId = m_pTreeCtrl->GetFirstChild(m_pTreeCtrl->GetRootItem(), Cookie);
    if (ItemId.IsOk() )
    {
        wxTreeItemId ItemToDelete = ItemId;
        ItemId = m_pTreeCtrl->GetNextChild(m_pTreeCtrl->GetRootItem(), Cookie);
        cmsTreebookPage* pPageToDelete = static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(ItemToDelete) );
        this->DeleteChildPages(pPageToDelete, true);
        this->DeletePage(pPageToDelete);
    }
    this->Thaw();
}

bool cmsTreebookCtrl::DeleteChildPages(cmsTreebookPage* pParentPage, bool Recursive)
{
    if (pParentPage != NULL)
    {
        this->Freeze();
        wxTreeItemIdValue Cookie;
        wxTreeItemId ChildId = m_pTreeCtrl->GetFirstChild(pParentPage->GetTreeItemId(), Cookie);
        while (ChildId.IsOk() )
        {
            wxTreeItemId ItemToDelete = ChildId;
            ChildId = m_pTreeCtrl->GetNextChild(pParentPage->GetTreeItemId(), Cookie);
            cmsTreebookPage* pPageToDelete = static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(ItemToDelete) );
            if (m_pTreeCtrl->ItemHasChildren(ItemToDelete) && Recursive)
            {
                this->DeleteChildPages(pPageToDelete, Recursive);
            }
            this->DeletePage(pPageToDelete);
        }
        this->Thaw();
        return true;
    }
    return false;
}

bool cmsTreebookCtrl::DeletePage(cmsTreebookPage* pPage)
{
    if (pPage != NULL)
    {
        this->Freeze();
        wxTreeItemId ItemId = pPage->GetTreeItemId();
        m_pTreeCtrl->SetItemData(ItemId, NULL);
        if (pPage->HasUserData() )
        {
            cmsTreebookUserData* pData = pPage->SetUserData(NULL);
            delete pData;
        }
        m_pCurrentPage = NULL;
        m_pSizer->Detach(pPage);
        m_pSizer->Show( (size_t)1);
        m_pSizer->Layout();
        delete pPage;
        m_pTreeCtrl->Delete(ItemId);
        this->Thaw();
        return true;
    }
    return false;
}

void cmsTreebookCtrl::ExpandAllPages()
{
    m_pTreeCtrl->ExpandAll();
}

void cmsTreebookCtrl::ExpandPage(const cmsTreebookPage* pPage)
{
    m_pTreeCtrl->Expand(pPage->GetTreeItemId() );
}

cmsTreebookPage* cmsTreebookCtrl::GetCurrentPage() const
{
    return m_pCurrentPage;
}

cmsTreebookPage* cmsTreebookCtrl::GetFirstTopLevelPage() const
{
    wxTreeItemIdValue Cookie;
    wxTreeItemId ItemId = m_pTreeCtrl->GetFirstChild(m_pTreeCtrl->GetRootItem(), Cookie);
    if (ItemId.IsOk() )
    {
        return static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(ItemId) );
    }
    return NULL;
}

wxString cmsTreebookCtrl::GetPageLabel(cmsTreebookPage* pPage) const
{
    return pPage->m_Label;
}

cmsTreebookPage* cmsTreebookCtrl::GetFirstChild(cmsTreebookPage* pParent) const
{
    wxASSERT(pParent != NULL);
    wxTreeItemId ParentId = pParent->GetTreeItemId();
    wxASSERT(ParentId.IsOk() );
    wxTreeItemIdValue Cookie;
    wxTreeItemId ChildId = m_pTreeCtrl->GetFirstChild(ParentId, Cookie);
    if (ChildId.IsOk() )
    {
        cmsTreebookPage* pChildPage = static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(ChildId) );
        wxASSERT(pChildPage != NULL);
        return pChildPage;
    }
    return NULL;
}

cmsTreebookPage* cmsTreebookCtrl::GetNextSibling(cmsTreebookPage* pPage) const
{
    wxASSERT(pPage != NULL);
    wxTreeItemId PageId = pPage->GetTreeItemId();
    wxASSERT(PageId.IsOk() );
    wxTreeItemId SiblingId = m_pTreeCtrl->GetNextSibling(PageId);
    if (SiblingId.IsOk() )
    {
        cmsTreebookPage* pSiblingPage = static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(SiblingId) );
        wxASSERT(pSiblingPage != NULL);
        return pSiblingPage;
    }
    return NULL;
}

cmsTreebookPage* cmsTreebookCtrl::GetPageParent(cmsTreebookPage* pChildPage)
{
    wxASSERT(pChildPage != NULL);
    wxTreeItemId ChildId = pChildPage->GetTreeItemId();
    wxTreeItemId ParentId = m_pTreeCtrl->GetItemParent(ChildId);
    if (!ParentId.IsOk() || ParentId == m_pTreeCtrl->GetRootItem() )
    {
        return NULL;
    }
    else
    {
        return static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(ParentId) );
    }
}

wxTreeCtrl* cmsTreebookCtrl::GetTreeCtrl() const
{
    return m_pTreeCtrl;
}

cmsTreebookUserData* cmsTreebookCtrl::GetUserData(cmsTreebookPage* pPage) const
{
    return pPage->GetUserData();
}

bool cmsTreebookCtrl::IsTopLevelPage(cmsTreebookPage* pPage)
{
    wxTreeItemId PageId = pPage->GetTreeItemId();
    if (PageId.IsOk() )
    {
        if (m_pTreeCtrl->GetItemParent(PageId) == m_pTreeCtrl->GetRootItem() )
        {
            return true;
        }
        return false;
    }
    return true;
}

bool cmsTreebookCtrl::ChangePage(cmsTreebookPage* pPage)
{
    wxASSERT(pPage != NULL);

    if (pPage != m_pCurrentPage)
    {
        this->Freeze();
        m_pTreeCtrl->SelectItem(pPage->GetTreeItemId() );
        if (m_pCurrentPage == NULL)
        {
            m_pSizer->Hide( (size_t)1);
        }
        else
        {
            m_pSizer->Hide(m_pCurrentPage);
        }
        m_pCurrentPage = pPage;
        m_pSizer->Show(pPage);
        m_pSizer->Layout();
        this->Thaw();
        return true;
    }
    return false;
}

void cmsTreebookCtrl::SetImageList(wxImageList* pImageList)
{
    m_pTreeCtrl->AssignImageList(pImageList);
}

void cmsTreebookCtrl::SetPageImage(cmsTreebookPage* pPage, int ImageId)
{
    m_pTreeCtrl->SetItemImage(pPage->GetTreeItemId(), ImageId);
}

void cmsTreebookCtrl::SetPageLabel(cmsTreebookPage* pPage, const wxString& Label)
{
    m_pTreeCtrl->SetItemText(pPage->GetTreeItemId(), Label);
}

cmsTreebookUserData* cmsTreebookCtrl::SetUserData(cmsTreebookPage* pPage, cmsTreebookUserData* pData)
{
    return pPage->SetUserData(pData);
}

void cmsTreebookCtrl::OnTreeItemDoubleClick(wxTreeEvent& Event)
{
/*
    this->ChangePage(Event.GetItem() );

    cmsTreebookEvent* pEvent = new cmsTreebookEvent(cmsEVT_TREEBOOK_ITEM_ACTIVATED, this->GetId() );
    pEvent->SetEventObject(static_cast<wxObject*>(this) );
//    pEvent->SetItem(Event.GetItem() );
    pEvent->SetPage(static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(Event.GetItem() ) );
    this->GetEventHandler()->QueueEvent(pEvent);
*/
}

void cmsTreebookCtrl::OnTreeItemRightClick(wxTreeEvent& Event)
{
    cmsTreebookPage* pPage = static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(Event.GetItem() ) );
//    m_pTreeCtrl->SelectItem(pPage->GetTreeItemId() );
    this->ChangePage(pPage);

    cmsTreebookEvent* pEvent = new cmsTreebookEvent(cmsEVT_TREEBOOK_PAGE_RIGHT_CLICK, this->GetId() );
    pEvent->SetEventObject(static_cast<wxObject*>(this) );
    pEvent->SetPage(pPage);
    wxTreeItemId OldItemId = Event.GetOldItem();
    if (OldItemId.IsOk() )
    {
        pEvent->SetOldPage(static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(OldItemId) ) );
    }
    this->GetEventHandler()->QueueEvent(pEvent);

}

void cmsTreebookCtrl::OnTreeItemLeftClick(wxTreeEvent& Event)
{
    cmsTreebookPage* pPage = static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(Event.GetItem() ) );
    this->ChangePage(pPage);

    cmsTreebookEvent* pEvent = new cmsTreebookEvent(cmsEVT_TREEBOOK_PAGE_CHANGED, this->GetId() );
    pEvent->SetEventObject(static_cast<wxObject*>(this) );
    pEvent->SetPage(pPage);
    wxTreeItemId OldItemId = Event.GetOldItem();
    if (OldItemId.IsOk() )
    {
        pEvent->SetOldPage(static_cast<cmsTreebookPage*>(m_pTreeCtrl->GetItemData(OldItemId) ) );
    }
    this->GetEventHandler()->QueueEvent(pEvent);
}

