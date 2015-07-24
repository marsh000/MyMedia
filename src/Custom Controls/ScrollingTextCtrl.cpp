#include <include/Custom Controls/ScrollingTextCtrl.h>
#include <include/Custom Controls/Media Players/MediaPlayerStyling.h>

cmsScrollingTextCtrl::cmsScrollingTextCtrl(wxWindow* pParent, wxWindowID ID, const wxSize& Size)
: wxTextCtrl(pParent, ID, wxEmptyString, wxDefaultPosition, Size, wxTE_READONLY | wxNO_BORDER)
{
    this->SetFont(_MUSIC_CONTROL_FONT);
    this->SetForegroundColour(_MUSIC_CONTROL_FOREGROUND_COLOR);
    this->SetBackgroundColour(_MUSIC_CONTROL_BACKGROUND_COLOR);
    // get average width of characters
    wxClientDC dc(this);
    dc.SetFont(this->GetFont() );
    m_CharWidth = dc.GetCharWidth();
    m_pTimer = new cmsScrollingTextCtrlTimer(this, wxID_ANY);
}

cmsScrollingTextCtrl::~cmsScrollingTextCtrl()
{
    delete m_pTimer;
}

void cmsScrollingTextCtrl::ClearDisplay()
{
    this->Clear();
    if (m_pTimer->IsRunning() )
        m_pTimer->Stop();
}

void cmsScrollingTextCtrl::SetText(wxString& Title, wxString& Artist, wxString& Album)
{
    // set initial text
    wxString str = wxString::Format("\"%s\" | %s | %s", Title, Artist, Album);
    this->ClearDisplay();
    this->ChangeValue(str);

    // check if text length is greater than window size
    int w = 0, h = 0;
    this->GetClientSize(&w, &h);
    int strlen = str.length();
    int numchars = w / m_CharWidth;
    if (strlen > numchars)
    {
        this->AppendText(" | ");
        m_pTimer->Start(200);
    }
}



cmsScrollingTextCtrlTimer::cmsScrollingTextCtrlTimer(cmsScrollingTextCtrl* pOwner, wxWindowID ID)
: wxTimer(pOwner, ID), m_pOwner(pOwner)
{

}

cmsScrollingTextCtrlTimer::~cmsScrollingTextCtrlTimer()
{

}

void cmsScrollingTextCtrlTimer::Notify()
{
//    m_pOwner->ScrollWindow(-1, 0);

    wxString str = m_pOwner->GetRange(0, 1);
    m_pOwner->Remove(0, 1);
    m_pOwner->AppendText(str);
}
