#include <include/Custom Controls/Media Players/MusicPlayer.h>
#include <include/Custom Controls/Media Players/MediaPlayerStyling.h>
#include <include/Custom Controls/SliderCtrl.h>
#include <include/Custom Controls/VolumeCtrl.h>
#include <include/Custom Controls/ScrollingTextCtrl.h>
#include <include/Custom Controls/ElapsedTimeCtrl.h>
#include <include/Media/Song.h>

//wxIMPLEMENT_DYNAMIC_CLASS(cmsMediaPlayerEvent, wxNotifyEvent);

wxDEFINE_EVENT(cmsEVT_SHUFFLE_ON, cmsMediaPlayerEvent);
wxDEFINE_EVENT(cmsEVT_SHUFFLE_OFF, cmsMediaPlayerEvent);
wxDEFINE_EVENT(cmsEVT_REPEAT_ON, cmsMediaPlayerEvent);
wxDEFINE_EVENT(cmsEVT_REPEAT_OFF, cmsMediaPlayerEvent);
wxDEFINE_EVENT(cmsEVT_MEDIA_STOPPED, cmsMediaPlayerEvent);
wxDEFINE_EVENT(cmsEVT_MEDIA_FINISHED, cmsMediaPlayerEvent);

CMusicPlayer::CMusicPlayer(wxWindow* pParent, wxWindowID ID)
: wxPanel(pParent, ID),
  m_pParent(pParent), m_IsOk(true), m_pTrackInfoDisplay(NULL),
  m_IsShuffleOn(false), m_IsRepeatOn(false)
{
//    m_ImageList.Create(16, 16);
//    m_ImageIndex.GreenDot = pImageList->Add(wxBitmap(wxGetCwd() + "/media/images/Green Dot.png", wxBITMAP_TYPE_PNG) );
//    m_ImageIndex.RedDot = pImageList->Add(wxBitmap(wxGetCwd() + "/media/images/Red Dot.png", wxBITMAP_TYPE_PNG) );
}

CMusicPlayer::~CMusicPlayer()
{

}

bool CMusicPlayer::InitializeDisplay()
{
// NOTE (Developer 1#9#): Uses WIN32 API function calls and/or types

    // find windows version
    bool UseWMP = false;
    OSVERSIONINFO vi;
    memset(&vi, 0, sizeof vi);
    vi.dwOSVersionInfoSize = sizeof vi;
    GetVersionEx(&vi);
    if (vi.dwPlatformId == VER_PLATFORM_WIN32_NT)
    {
        if (vi.dwMajorVersion >= 6 && vi.dwMinorVersion >= 1)
        {
            UseWMP = true;
        }
        else
        {
            UseWMP = false;
        }
    }
    else
    {
        m_IsOk = false;
        return false;
    }

    // create the media ctrl based on windows version
    if (UseWMP)
    {
        m_MediaCtrl.Create(m_pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxMEDIABACKEND_WMP10);
    }
    else
    {
        m_MediaCtrl.Create(m_pParent, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0, wxMEDIABACKEND_QUICKTIME);
    }
    m_MediaCtrl.SetVolume(10.0f);

    // media player buttons
    wxButton* pRevSeekBtn = new wxButton(this, idRevSeekBtn, wxEmptyString, wxDefaultPosition, wxSize(25,25), wxBU_NOTEXT | wxBORDER_NONE);
    pRevSeekBtn->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);
    pRevSeekBtn->SetBitmap(wxBitmap("resource\\images\\reverse.png", wxBITMAP_TYPE_PNG) );

    wxButton* pForSeekBtn = new wxButton(this, idForSeekBtn, wxEmptyString, wxDefaultPosition, wxSize(25,25), wxBU_NOTEXT | wxBORDER_NONE);
    pForSeekBtn->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);
    pForSeekBtn->SetBitmap(wxBitmap("resource\\images\\forward.png", wxBITMAP_TYPE_PNG) );

    wxButton* pPlayBtn = new wxButton(this, idPlayBtn, wxEmptyString, wxDefaultPosition, wxSize(25,25), wxBU_NOTEXT | wxBORDER_NONE);
    pPlayBtn->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);
    pPlayBtn->SetBitmap(wxBitmap("resource\\images\\play.png", wxBITMAP_TYPE_PNG) );

    wxButton* pStopBtn = new wxButton(this, idStopBtn, wxEmptyString, wxDefaultPosition, wxSize(25,25), wxBU_NOTEXT | wxBORDER_NONE);
    pStopBtn->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);
    pStopBtn->SetBitmap(wxBitmap("resource\\images\\stop.png", wxBITMAP_TYPE_PNG) );

    m_pRepeatBtn = new wxButton(this, idRepeatBtn, wxEmptyString, wxDefaultPosition, wxSize(25,25), wxBU_NOTEXT | wxBORDER_NONE);
    m_pRepeatBtn->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);
    m_pRepeatBtn->SetBitmap(wxBitmap("resource\\images\\repeat_off.png", wxBITMAP_TYPE_PNG) );
