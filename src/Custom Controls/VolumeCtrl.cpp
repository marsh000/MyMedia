#include <include/Custom Controls/VolumeCtrl.h>
#include <include/Custom Controls/Media Players/MusicPlayer.h>
#include <include/Custom Controls/Media Players/MediaPlayerStyling.h>

cmsVolumeLevelDisplay::cmsVolumeLevelDisplay(cmsVolumeCtrl* pParent, wxWindowID ID, const wxPoint& Pos, const wxSize& Size)
: wxPanel(pParent, ID, Pos, Size), m_pParent(pParent)
{
    this->Bind(wxEVT_PAINT, &cmsVolumeLevelDisplay::OnPaint, this, ID);
    this->Bind(wxEVT_LEFT_DOWN, &cmsVolumeLevelDisplay::OnLeftClick, this, ID);
}

cmsVolumeLevelDisplay::~cmsVolumeLevelDisplay()
{

}

void cmsVolumeLevelDisplay::DrawVolumeControl(wxDC& DC)
{
    const int NumOfLines = 10;
    // set up the volume lines
    wxRect VolumeLines[NumOfLines] =
    {
        wxRect( 4, 18,  3,  2),
        wxRect(10, 16,  3,  4),
        wxRect(16, 14,  3,  6),
        wxRect(22, 12,  3,  8),
        wxRect(28, 10,  3, 10),
        wxRect(34,  8,  3, 12),
        wxRect(40,  6,  3, 14),
        wxRect(46,  4,  3, 16),
        wxRect(52,  2,  3, 18),
        wxRect(58,  0,  3, 20)
    };

    // draw the volume lines
    for (int x = 0; x < NumOfLines; x++)
    {
        if (x < m_pParent->GetVolume() )
        {
            DC.SetPen(wxPen(_MUSIC_CONTROL_FOREGROUND_COLOR) );
            DC.SetBrush(wxBrush(_MUSIC_CONTROL_FOREGROUND_COLOR) );
        }
        else
        {
            DC.SetPen(wxPen(_MUSIC_CONTROL_FOREGROUND_DISABLE_COLOR) );
            DC.SetBrush(wxBrush(_MUSIC_CONTROL_FOREGROUND_DISABLE_COLOR) );
        }
        DC.DrawRectangle(VolumeLines[x] );
    }
}

int cmsVolumeLevelDisplay::HitTest(wxPoint& Point)
{
    const int NumOfLines = 10;
    // set up the volume lines
    wxRect VolumeLines[NumOfLines] =
    {
        wxRect( 3,  0,  6, 20),
        wxRect( 9,  0,  6, 20),
        wxRect(15,  0,  6, 20),
        wxRect(21,  0,  6, 20),
        wxRect(27,  0,  6, 20),
        wxRect(33,  0,  6, 20),
        wxRect(39,  0,  6, 20),
        wxRect(45,  0,  6, 20),
        wxRect(51,  0,  6, 20),
        wxRect(57,  0,  6, 20)
    };

    for (int x = 0; x < NumOfLines; x++)
    {
        if (VolumeLines[x].Contains(Point) )
        {
            return x + 1;
        }
    }
    return 0;
}

void cmsVolumeLevelDisplay::OnPaint(wxPaintEvent& Event)
{
    wxPaintDC DC(this);
    this->DrawVolumeControl(DC);
}

void cmsVolumeLevelDisplay::OnLeftClick(wxMouseEvent& Event)
{
    // determine which, if any, volume line was clicked on
    int LineNum = this->HitTest(Event.GetPosition() );
    if (LineNum > 0)
    {
        m_pParent->SetVolume(LineNum);
    }
    Event.Skip();
}





