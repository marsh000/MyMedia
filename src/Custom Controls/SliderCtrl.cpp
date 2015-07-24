#include <include/Custom Controls/SliderCtrl.h>

wxDEFINE_EVENT(cmsEVT_SLIDER_VALUECHANGED, cmsSliderEvent);
wxDEFINE_EVENT(cmsEVT_SLIDER_TIMER, cmsSliderEvent);

cmsSliderEvent::cmsSliderEvent(wxEventType EventType, int ID, int Position)
{
    this->SetEventType(EventType);
    this->SetId(ID);
    m_Position = Position;
}

cmsSliderEvent* cmsSliderEvent::Clone() const
{
    return new cmsSliderEvent(*this);
}

cmsSliderEvent::~cmsSliderEvent()
{

}

int cmsSliderEvent::GetPosition() const
{
    return m_Position;
}



cmsSliderTimer::cmsSliderTimer(wxWindow* pOwner, wxWindowID ID)
: wxTimer(pOwner, ID), m_pOwner(pOwner)
{

}

cmsSliderTimer::~cmsSliderTimer()
{

}

void cmsSliderTimer::Notify()
{
    cmsSliderEvent Event(cmsEVT_SLIDER_TIMER, m_pOwner->GetId() );
    Event.SetEventObject(this);
    m_pOwner->ProcessWindowEvent(Event);
}



wxIMPLEMENT_DYNAMIC_CLASS(cmsSliderCtrl, wxControl);

cmsSliderCtrl::cmsSliderCtrl()
{
    Initialize();
}

cmsSliderCtrl::cmsSliderCtrl(wxWindow* pParent, wxWindowID ID, int Value, int MinValue, int MaxValue, const wxPoint& Pos, const wxSize& Size, long Style, const wxValidator& Validator, const wxString& Name)
{
    this->Initialize();
    this->Create(pParent, ID, Value, MinValue, MaxValue, Pos, Size, Style, Validator, Name);
}

cmsSliderCtrl::~cmsSliderCtrl()
{
    delete m_pTimer;
}

void cmsSliderCtrl::ChangeValue(int Value)
{
    m_Value = Value;
    this->Refresh();

    cmsSliderEvent Event(cmsEVT_SLIDER_VALUECHANGED, this->GetId(), Value);
    Event.SetEventObject(this);
    this->ProcessWindowEvent(Event);
}

void cmsSliderCtrl::ClearTicks()
{

}

bool cmsSliderCtrl::Create(wxWindow* pParent, wxWindowID ID, int Value, int MinValue, int MaxValue, const wxPoint& Pos, const wxSize& Size, long Style, const wxValidator& Validator, const wxString& Name)
{
    m_pParent = pParent;
    m_Style = Style;
    wxControl::Create(pParent, ID, Pos, Size, wxNO_BORDER, Validator, Name);
    this->SetBackgroundStyle(wxBG_STYLE_PAINT);

    this->SetMin(MinValue);
    this->SetMax(MaxValue);
    this->SetValue(Value);

    this->InitializeThumb();

    m_pTimer = new cmsSliderTimer(this, wxID_ANY);

    this->Bind(wxEVT_PAINT, &cmsSliderCtrl::OnPaint, this, ID);
    this->Bind(wxEVT_LEFT_DOWN, &cmsSliderCtrl::OnMouseLeftDown, this, ID);
    this->Bind(wxEVT_MOTION, &cmsSliderCtrl::OnMouseMoving, this, ID);
    this->Bind(wxEVT_LEFT_UP, &cmsSliderCtrl::OnMouseLeftUp, this, ID);
    this->Bind(wxEVT_LEAVE_WINDOW, &cmsSliderCtrl::OnLostMouseFocus, this, ID);

    return true;
}

void cmsSliderCtrl::StartTimer(long LengthInSeconds)
{
    if (m_pTimer->IsRunning() )
    {
        m_pTimer->Stop();
        this->SetValue(0);
    }

    this->SetRange(0, LengthInSeconds - 1);
    this->SetValue(0);
    m_pTimer->Start(1000);
}

void cmsSliderCtrl::StepValue(bool Forward)
{
    if (Forward)
    {
        if (m_Value < m_MaxValue)
        {
            m_Value++;
        }
    }
    else
    {
        if (m_Value > m_MinValue)
        {
            m_Value--;
        }
    }
}

void cmsSliderCtrl::StopTimer()
{
    if (m_pTimer->IsRunning() )
    {
        m_pTimer->Stop();
        this->SetRange(0, 1);
        this->SetValue(0);
    }
}

wxRect cmsSliderCtrl::GetLineRect()
{
    wxSize ClientSize = this->GetClientSize();
    return wxRect(0 + (_THUMBWIDTH / 2),
                  (ClientSize.GetY() / 2) - 3,
                  (ClientSize.GetX() ) - (_THUMBWIDTH),
                  6);
}

int cmsSliderCtrl::GetMax() const
{
    return m_MaxValue;
}

int cmsSliderCtrl::GetMin() const
{
    return m_MinValue;
}

wxRect cmsSliderCtrl::GetThumbRect()
{
    return wxRect(this->TranslateValueToThumbPoint(), wxSize(_THUMBWIDTH, _THUMBHEIGHT) );
}

int cmsSliderCtrl::GetTickFreq() const
{
    return m_TickFreq;
}

int cmsSliderCtrl::GetValue() const
{
    return m_Value;
}

void cmsSliderCtrl::SetMax(int MaxValue)
{
    m_MaxValue = MaxValue;
}

void cmsSliderCtrl::SetMin(int MinValue)
{
    m_MinValue = MinValue;
}

void cmsSliderCtrl::SetRange(int MinValue, int MaxValue)
{
    this->SetMin(MinValue);
    this->SetMax(MaxValue);
}