//    m_pRepeatBtn->SetBitmapDisabled(wxBitmap("resource\\images\\repeat_off.png", wxBITMAP_TYPE_PNG) );

    m_pShuffleBtn = new wxButton(this, idShuffleBtn, wxEmptyString, wxDefaultPosition, wxSize(25,25), wxBU_NOTEXT | wxBORDER_NONE);
    m_pShuffleBtn->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);
    m_pShuffleBtn->SetBitmap(wxBitmap("resource\\images\\shuffle_off.png", wxBITMAP_TYPE_PNG) );
//    m_pShuffleBtn->SetBitmapDisabled(wxBitmap("resource\\images\\shuffle_off.png", wxBITMAP_TYPE_PNG) );

    // media player displays
    m_pTimeElapsedDisplay = new cmsTimerCtrl(this, wxID_ANY, wxSize(50, 20), cmsTIMER_INCREASE);
    m_pTimeElapsedDisplay->SetMediaCtrlObject(m_MediaCtrl);

    m_pTrackInfoDisplay = new cmsScrollingTextCtrl(this, wxID_ANY, wxSize(100, 20) );

    m_pTimeRemainingDisplay = new cmsTimerCtrl(this, wxID_ANY, wxSize(50, 20), cmsTIMER_DECREASE);
    m_pTimeRemainingDisplay->SetMediaCtrlObject(m_MediaCtrl);

    // media player controls
    m_pStreamPositionSlider = new cmsSliderCtrl(this, idStreamPositionSlider, 0, 0, 1, wxDefaultPosition, wxDefaultSize, cmsSC_HORIZONTAL);

    m_pVolumeCtrl = new cmsVolumeCtrl(this, idVolumeCtrl, 10);

    // display sizer
    wxBoxSizer* pDisplaySizer = new wxBoxSizer(wxHORIZONTAL);
    pDisplaySizer->Add(m_pTimeElapsedDisplay, wxSizerFlags(1).Border(wxALL, 5) );
    pDisplaySizer->Add(m_pTrackInfoDisplay, wxSizerFlags(3).Border(wxALL, 5) );
    pDisplaySizer->Add(m_pTimeRemainingDisplay, wxSizerFlags(1).Border(wxALL, 5) );

    // button sizer
    wxBoxSizer* pBtnSizer = new wxBoxSizer(wxHORIZONTAL);
    pBtnSizer->Add(m_pRepeatBtn, wxSizerFlags(1).Border(wxALL, 5) );
    pBtnSizer->AddStretchSpacer(1);
    pBtnSizer->Add(pRevSeekBtn, wxSizerFlags(1).Border(wxALL, 5) );
    pBtnSizer->Add(pStopBtn, wxSizerFlags(1).Border(wxALL, 5) );
    pBtnSizer->Add(pPlayBtn, wxSizerFlags(1).Border(wxALL, 5) );
    pBtnSizer->Add(pForSeekBtn, wxSizerFlags(1).Border(wxALL, 5) );
    pBtnSizer->AddStretchSpacer(1);
    pBtnSizer->Add(m_pShuffleBtn, wxSizerFlags(1).Border(wxALL, 5) );

    // stream position sizer
    wxBoxSizer* pStreamPosSizer = new wxBoxSizer(wxHORIZONTAL);
    pStreamPosSizer->Add(m_pStreamPositionSlider, wxSizerFlags(1).Border(wxALL, 2) );

    // main sizer
    wxBoxSizer* pMainSizer = new wxBoxSizer(wxVERTICAL);
    pMainSizer->Add(pDisplaySizer, wxSizerFlags(1).Center() );
    pMainSizer->Add(pStreamPosSizer, wxSizerFlags(1).Expand() );
    pMainSizer->Add(pBtnSizer, wxSizerFlags(1).Centre() );
    pMainSizer->Add(m_pVolumeCtrl, wxSizerFlags(1).Centre() );
