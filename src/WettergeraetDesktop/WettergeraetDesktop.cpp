#include "WettergeraetDesktop.h"
#include "Frame.h"

IMPLEMENT_APP(WettergeraetDesktop)

bool WettergeraetDesktop::OnInit()
{
	this->m_checker = new wxSingleInstanceChecker;
	if (this->m_checker->IsAnotherRunning())
	{
		wxLogError(_("Another program instance is already running, aborting."));
		delete this->m_checker; // OnExit() won't be called if we return false
		this->m_checker = NULL;
		return false;
	}

	Frame *frame = new Frame(wxT("Aerofly Wetterger\u00E4t"), wxApp::argc, wxApp::argv);
	frame->Show(true);

	return true;
}

int WettergeraetDesktop::OnExit()
{
	delete this->m_checker;
	return 0;
}
