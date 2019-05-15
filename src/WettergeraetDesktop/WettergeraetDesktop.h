#pragma once

#include <wx/wx.h>
#include <wx/snglinst.h>

class WettergeraetDesktop : public wxApp
{
private:
	wxSingleInstanceChecker *m_checker;
public:
	virtual bool OnInit() override;
	virtual int OnExit() override;

};
