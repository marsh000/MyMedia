#ifndef MUSICPLAYER_H_INCLUDED
#define MUSICPLAYER_H_INCLUDED

#include <include/wxMSWHeaders.h>

class CSong;
class cmsScrollingTextCtrl;
class cmsSliderCtrl;
class cmsTimerCtrl;
class cmsVolumeCtrl;
class cmsSliderEvent;

class CMusicPlayer : public wxPanel
{
public:
    CMusicPlayer(wxWindow* pParent, wxWindowID ID);
    ~CMusicPlayer();
    bool InitializeDisplay();
    wxMediaState GetState();
    bool Load(CSong* pSong);
    bool Play();
    bool Pause();
    bool Stop();

    const int GetMediaCtrlVolume();

    void SetMediaCtrlVolume(const int Volume);

protected:
    enum
    {
        idRevSeekBtn = 1100,
        idForSeekBtn,
        idPlayBtn,
        idStopBtn,
        idVolumeCtrl,
        idRepeatBtn,
        idShuffleBtn,
        idStreamPositionSlider,
    };

private:
    void OnMediaLoaded(wxMediaEvent& Event);
    void OnMediaStoping(wxMediaEvent& Event);
    void OnMediaFinished(wxMediaEvent& Event);
    void OnMediaStateChanged(wxMediaEvent& Event);
    void OnMediaPlay(wxMediaEvent& Event);
    void OnMediaPause(wxMediaEvent& Event);
    void OnRevSeekBtnClicked(wxCommandEvent& Event);
    void OnStopBtnClicked(wxCommandEvent& Event);
    void OnPlayBtnClicked(wxCommandEvent& Event);
    void OnForSeekBtnClicked(wxCommandEvent& Event);
    void OnRepeatBtnClicked(wxCommandEvent& Event);
    void OnShuffleBtnClicked(wxCommandEvent& Event);
    void OnStreamPositionChange(cmsSliderEvent& Event);
    void OnStreamPositionTimer(cmsSliderEvent& Event);

    wxWindow*                     m_pParent;
    bool                          m_IsOk;
    wxMediaCtrl                   m_MediaCtrl;
//    wxImageList                   m_ImageList;
    bool                          m_IsShuffleOn;
    bool                          m_IsRepeatOn;

    cmsScrollingTextCtrl*         m_pTrackInfoDisplay;
    cmsTimerCtrl*                 m_pTimeElapsedDisplay;
    cmsTimerCtrl*                 m_pTimeRemainingDisplay;
    cmsSliderCtrl*                m_pStreamPositionSlider;
    cmsVolumeCtrl*                m_pVolumeCtrl;
    wxButton*                     m_pRepeatBtn;
    wxButton*                     m_pShuffleBtn;

//    struct _IMAGE_LIST_INDEX
//    {
//        int                       GreenDot;
//        int                       RedDot;
//    } m_ImageIndex;
};

class cmsMediaPlayerEvent : public wxNotifyEvent
{
public:
    cmsMediaPlayerEvent() : wxNotifyEvent(wxEVT_NULL, 0)
    {

    }

    cmsMediaPlayerEvent(wxEventType EventType, int ID) : wxNotifyEvent(EventType, ID)
    {

    }

    cmsMediaPlayerEvent(const cmsMediaPlayerEvent& Event) : wxNotifyEvent(Event)
    {

    }

    virtual cmsMediaPlayerEvent* Clone() const
    {
        return new cmsMediaPlayerEvent(*this);
    }

    virtual ~cmsMediaPlayerEvent()
    {

    }

private:
//    wxDECLARE_DYNAMIC_CLASS(cmsMediaPlayerEvent);
};

// define event types associated with this class for use with Bind<>()
wxDECLARE_EVENT(cmsEVT_SHUFFLE_ON, cmsMediaPlayerEvent);
wxDECLARE_EVENT(cmsEVT_SHUFFLE_OFF, cmsMediaPlayerEvent);
wxDECLARE_EVENT(cmsEVT_REPEAT_ON, cmsMediaPlayerEvent);
wxDECLARE_EVENT(cmsEVT_REPEAT_OFF, cmsMediaPlayerEvent);
wxDECLARE_EVENT(cmsEVT_MEDIA_STOPPED, cmsMediaPlayerEvent);
wxDECLARE_EVENT(cmsEVT_MEDIA_FINISHED, cmsMediaPlayerEvent);

// define macro for creating the event table entries for each event type
#define EVT_CSTM_SHUFFLE_ON(func, id) wx__DECLARE_EVT1(cmsEVT_SHUFFLE_ON, id, &func)
#define EVT_CSTM_SHUFFLE_OFF(func, id) wx__DECLARE_EVT1(cmsEVT_SHUFFLE_OFF, id, &func)
#define EVT_CSTM_REPEAT_ON(func, id) wx__DECLARE_EVT1(cmsEVT_REPEAT_ON, id, &func)
#define EVT_CSTM_REPEAT_OFF(func, id) wx__DECLARE_EVT1(cmsEVT_REPEAT_OFF, id, &func)
#define EVT_CSTM_MEDIA_STOPPED(func, id) wx__DECLARE_EVT1(cmsEVT_MEDIA_STOPPED, id, &func)
#define EVT_CSTM_MEDIA_FINISHED(func, id) wx__DECLARE_EVT1(cmsEVT_MEDIA_FINISHED, id, &func)

#endif // MUSICPLAYER_H_INCLUDED
