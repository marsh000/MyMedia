#ifndef ERRORLOG_H_INCLUDED
#define ERRORLOG_H_INCLUDED

#include <include/wxMSWHeaders.h>

#define cmsShowMessage(x) wxMessageBox(x)

#ifdef _DEBUG
 #define cmsInitDebugWindow \
     cmsPrivate::g_pDebugWindow = new wxTextCtrl(this, \
                                                 wxID_ANY, \
                                                 wxEmptyString, \
                                                 wxDefaultPosition, \
                                                 wxDefaultSize, \
                                                 wxTE_MULTILINE | wxTE_READONLY | wxTE_BESTWRAP)

 #define cmsDebugWindow cmsPrivate::g_pDebugWindow

 #define cmsDebugMessage(s) \
 { \
     cmsPrivate::g_pDebugWindow->AppendText(s); \
     cmsPrivate::g_pDebugWindow->AppendText("\n\n"); \
 }

 #define cmsLogError(s) \
 { \
     wxString ErrStr = s; \
     ErrStr << "\nFile: " << __FILE__ << "\nLine: " << __LINE__; \
     wxMessageBox(ErrStr, "Error", wxOK | wxCENTRE | wxICON_ERROR, NULL); \
 }
#else
 #define cmsInitDebugWindow
 #define cmsDebugWindow
 #define cmsDebugMessage(s)

 #define cmsLogError(s) \
 { \
     wxString ErrStr = s; \
     ErrStr << "\nFile: " << __FILE__ << "\nLine: " << __LINE__; \
     wxMessageBox(ErrStr, "Error", wxOK | wxCENTRE | wxICON_ERROR, NULL); \
 }
#endif // _DEBUG

namespace cmsPrivate
{
    extern wxTextCtrl* g_pDebugWindow;
}

#endif // ERRORLOG_H_INCLUDED