void cmsSliderCtrl::SetTickFreq(int Freq)
{
    m_TickFreq = Freq;
}

void cmsSliderCtrl::SetValue(int Value)
{
    m_Value = Value;
    this->Refresh();
}

void cmsSliderCtrl::OnPaint(wxPaintEvent& Event)
{
    wxAutoBufferedPaintDC DC(this);
    this->RenderControl(DC);
    Event.Skip();
}

void cmsSliderCtrl::OnMouseLeftDown(wxMouseEvent& Event)
{
    if (this->GetThumbRect().Contains(Event.GetPosition() ) )
    {
        m_CanDrag = true;
    }
    else if (this->GetLineRect().Contains(Event.GetPosition() ) )
    {
        this->ChangeValue(this->TranslateXToValue(Event.GetX() ) );
    }
}

void cmsSliderCtrl::OnMouseMoving(wxMouseEvent& Event)
{
    if (m_CanDrag)
    {
        this->ChangeValue(this->TranslateXToValue(Event.GetX() ) );
    }
}

void cmsSliderCtrl::OnMouseLeftUp(wxMouseEvent& Event)
{
    if (m_CanDrag)
    {
        m_CanDrag = false;
    }
}

void cmsSliderCtrl::OnLostMouseFocus(wxMouseEvent& Event)
{
    m_CanDrag = false;
}

void cmsSliderCtrl::Initialize()
{
    m_CanDrag = false;
    m_TickFreq = 10;
}

void cmsSliderCtrl::InitializeThumb()
{
    m_ThumbImageOn.LoadFile("resource\\images\\sliderthumb_on.png", wxBITMAP_TYPE_PNG);
    m_ThumbImageOff.LoadFile("resource\\images\\sliderthumb_off.png", wxBITMAP_TYPE_PNG);
    m_pCurrentThumbImage = &m_ThumbImageOn;
}

double cmsSliderCtrl::GetXInterval()
{
    return static_cast<double>(this->GetLineRect().GetWidth() ) / static_cast<double>(this->GetMax() - this->GetMin() );
}

void cmsSliderCtrl::RenderControl(wxDC& DC)
{
    // get window client size and current thumb size
    wxSize ClientSize = this->GetClientSize();

    // set the background color
    DC.SetBackground(wxBrush(m_pParent->GetBackgroundColour() ) );
    DC.Clear();

    // draw the slider bar
    wxRect SliderBar = this->GetLineRect();
    DC.SetPen(wxPen(wxColor(100, 100, 100) ) );
    DC.SetBrush(wxBrush(wxColor(100, 100, 100) ) );
    DC.DrawRectangle(SliderBar.GetX() - (_THUMBWIDTH / 2),
                     SliderBar.GetY(),
                     SliderBar.GetWidth() + (_THUMBWIDTH),
                     SliderBar.GetHeight() );
    DC.SetBrush(wxBrush(wxColor(0, 0, 0) ) );
    DC.DrawRectangle(SliderBar.GetX(),
                     SliderBar.GetY() + 1,
                     SliderBar.GetWidth(),
                     SliderBar.GetHeight() - 2);

    // draw ticks
    if (m_Style & cmsSC_TICKS)
    {
        int TickPos = m_MinValue;
        for (TickPos; TickPos < m_MaxValue;)
        {
            DC.SetPen(wxPen(wxColor(100, 100, 100) ) );
            DC.DrawLine(this->TranslateValueToX(&TickPos),
                        SliderBar.GetY() - 13,
                        this->TranslateValueToX(&TickPos),
                        SliderBar.GetY() - 3);
            TickPos += m_TickFreq;
        }
        TickPos = m_MaxValue;
        DC.DrawLine(this->TranslateValueToX(&TickPos),
                    SliderBar.GetY() - 13,
                    this->TranslateValueToX(&TickPos),
                    SliderBar.GetY() - 3);
    }

    // draw

    // draw the thumb
    DC.SetPen(wxPen(wxColor(255, 0, 0) ) );
//    DC.DrawLine(this->TranslateValueToX(),
//                (ClientSize.GetY() / 2) - 10,
//                this->TranslateValueToX(),
//                20);
    wxPoint ThumbPosition(this->TranslateValueToX(), (ClientSize.GetY() / 2) - (_THUMBHEIGHT / 2) );
    DC.DrawBitmap(*m_pCurrentThumbImage, this->TranslateValueToThumbPoint() );
}

int cmsSliderCtrl::TranslateXToValue(const int XX)
{
    for (int x = this->GetMin(); x <= this->GetMax(); x++)
    {
        if (static_cast<double>(XX) < (this->GetXInterval() * x) + (this->GetXInterval() / 2) )
        {
            return x;
        }
    }
    return this->GetMin();
}

int cmsSliderCtrl::TranslateValueToX(int* pValue)
{
    if (pValue)
    {
        if (*pValue < m_MinValue)
        {
            return m_MinValue;
        }
        if (*pValue > m_MaxValue)
        {
            return m_MaxValue;
        }
    }
    int Val = (pValue) ? *pValue : this->GetValue();
    double Interval = GetXInterval();
    int ZeroBaseValue = Val - this->GetMin();
    return (int)( (double)this->GetLineRect().GetX() + ( (double)ZeroBaseValue * Interval) );
}

wxPoint cmsSliderCtrl::TranslateValueToThumbPoint()
{
    int x = (this->TranslateValueToX() - (_THUMBWIDTH / 2) );
    int y = ( (this->GetClientSize().GetY() / 2) - (_THUMBHEIGHT / 2) ) - 1;
    return wxPoint(x, y);
}

wxSize cmsSliderCtrl::DoGetBestClientSize() const
{
    return wxSize(175, 25);
}

