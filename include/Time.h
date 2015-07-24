#ifndef TIME_H_INCLUDED
#define TIME_H_INCLUDED

#include <include/wxMSWHeaders.h>

class CTime
{
public:
    CTime(const int Sec = 0)
    {
        m_Minutes = ( (Sec - (Sec % 60) ) / 60);
        m_Seconds = (Sec % 60);
    }

    ~CTime()
    {
    }

    wxString GetTimeStr()
    {
        return wxString::Format("%i:%02i", m_Minutes, m_Seconds);
    }

    int GetMinutes()
    {
        return m_Minutes;
    }

    int GetSeconds()
    {
        return m_Seconds;
    }

    void SetTimeStr(wxString x)
    {
        m_Minutes = 0;
        m_Seconds = 0;
        wxString First, Last;
        First = x.BeforeFirst(':', &Last);
        if (First != x)
        {
//            First.ToLong(static_cast<long*)&m_Minutes) );
//            Last.ToLong(static_cast<long*>(&m_Seconds) );
            long Min = 0, Sec = 0;
            First.ToLong(&Min);
            Last.ToLong(&Sec);
            m_Minutes = Min;
            m_Seconds = Sec;
        }
    }

private:
    int                      m_Minutes;
    int                      m_Seconds;
};

#endif // TIME_H_INCLUDED
