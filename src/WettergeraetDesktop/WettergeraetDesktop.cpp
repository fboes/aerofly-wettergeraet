#include "WettergeraetDesktop.h"
#include "Frame.h"

IMPLEMENT_APP(WettergeraetDesktop)

bool WettergeraetDesktop::OnInit()
{
	Frame *frame = new Frame(wxT("Aerofly Wetterger\u00E4t"), wxApp::argc, wxApp::argv);
	frame->Show(true);

	return true;
}
