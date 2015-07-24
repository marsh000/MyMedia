#ifndef ELAPSEDTIMECTRL_H_INCLUDED
#define ELAPSEDTIMECTRL_H_INCLUDED

#include <include/wxMSWHeaders.h>

enum ETimerFlags
{
    cmsTIMER_INCREASE,
    cmsTIMER_DECREASE
};

class cmsTimer;

class cmsTimerCtrl : public wxTextCtrl
{
public:
    cmsTimerCtrl(wxWindow* pParent, wxWindowID ID, const wxSize& Size = wxDefaultSize, int ExtFlags = cmsTIMER_INCREASE);
    ~cmsTimerCtrl();
    void SetMediaCtrlObject(wxMediaCtrl& MediaCtrl);
    void StopTimer();
    void StartTimer(long StartTimeInSeconds = 0);
    void StepTime();
    void UpdateDisplay(long TimeInSeconds);

private:
    wxMediaCtrl*                  m_pMediaCtrl;
    cmsTimer*                     m_pTimer;
    int                           m_ExtFlags;
    long                          m_CurrentTimeInSeconds;
};

class cmsTimer : public wxTimer
{
public:
    cmsTimer(cmsTimerCtrl* pOwner, wxWindowID ID);
    ~cmsTimer();
    void Notify();

private:
    cmsTimerCtrl*                 m_pOwner;
};

#endif // ELAPSEDTIMECTRL_H_INCLUDED
