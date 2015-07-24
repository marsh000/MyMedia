#include <include/Custom Controls/ElapsedTimeCtrl.h>
#include <include/Custom Controls/Media Players/MediaPlayerStyling.h>

cmsTimerCtrl::cmsTimerCtrl(wxWindow* pParent, wxWindowID ID, const wxSize& Size, int ExtFlags)
: wxTextCtrl(pParent, ID, wxEmptyString, wxDefaultPosition, Size, wxTE_READONLY | wxNO_BORDER),
  m_ExtFlags(ExtFlags)
{
    this->SetFont(_MUSIC_CONTROL_FONT);
    this->SetOwnForegroundColour(_MUSIC_CONTROL_FOREGROUND_COLOR);
    this->SetBackgroundColour(_MUSIC_CONTROL_BACKGROUND_COLOR);
    m_pTimer = new cmsTimer(this, wxID_ANY);
}

cmsTimerCtrl::~cmsTimerCtrl()
{
    delete m_pTimer;
}

void cmsTimerCtrl::SetMediaCtrlObject(wxMediaCtrl& MediaCtrl)
{
    m_pMediaCtrl = &MediaCtrl;
}

void cmsTimerCtrl::StopTimer()
{
    this->Clear();
    if (m_pTimer->IsRunning() )
        m_pTimer->Stop();
}

void cmsTimerCtrl::StartTimer(long StartTimeInSeconds)
{
    m_CurrentTimeInSeconds = StartTimeInSeconds;
    // set initial text
    this->StopTimer();
    this->UpdateDisplay(StartTimeInSeconds);
    m_pTimer->Start(1000);
}

void cmsTimerCtrl::StepTime()
{
    if (m_ExtFlags == cmsTIMER_INCREASE)
    {
        this->UpdateDisplay( (m_pMediaCtrl->Tell() ) / 1000 );
    }
    else
    {
        this->UpdateDisplay( (m_pMediaCtrl->Length() - m_pMediaCtrl->Tell() ) / 1000 );
    }
//    this->UpdateDisplay(m_CurrentTimeInSeconds);
}

void cmsTimerCtrl::UpdateDisplay(long TimeInSeconds)
{
    long Minutes = ( (TimeInSeconds - (TimeInSeconds % 60) ) / 60);
    long Seconds = (TimeInSeconds % 60);
    if (m_ExtFlags == cmsTIMER_INCREASE)
    {
        this->SetValue(wxString::Format("%i:%02i", Minutes, Seconds) );
    }
    else
    {
        this->SetValue(wxString::Format("-%i:%02i", Minutes, Seconds) );
    }
}



cmsTimer::cmsTimer(cmsTimerCtrl* pOwner, wxWindowID ID)
: wxTimer(pOwner, ID), m_pOwner(pOwner)
{

}

cmsTimer::~cmsTimer()
{

}

void cmsTimer::Notify()
{
    m_pOwner->StepTime();
}
