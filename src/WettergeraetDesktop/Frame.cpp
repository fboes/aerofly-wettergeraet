#define wxUSE_DATEPICKCTRL 1
#define MY_APP_VERSION_STRING "1.0.0"

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/aboutdlg.h>
#include <wx/icon.h>
#include <math.h>
#include "Frame.h"
#include "Argumentor.h"
#include "AeroflyConfigFile.h"

Frame::Frame(const wxString& title, int argc, char * argv[]) : wxFrame(nullptr, wxID_ANY, title, wxPoint(-1, -1), wxSize(640, 480))
{
	this->utcDateValue.SetToCurrent();
	this->SetIcon(wxICON(APPICON));
	this->argumentor.getArgs(argc, argv);

	wxMenuBar *menubar = new wxMenuBar;
	{
		wxMenu *file = new wxMenu;
		file->Append(wxID_OPEN, wxT("&Load 'main.mcf'"));
		file->Append(wxID_EXIT, wxT("&Exit"));
		menubar->Append(file, wxT("&File"));

		wxMenu *help = new wxMenu;
		help->Append(wxID_ABOUT, wxT("&About"));
		menubar->Append(help, wxT("&Help"));
	}
	SetMenuBar(menubar);


	const int labelBorder = 8;

	wxPanel *panel = new wxPanel(this, wxID_ANY);

	// build vbox
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	{
		// build hbox1
		wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *icaoLabel = new wxStaticText(panel, wxID_ANY, wxT("ICAO code"));
			hbox1->Add(icaoLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			this->icaoInput = new wxTextCtrl(panel, wxID_ANY);
			hbox1->Add(this->icaoInput, 2, wxALIGN_CENTER_VERTICAL);

			hbox1->Add(10, -1);

			wxButton *fetchButton = new wxButton(panel, Frame::EL_BUTTON_FETCH, wxT("Fetch METAR data"), wxPoint(5, 5));
			fetchButton->SetFocus();
			hbox1->Add(fetchButton, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox1, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 10);
		vbox->Add(-1, 10);

		// build hbox2
		wxBoxSizer *hbox2 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *metarLabel = new wxStaticText(panel, wxID_ANY, wxT("METAR"));
			hbox2->Add(metarLabel, 1, wxRIGHT, labelBorder);

			this->metarInput = new wxTextCtrl(panel, Frame::EL_CTRL_METAR, wxT(""), wxPoint(-1, -1), wxSize(-1, -1), wxTE_MULTILINE);
			hbox2->Add(this->metarInput, 3, wxLEFT | wxEXPAND);

			hbox2->Add(10, -1);

			/*wxButton *parseButton = new wxButton(panel, Frame::EL_BUTTON_PARSE, wxT("Convert METAR data"), wxPoint(5, 5));
			hbox2->Add(parseButton, 1, wxLEFT);*/
		}
		vbox->Add(hbox2, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);

		// build hbox3
		wxBoxSizer *hbox3 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *utcTimeLabel = new wxStaticText(panel, wxID_ANY, wxT("Time (UTC)"));
			hbox3->Add(utcTimeLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			wxDateTime utcDateValue = wxDateTime::Now();
			this->utcTimeInput = new wxTimePickerCtrl(panel, wxID_ANY, utcDateValue);
			hbox3->Add(this->utcTimeInput, 1, wxALIGN_CENTER_VERTICAL);

			hbox3->Add(10, -1);

			wxStaticText *utcDateLabel = new wxStaticText(panel, wxID_ANY, wxT("Date (UTC)"));
			hbox3->Add(utcDateLabel, 1, wxALIGN_CENTER_VERTICAL, labelBorder);

			this->utcDateInput = new wxDatePickerCtrl(panel, wxID_ANY, utcDateValue);
			hbox3->Add(this->utcDateInput, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox3, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);

		// build hbox8
		for (int i = 0; i < 3; ++i) {
			std::string cloudName = "";
			switch (i) {
			case 1:
				cloudName = "Cumulus";
				break;
			case 2:
				cloudName = "Cumulus med";
				break;
			default:
				cloudName = "Cirrus";
				break;
			}
			wxBoxSizer *hbox8 = new wxBoxSizer(wxHORIZONTAL);
			{
				wxStaticText *clouds0HeightLabel = new wxStaticText(panel, wxID_ANY, cloudName + " height (%)");
				hbox8->Add(clouds0HeightLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

				this->clouds[i].heightInput = new wxSlider(panel, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
				hbox8->Add(this->clouds[i].heightInput, 1, wxALIGN_CENTER_VERTICAL);

				hbox8->Add(10, -1);

				wxStaticText *clouds0DensityLabel = new wxStaticText(panel, wxID_ANY, cloudName + " density (%)");
				hbox8->Add(clouds0DensityLabel, 1, wxALIGN_CENTER_VERTICAL, labelBorder);

				this->clouds[i].densityInput = new wxSlider(panel, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
				hbox8->Add(this->clouds[i].densityInput, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
			}
			vbox->Add(hbox8, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
			vbox->Add(-1, 10);
		}

		// build hbox5
		wxBoxSizer *hbox5 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *windDirectionLabel = new wxStaticText(panel, wxID_ANY, wxT("Wind direction (\u00B0)"));
			hbox5->Add(windDirectionLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			this->windDirectionInput = new wxSlider(panel, wxID_ANY, 180, 0, 359, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
			hbox5->Add(this->windDirectionInput, 1, wxALIGN_CENTER_VERTICAL);

			hbox5->Add(10, -1);

			wxStaticText *windStrengthLabel = new wxStaticText(panel, wxID_ANY, wxT("Wind strength (%)"));
			hbox5->Add(windStrengthLabel, 1, wxALIGN_CENTER_VERTICAL, labelBorder);

			this->windStrengthInput = new wxSlider(panel, wxID_ANY, 50, 0, 200, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
			hbox5->Add(this->windStrengthInput, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox5, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);

		// build hbox6
		wxBoxSizer *hbox6 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *windTurbulenceLabel = new wxStaticText(panel, wxID_ANY, wxT("Turbulence (%)"));
			hbox6->Add(windTurbulenceLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			this->windTurbulenceInput = new wxSlider(panel, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
			hbox6->Add(this->windTurbulenceInput, 1, wxALIGN_CENTER_VERTICAL);

			hbox6->Add(10, -1);

			wxStaticText *thermalActivityLabel = new wxStaticText(panel, wxID_ANY, wxT("Thermal activity (%)"));
			hbox6->Add(thermalActivityLabel, 1, wxALIGN_CENTER_VERTICAL, labelBorder);

			this->thermalActivityInput = new wxSlider(panel, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
			hbox6->Add(this->thermalActivityInput, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox6, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);

		// build hbox7
		wxBoxSizer *hbox7 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *visbilityLabel = new wxStaticText(panel, wxID_ANY, wxT("Visibility (%)"));
			hbox7->Add(visbilityLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			this->visbilityInput = new wxSlider(panel, wxID_ANY, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
			hbox7->Add(this->visbilityInput, 1, wxALIGN_CENTER_VERTICAL);

			hbox7->Add(10, -1);

			wxStaticText *noLabel = new wxStaticText(panel, wxID_ANY, wxT(""));
			hbox7->Add(noLabel, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);

			this->saveButton = new wxButton(panel, wxID_SAVE, wxT("Save main.mcf"), wxPoint(5, 5));
			hbox7->Add(this->saveButton, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox7, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);
	}
	panel->SetSizer(vbox);

	// ----------------------------------------------------

	this->loadMainMcf();
}

Frame::~Frame()
{

}

void Frame::fromObjectToInput()
{
	this->icaoInput->SetValue(wxString(this->argumentor.icaoCode));

	this->utcDateValue.SetYear(this->aerofly.year);
	this->utcDateValue.SetMonth((wxDateTime::Month)(this->aerofly.month - 1));
	this->utcDateValue.SetDay(this->aerofly.day);
	auto hour = floor(this->aerofly.hours);
	this->utcDateValue.SetHour(hour);
	this->utcDateValue.SetMinute(round((this->aerofly.hours - hour) * 60));

	this->utcTimeInput->SetValue(this->utcDateValue);
	this->utcDateInput->SetValue(this->utcDateValue);

	this->windDirectionInput->SetValue(this->aerofly.windDirection);
	this->windStrengthInput->SetValue(ceil(this->aerofly.windStrength * 100));

	this->windTurbulenceInput->SetValue(ceil(this->aerofly.windTurbulence * 100));
	this->thermalActivityInput->SetValue(ceil(this->aerofly.thermalActivity * 100));

	this->visbilityInput->SetValue(ceil(this->aerofly.visibility * 100));

	for (int i = 0; i < 3; ++i) {
		this->clouds[i].heightInput->SetValue(ceil(this->aerofly.clouds[i].height * 100));
		this->clouds[i].densityInput->SetValue(ceil(this->aerofly.clouds[i].density * 100));
	}
}

void Frame::fromInputToObject()
{
	auto date = this->utcDateInput->GetValue();
	this->aerofly.year = date.GetYear();
	this->aerofly.month = (date.GetMonth() + 1);
	this->aerofly.day = date.GetDay();

	auto time = this->utcTimeInput->GetValue();
	this->aerofly.hours = time.GetHour() + time.GetMinute() / 60.0;

	this->aerofly.windDirection = this->windDirectionInput->GetValue();
	this->aerofly.windStrength = this->windStrengthInput->GetValue() / 100.0;

	this->aerofly.windTurbulence = this->windTurbulenceInput->GetValue() / 100.0;
	this->aerofly.thermalActivity = this->thermalActivityInput->GetValue() / 100.0;

	this->aerofly.visibility = this->visbilityInput->GetValue() / 100.0;

	for (int i = 0; i < 3; ++i) {
		this->aerofly.clouds[i].height = this->clouds[i].heightInput->GetValue() / 100.0;
		this->aerofly.clouds[i].density = this->clouds[i].densityInput->GetValue() / 100.0;
	}
}

void Frame::loadMainMcf()
{
	this->mainConfig.setFilename(this->argumentor.filename);
	this->mainConfig.load();
	this->mainConfig.getToAeroflyObject(this->aerofly);
	this->utcDateValue.SetToCurrent();
	this->metarInput->SetValue("");
	if (strlen(this->argumentor.icaoCode) == 0) {
		std::string origin;
		std::string destination;
		std::tie(origin, destination) = this->mainConfig.getFlightplan();
		strcpy(this->argumentor.icaoCode, origin.c_str());
	}
	this->fromObjectToInput();
}

void Frame::actionFetch(wxCommandEvent& WXUNUSED(event))
{
	auto icaoCode = this->icaoInput->GetValue();
	if (icaoCode == "") {
		this->metarInput->SetValue("No ICAO code given");
		return;
	}
	this->metarInput->SetValue("Loading...");
	FetchUrl urlFetcher;
	try {
		auto metarString = urlFetcher.fetch(this->argumentor.url, icaoCode, this->argumentor.response, this->argumentor.apikey);
		this->metarInput->SetValue(metarString);
		//this->metarInput->SetValue("METAR KTTN 051853Z 04011KT 1/2SM VCTS SN FZFG BKN003 OVC010 M02/M02 A3006 RMK AO2 TSB40 SLP176 P0002 T10171017=");
		this->saveButton->SetFocus();
	}
	catch (std::invalid_argument& e) {
		this->metarInput->SetValue(e.what());
	}
}

void Frame::actionParse(wxCommandEvent& WXUNUSED(event))
{
	auto metarInput = this->metarInput->GetValue();
	if (metarInput.length() < 14) {
		return;
	}
	try {
		this->aerofly.setFromMetarString(metarInput);
		//this->icaoInput->SetValue();
		this->fromObjectToInput();
	}
	catch (std::invalid_argument& e) {
		this->metarInput->SetValue(e.what());
	}
}

void Frame::actionSave(wxCommandEvent& WXUNUSED(event))
{
	if (!this->argumentor.isDryRun) {
		this->fromInputToObject();
		this->mainConfig.setFromAeroflyObject(this->aerofly);
		this->mainConfig.save();
	}
}

void Frame::actionExit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void Frame::actionAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo aboutInfo;
	aboutInfo.SetName("Aerofly Wetterger\u00E4t");
	aboutInfo.SetVersion(MY_APP_VERSION_STRING);
	aboutInfo.SetDescription(_("Copy METAR weather information into IPCAS' Aerofly FS 2.\n\nCurrentAPI:\n") + this->argumentor.url);
	aboutInfo.SetCopyright("(C) 2019");
	aboutInfo.SetWebSite("https://github.com/fboes/aerofly-wettergeraet");
	aboutInfo.AddDeveloper("Frank Bo\u00EBs");
	aboutInfo.SetIcon(wxICON(APPICON));
	aboutInfo.SetLicence("MIT");

	wxAboutBox(aboutInfo);
}

void Frame::actionLoadMainMcf(wxCommandEvent& WXUNUSED(event))
{
	this->loadMainMcf();
}

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
EVT_BUTTON(Frame::EL_BUTTON_FETCH, Frame::actionFetch)
//EVT_BUTTON(Frame::EL_BUTTON_PARSE, Frame::actionParse)
EVT_BUTTON(wxID_SAVE, Frame::actionSave)
EVT_MENU(wxID_ABOUT, Frame::actionAbout)
EVT_MENU(wxID_EXIT, Frame::actionExit)
EVT_MENU(wxID_OPEN, Frame::actionLoadMainMcf)
EVT_TEXT(Frame::EL_CTRL_METAR, Frame::actionParse)
//EVT_COMMAND_SCROLL_CHANGED(Frame::actionSave);
wxEND_EVENT_TABLE()