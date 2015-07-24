#ifndef SCROLLINGTEXTCTRL_H_INCLUDED
#define SCROLLINGTEXTCTRL_H_INCLUDED

#include <include/wxMSWHeaders.h>

class cmsScrollingTextCtrlTimer;

class cmsScrollingTextCtrl : public wxTextCtrl
{
public:
    cmsScrollingTextCtrl(wxWindow* pParent, wxWindowID ID, const wxSize& Size = wxDefaultSize);
    ~cmsScrollingTextCtrl();
    void ClearDisplay();
    void SetText(wxString& Title, wxString& Artist, wxString& Album);

private:
    cmsScrollingTextCtrlTimer*    m_pTimer;
    int                           m_CharWidth;
};

class cmsScrollingTextCtrlTimer : public wxTimer
{
public:
    cmsScrollingTextCtrlTimer(cmsScrollingTextCtrl* pOwner, wxWindowID ID);
    ~cmsScrollingTextCtrlTimer();
    void Notify();

private:
    cmsScrollingTextCtrl*           m_pOwner;
};

#endif // SCROLLINGTEXTCTRL_H_INCLUDED
