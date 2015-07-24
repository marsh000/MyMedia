#include <include/App.h>
#include <include/Main.h>

wxIMPLEMENT_APP(CMyMediaApp);

bool CMyMediaApp::OnInit()
{
	bool wxsOK = true;

	// seed random number generator
	srand(time(NULL) );

    wxInitAllImageHandlers();

    CMyMediaFrame* frame = new CMyMediaFrame();
    frame->SetIcon(wxICON(aaaa)); // To Set App Icon
    frame->Maximize();
    frame->Show();

	return wxsOK;
}
