#ifndef APP_H_INCLUDED
#define APP_H_INCLUDED

#include <include/wxMSWHeaders.h>

class CMyMediaApp : public wxApp
{
    public:
        virtual bool OnInit();
};

wxDECLARE_APP(CMyMediaApp);

#endif // APP_H_INCLUDED
