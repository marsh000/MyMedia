#ifndef SLIDERCTRL_H_INCLUDED
#define SLIDERCTRL_H_INCLUDED

#include <include/wxMSWHeaders.h>

#define _THUMBWIDTH 11
#define _THUMBHEIGHT 19
#define _THUMBSIZE wxSize(11,19)

enum
{
    cmsSC_HORIZONTAL    = 1,
    cmsSC_VERTICAL      = 1 << 1,
    cmsSC_TICKS         = 1 << 2,
    cmsSC_MINMAX_LABELS = 1 << 3,
    cmsSC_VALUE_LABELS  = 1 << 4,
    cmsSC_ALL_LABELS    = cmsSC_MINMAX_LABELS | cmsSC_VALUE_LABELS,
    cmsSC_TICKS_LEFT    = (1 << 5) | cmsSC_TICKS,
    cmsSC_TICKS_RIGHT   = (1 << 6) | cmsSC_TICKS,
    cmsSC_TICKS_TOP     = (1 << 7) | cmsSC_TICKS,
    cmsSC_TICKS_BOTTOM  = (1 << 8) | cmsSC_TICKS,
    cmsSC_DEFAULT       = cmsSC_HORIZONTAL
};

class cmsSliderTimer;

class cmsSliderCtrl : public wxControl
{
public:
    cmsSliderCtrl();
    cmsSliderCtrl(wxWindow* pParent, wxWindowID ID, int Value, int MinValue, int MaxValue, const wxPoint& Pos = wxDefaultPosition, const wxSize& Size = wxDefaultSize, long Style = cmsSC_HORIZONTAL, const wxValidator& Validator = wxDefaultValidator, const wxString& Name = "customslider");
    ~cmsSliderCtrl();
    void ChangeValue(int Value);
    void ClearTicks();
    bool Create(wxWindow* pParent, wxWindowID ID, int Value, int MinValue, int MaxValue, const wxPoint& Pos = wxDefaultPosition, const wxSize& Size = wxDefaultSize, long Style = cmsSC_HORIZONTAL, const wxValidator& Validator = wxDefaultValidator, const wxString& Name = "customslider");
    void StartTimer(long LengthInSeconds);
    void StepValue(bool Forward = true);
    void StopTimer();

    wxRect GetLineRect();
    int GetMax() const;
    int GetMin() const;
    wxRect GetThumbRect();
    int GetTickFreq() const;
    int GetValue() const;

    void SetMax(int MaxValue);
    void SetMin(int MinValue);
    void SetRange(int MinValue, int MaxValue);
    void SetTickFreq(int Freq);
    void SetValue(int Value);

protected:
    void OnPaint(wxPaintEvent& Event);
    void OnMouseLeftDown(wxMouseEvent& Event);
    void OnMouseMoving(wxMouseEvent& Event);
    void OnMouseLeftUp(wxMouseEvent& Event);
    void OnLostMouseFocus(wxMouseEvent& Event);

private:
    void Initialize();
    void InitializeThumb();
    double GetXInterval();
    void RenderControl(wxDC& DC);
    int TranslateXToValue(const int XX);
    int TranslateValueToX(int* pValue = NULL);
    wxPoint TranslateValueToThumbPoint();
    virtual wxSize DoGetBestClientSize() const;

    wxWindow*                     m_pParent;
    long                          m_Style;
    cmsSliderTimer*               m_pTimer;
    int                           m_MinValue;
    int                           m_MaxValue;
    int                           m_Value;
    int                           m_TickFreq;
    wxBitmap                      m_ThumbImageOn;
    wxBitmap                      m_ThumbImageOff;
    wxBitmap*                     m_pCurrentThumbImage;
    bool                          m_CanDrag;

    wxDECLARE_DYNAMIC_CLASS(cmsSliderCtrl);
};

class cmsSliderTimer : public wxTimer
{
public:
    cmsSliderTimer(wxWindow* pOwner, wxWindowID ID);
    ~cmsSliderTimer();
    void Notify();

private:
    wxWindow*                     m_pOwner;
};

class cmsSliderEvent : public wxCommandEvent
{
public:
	cmsSliderEvent(wxEventType EventType, int ID, int Position = -1);
	~cmsSliderEvent();
	virtual cmsSliderEvent* Clone() const;
	int GetPosition() const;

private:
    int                      m_Position;
};

// define event types associated with this class for use with Bind<>()
wxDECLARE_EVENT(cmsEVT_SLIDER_VALUECHANGED, cmsSliderEvent);
wxDECLARE_EVENT(cmsEVT_SLIDER_TIMER, cmsSliderEvent);

// define macro for creating the event table entries for each event type
#define CMS_EVT_SLIDER_VALUECHANGED(func, id) wx__DECLARE_EVT1(cmsEVT_SLIDER_VALUECHANGED, id, &func)
#define CMS_EVT_SLIDER_TIMER(func, id) wx__DECLARE_EVT1(cmsEVT_SLIDER_TIMER, id, &func)

#endif // SLIDERCTRL_H_INCLUDED