cmsVolumeCtrl::cmsVolumeCtrl(CMusicPlayer* pParent, wxWindowID ID, int Volume)
: wxPanel(pParent, ID, wxDefaultPosition, wxDefaultSize),
  m_pParent(pParent), m_IsMuted(false)
{
    this->SetVolume(Volume);
    this->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);

    m_pMuteBtn = new wxButton(this, idMuteBtn, wxEmptyString, wxDefaultPosition, wxSize(20,20), wxBU_NOTEXT | wxBORDER_NONE);
    m_pMuteBtn->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);
    m_pMuteBtn->SetBitmapDisabled(wxBitmap("resource\\images\\mute_disable.png", wxBITMAP_TYPE_PNG) );
    this->SetMuteButtonImage(false);

    wxButton* pVolumeUpBtn = new wxButton(this, idVolumeUpBtn, wxEmptyString, wxDefaultPosition, wxSize(20,20), wxBU_NOTEXT | wxBORDER_NONE);
    pVolumeUpBtn->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);
    pVolumeUpBtn->SetBitmap(wxBitmap("resource\\images\\plus.png", wxBITMAP_TYPE_PNG) );

    m_pVolumePanel = new cmsVolumeLevelDisplay(this, idVolumePanel, wxDefaultPosition, wxSize(65, 20) );

    wxButton* pVolumeDownBtn = new wxButton(this, idVolumeDownBtn, wxEmptyString, wxDefaultPosition, wxSize(20,20), wxBU_NOTEXT | wxBORDER_NONE);
    pVolumeDownBtn->SetBackgroundColour(_MUSIC_PLAYER_BACKGROUND_COLOR);
    pVolumeDownBtn->SetBitmap(wxBitmap("resource\\images\\minus.png", wxBITMAP_TYPE_PNG) );

    wxBoxSizer* pVolSizer = new wxBoxSizer(wxHORIZONTAL);
    pVolSizer->Add(m_pMuteBtn, wxSizerFlags(0).Border(wxALL, 5) );
    pVolSizer->Add(pVolumeDownBtn, wxSizerFlags(0).Border(wxALL, 5) );
    pVolSizer->Add(m_pVolumePanel, wxSizerFlags(0).Border(wxALL, 5) );
    pVolSizer->Add(pVolumeUpBtn, wxSizerFlags(0).Border(wxALL, 5) );

    this->SetSizer(pVolSizer);

    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cmsVolumeCtrl::OnMuteBtnClicked, this, idMuteBtn);
    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cmsVolumeCtrl::OnVolumeUpBtnClicked, this, idVolumeUpBtn);
    this->Bind(wxEVT_COMMAND_BUTTON_CLICKED, &cmsVolumeCtrl::OnVolumeDownBtnClicked, this, idVolumeDownBtn);
}

cmsVolumeCtrl::~cmsVolumeCtrl()
{

}

void cmsVolumeCtrl::SetVolume(int Volume)
{
    wxASSERT_MSG(Volume > 0 && Volume <= 10, "Volume must be in the range of 1 - 10");
    m_Volume = Volume;
    if (!m_IsMuted)
    {
        m_pParent->SetMediaCtrlVolume(Volume);
    }
    this->Refresh();
}

int cmsVolumeCtrl::GetVolume() const
{
    return m_Volume;
}

void cmsVolumeCtrl::SetMuteButtonImage(bool IsMuted)
{
    if (IsMuted)
    {
        m_pMuteBtn->SetBitmap(wxBitmap("resource\\images\\mute_on.png", wxBITMAP_TYPE_PNG) );
    }
    else
    {
        m_pMuteBtn->SetBitmap(wxBitmap("resource\\images\\mute_off.png", wxBITMAP_TYPE_PNG) );
    }
}

void cmsVolumeCtrl::OnMuteBtnClicked(wxCommandEvent& Event)
{
    m_IsMuted = !m_IsMuted;
    if (m_IsMuted)
    {
        m_pParent->SetMediaCtrlVolume(0);
        this->SetMuteButtonImage(true);
    }
    else
    {
        m_pParent->SetMediaCtrlVolume(m_Volume);
        this->SetMuteButtonImage(false);
    }
}

void cmsVolumeCtrl::OnVolumeUpBtnClicked(wxCommandEvent& Event)
{
    if (m_Volume >= 10)
        return;

    m_Volume++;
    this->SetVolume(m_Volume);
}

void cmsVolumeCtrl::OnVolumeDownBtnClicked(wxCommandEvent& Event)
{
    if (m_Volume <= 1)
        return;

    m_Volume --;
    this->SetVolume(m_Volume);
}