//    pMainSizer->AddStretchSpacer(3);

    this->SetSizer(pMainSizer);

    this->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);

    // bind event handlers
    m_MediaCtrl.Bind(wxEVT_MEDIA_LOADED, &CMusicPlayer::OnMediaLoaded, this, m_MediaCtrl.GetId() );
    m_MediaCtrl.Bind(wxEVT_MEDIA_STOP, &CMusicPlayer::OnMediaStoping, this, m_MediaCtrl.GetId() );
    m_MediaCtrl.Bind(wxEVT_MEDIA_FINISHED, &CMusicPlayer::OnMediaFinished, this, m_MediaCtrl.GetId() );
    m_MediaCtrl.Bind(wxEVT_MEDIA_STATECHANGED, &CMusicPlayer::OnMediaStateChanged, this, m_MediaCtrl.GetId() );
    m_MediaCtrl.Bind(wxEVT_MEDIA_PLAY, &CMusicPlayer::OnMediaPlay, this, m_MediaCtrl.GetId() );
    m_MediaCtrl.Bind(wxEVT_MEDIA_PAUSE, &CMusicPlayer::OnMediaPause, this, m_MediaCtrl.GetId() );
    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CMusicPlayer::OnRevSeekBtnClicked, this, idRevSeekBtn);
    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CMusicPlayer::OnStopBtnClicked, this, idStopBtn);
    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CMusicPlayer::OnPlayBtnClicked, this, idPlayBtn);
    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CMusicPlayer::OnForSeekBtnClicked, this, idForSeekBtn);
    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CMusicPlayer::OnRepeatBtnClicked, this, idRepeatBtn);
    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &CMusicPlayer::OnShuffleBtnClicked, this, idShuffleBtn);
    this->Bind(cmsEVT_SLIDER_VALUECHANGED, &CMusicPlayer::OnStreamPositionChange, this, idStreamPositionSlider);
    this->Bind(cmsEVT_SLIDER_TIMER, &CMusicPlayer::OnStreamPositionTimer, this, idStreamPositionSlider);

    return true;
}

wxMediaState CMusicPlayer::GetState()
{
    return m_MediaCtrl.GetState();
}

bool CMusicPlayer::Load(CSong* pSong)
{
    if (!m_IsOk)
    {
        return false;
    }
    m_pTrackInfoDisplay->SetText(pSong->GetTitle(), pSong->GetArtist(), pSong->GetAlbum() );
    return m_MediaCtrl.Load(pSong->GetFullPath() );
}

bool CMusicPlayer::Play()
{
    if (!m_IsOk)
    {
        return false;
    }
    wxButton* pBtn = (wxButton*)this->FindWindowById(idPlayBtn, this);
    pBtn->SetBitmap(wxBitmap("resource\\images\\pause.png", wxBITMAP_TYPE_PNG) );
    this->SetMediaCtrlVolume(m_pVolumeCtrl->GetVolume() );
    return m_MediaCtrl.Play();
}

bool CMusicPlayer::Pause()
{
    if (!m_IsOk)
    {
        return false;
    }
    wxButton* pBtn = (wxButton*)this->FindWindowById(idPlayBtn, this);
    pBtn->SetBitmap(wxBitmap("resource\\images\\play.png", wxBITMAP_TYPE_PNG) );
    return m_MediaCtrl.Pause();
}

bool CMusicPlayer::Stop()
{
    if (!m_IsOk)
    {
        return false;
    }

    m_pTrackInfoDisplay->ClearDisplay();
    m_pTimeElapsedDisplay->StopTimer();
    m_pTimeRemainingDisplay->StopTimer();
    m_pStreamPositionSlider->StopTimer();
    wxButton* pBtn = (wxButton*)this->FindWindowById(idPlayBtn, this);
    pBtn->SetBitmap(wxBitmap("resource\\images\\play.png", wxBITMAP_TYPE_PNG) );
    return m_MediaCtrl.Stop();
}

const int CMusicPlayer::GetMediaCtrlVolume()
{
    return m_MediaCtrl.GetVolume() * 10;
}

void CMusicPlayer::SetMediaCtrlVolume(const int Volume)
{
    m_MediaCtrl.SetVolume(Volume / 10.0f);
}

void CMusicPlayer::OnMediaLoaded(wxMediaEvent& Event)
{
    if (this->Play() )
    {
        long StartSeconds = (long)(m_MediaCtrl.Tell() / 1000);
        long EndSeconds = (long)(m_MediaCtrl.Length() / 1000);
        m_pTimeElapsedDisplay->StartTimer(StartSeconds);
        m_pTimeRemainingDisplay->StartTimer(EndSeconds);
        m_pStreamPositionSlider->StartTimer(EndSeconds);
    }
}

