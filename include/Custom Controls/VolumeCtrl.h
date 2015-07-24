#ifndef VOLUMECTRL_H_INCLUDED
#define VOLUMECTRL_H_INCLUDED

#include <include/wxMSWHeaders.h>

class CMusicPlayer;
class cmsVolumeCtrl;

class cmsVolumeLevelDisplay : public wxPanel
{
public:
    cmsVolumeLevelDisplay(cmsVolumeCtrl* pParent, wxWindowID ID, const wxPoint& Pos = wxDefaultPosition, const wxSize& Size = wxDefaultSize);
    ~cmsVolumeLevelDisplay();

private:
    void DrawVolumeControl(wxDC& DC);
    int HitTest(wxPoint& Point);

    void OnPaint(wxPaintEvent& Event);
    void OnLeftClick(wxMouseEvent& Event);

    cmsVolumeCtrl*                m_pParent;
};

class cmsVolumeCtrl : public wxPanel
{
public:
    cmsVolumeCtrl(CMusicPlayer* pParent, wxWindowID ID, int Volume);
    ~cmsVolumeCtrl();
    void SetVolume(int Volume);
    int GetVolume() const;

protected:
    enum
    {
        idMuteBtn = 1200,
        idVolumePanel,
        idVolumeUpBtn,
        idVolumeDownBtn,
    };

private:
    void SetMuteButtonImage(bool IsMuted);

    void OnMuteBtnClicked(wxCommandEvent& Event);
    void OnVolumeUpBtnClicked(wxCommandEvent& Event);
    void OnVolumeDownBtnClicked(wxCommandEvent& Event);

    CMusicPlayer*                 m_pParent;
    cmsVolumeLevelDisplay*        m_pVolumePanel;
    wxButton*                     m_pMuteBtn;
    int                           m_Volume;
    bool                          m_IsMuted;
};

#endif // VOLUMECTRL_H_INCLUDED