void CMusicPlayer::OnMediaStoping(wxMediaEvent& Event)
{
    cmsMediaPlayerEvent CustomEvent(cmsEVT_MEDIA_STOPPED, this->GetId() );
    CustomEvent.SetEventObject( (wxObject*)this);
    this->GetEventHandler()->ProcessEvent(CustomEvent);
}

void CMusicPlayer::OnMediaFinished(wxMediaEvent& Event)
{
    this->Stop();
    cmsMediaPlayerEvent CustomEvent(cmsEVT_MEDIA_FINISHED, this->GetId() );
    CustomEvent.SetEventObject( (wxObject*)this);
    this->GetEventHandler()->ProcessEvent(CustomEvent);
}

void CMusicPlayer::OnMediaStateChanged(wxMediaEvent& Event)
{

}

void CMusicPlayer::OnMediaPlay(wxMediaEvent& Event)
{

}

void CMusicPlayer::OnMediaPause(wxMediaEvent& Event)
{

}

void CMusicPlayer::OnRevSeekBtnClicked(wxCommandEvent& Event)
{
    long Pos = (long)m_MediaCtrl.Tell() / 1000;
    if (Pos > 3)
    {
        m_pStreamPositionSlider->ChangeValue(m_pStreamPositionSlider->GetMin() );
    }
//    else
//    {
//        long ItemId = m_Playlist.GetLastTrack();
//        if (ItemId != _INVALID_ITEM)
//        {
//            this->Load(ItemId);
//        }
//    }
}

void CMusicPlayer::OnStopBtnClicked(wxCommandEvent& Event)
{
    this->Stop();
}

void CMusicPlayer::OnPlayBtnClicked(wxCommandEvent& Event)
{
    if (m_IsOk)
    {
        switch (m_MediaCtrl.GetState() )
        {
            case wxMEDIASTATE_STOPPED:
            {
            }
            break;

            case wxMEDIASTATE_PLAYING:
            {
                this->Pause();
            }
            break;

            case wxMEDIASTATE_PAUSED:
            {
                this->Play();
            }
            break;
        }
    }
}

void CMusicPlayer::OnForSeekBtnClicked(wxCommandEvent& Event)
{
    this->Stop();
}

void CMusicPlayer::OnRepeatBtnClicked(wxCommandEvent& Event)
{
    m_IsRepeatOn = !m_IsRepeatOn;
    if (m_IsRepeatOn)
    {
        m_pRepeatBtn->SetBitmap(wxBitmap("resource\\images\\repeat_on.png", wxBITMAP_TYPE_PNG) );
        cmsMediaPlayerEvent Event(cmsEVT_REPEAT_ON, this->GetId() );
        Event.SetEventObject( (wxObject*)this);
        this->GetEventHandler()->ProcessEvent(Event);
    }
    else
    {
        m_pRepeatBtn->SetBitmap(wxBitmap("resource\\images\\repeat_off.png", wxBITMAP_TYPE_PNG) );
        cmsMediaPlayerEvent Event(cmsEVT_REPEAT_OFF, this->GetId() );
        Event.SetEventObject( (wxObject*)this);
        this->GetEventHandler()->ProcessEvent(Event);
    }
}

void CMusicPlayer::OnShuffleBtnClicked(wxCommandEvent& Event)
{
    m_IsShuffleOn = !m_IsShuffleOn;
    if (m_IsShuffleOn)
    {
        m_pShuffleBtn->SetBitmap(wxBitmap("resource\\images\\shuffle_on.png", wxBITMAP_TYPE_PNG) );
        cmsMediaPlayerEvent Event(cmsEVT_SHUFFLE_ON, this->GetId() );
        Event.SetEventObject( (wxObject*)this);
        this->GetEventHandler()->ProcessEvent(Event);
    }
    else
    {
        m_pShuffleBtn->SetBitmap(wxBitmap("resource\\images\\shuffle_off.png", wxBITMAP_TYPE_PNG) );
        cmsMediaPlayerEvent Event(cmsEVT_SHUFFLE_OFF, this->GetId() );
        Event.SetEventObject( (wxObject*)this);
        this->GetEventHandler()->ProcessEvent(Event);
    }
}

void CMusicPlayer::OnStreamPositionChange(cmsSliderEvent& Event)
{
    m_MediaCtrl.Seek(Event.GetPosition() * 1000);
}

void CMusicPlayer::OnStreamPositionTimer(cmsSliderEvent& Event)
{
    long pos = (m_MediaCtrl.Tell() );
    int time = pos / 1000;
    m_pStreamPositionSlider->SetValue(time);
}
