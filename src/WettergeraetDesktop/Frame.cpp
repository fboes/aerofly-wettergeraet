#define wxUSE_DATEPICKCTRL 1

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include <wx/aboutdlg.h>
#include <wx/icon.h>
#include <wx/event.h>
#include <wx/filedlg.h>
#include <wx/textfile.h>
#include <wx/filename.h>
#include <math.h>
#include <tuple>
#include "Frame.h"
#include "Argumentor.h"
#include "AeroflyConfigFile.h"

const char* Frame::EL_BUTTON_SAVE_LABEL = "Save main.mcf";
const char* Frame::EL_BUTTON_SAVE_LABEL_DIRTY = "Save main.mcf \u2022";

void Frame::addIcaoChoice(char const icaoCode[8])
{
	if (strlen(icaoCode) == 0) {
		return;
	}
	auto ret = this->icaoChoices.insert(std::make_pair(icaoCode, 1));
	if (ret.second != false) {
		this->icaoInput->Append(icaoCode);
	}
}

Frame::Frame(const wxString& title, int argc, char * argv[]) : wxFrame(nullptr, wxID_ANY, title, wxPoint(-1, -1), wxSize(640, 580))
{
	this->utcDateValue.SetToCurrent();
	this->SetIcon(wxICON(APPICON));
	this->argumentor.getArgs(argc, argv);

	wxMenuBar *menubar = new wxMenuBar;
	{
		wxMenu* file = new wxMenu;
		file->Append(wxID_OPEN, wxT("&Open METAR File...\tCTRL+O"));
		file->Append(wxID_SAVEAS, wxT("&Save METAR File As..."));
		file->Append(Frame::EL_MENU_SAVECLIP, wxT("Save &Clipboard File..."));
		file->AppendSeparator();
		file->Append(EL_MENU_LOAD, wxT("Open &Aerofly Configuration File..."));
		file->Append(EL_MENU_RELOAD, wxT("&Reload Aerofly Configuration File\tCTRL+R"));
		file->Append(wxID_SAVE, wxT("Sa&ve Aerofly Configuration File\tCTRL+S"));
		file->AppendSeparator();
		file->Append(wxID_EXIT, wxT("E&xit"));
		menubar->Append(file, wxT("&File"));

		wxMenu* tools = new wxMenu;
		tools->Append(EL_MENU_GET_PLATES, wxT("Get &Airport Information\tCTRL+F"));
		tools->AppendSeparator();
		tools->Append(EL_MENU_RESET_DATE_TIME, wxT("&Reset Date and Time"));
		tools->Append(EL_MENU_OPEN_WORLDCLOCK, wxT("Show &World Clock"));
		menubar->Append(tools, wxT("&Tools"));

		wxMenu* help = new wxMenu;
		help->Append(wxID_HELP, wxT("View &Help\tF1"));
		help->Append(EL_MENU_UPDATE, wxT("Check for &Updates"));
		help->AppendSeparator();
		help->Append(wxID_ABOUT, wxT("&About"));
		menubar->Append(help, wxT("&Help"));
	}
	SetMenuBar(menubar);

	CreateStatusBar(3);
	SetStatusText((this->argumentor.apikey) ? "API key present" : "No API key loaded", 0);

	const int labelBorder = 8;

	wxPanel *panel = new wxPanel(this, wxID_ANY);

	// build vbox
	wxBoxSizer *vbox = new wxBoxSizer(wxVERTICAL);
	{
		// build hbox1
		wxBoxSizer *hbox1 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *icaoLabel = new wxStaticText(panel, wxID_ANY, wxT("ICAO airport code"));
			hbox1->Add(icaoLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			this->icaoInput = new wxComboBox(panel, wxID_ANY);
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
		}
		vbox->Add(hbox2, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);

		// build hbox3
		wxBoxSizer *hbox3 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *utcTimeLabel = new wxStaticText(panel, wxID_ANY, wxT("Time (UTC)"));
			hbox3->Add(utcTimeLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			wxDateTime utcDateValue = wxDateTime::Now().MakeUTC();
			this->utcTimeInput = new wxTimePickerCtrl(panel, Frame::EL_CTRL_DATETIME, utcDateValue);
			hbox3->Add(this->utcTimeInput, 1, wxALIGN_CENTER_VERTICAL);

			hbox3->Add(10, -1);

			wxStaticText *utcDateLabel = new wxStaticText(panel, wxID_ANY, wxT("Date (UTC)"));
			hbox3->Add(utcDateLabel, 1, wxALIGN_CENTER_VERTICAL, labelBorder);

			this->utcDateInput = new wxDatePickerCtrl(panel, Frame::EL_CTRL_DATETIME, utcDateValue);
			this->utcDateInput->SetRange(
				wxDateTime::Now().MakeUTC().Subtract(
					wxDateSpan::Year()
				),
				utcDateValue
			);
			hbox3->Add(this->utcDateInput, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox3, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);

		// build hbox8
		for (int i = 0; i < 3; ++i) {
			std::string cloudName = "";
			switch (i) {
			case 0:
				cloudName = "Cumulus";
				break;
			case 1:
				cloudName = "Cumulus med";
				break;
			default:
				cloudName = "Cirrus";
				break;
			}
			wxBoxSizer *hbox8 = new wxBoxSizer(wxHORIZONTAL);
			{
				wxStaticText *cloudsHeightLabel = new wxStaticText(panel, wxID_ANY, cloudName + " height (ft)");
				hbox8->Add(cloudsHeightLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

				this->clouds[i].heightInput = new wxSlider(
					panel, Frame::EL_CTRL_SLIDER,
					this->aerofly.maxCloudsHeight / 4.0 * (i + 1),
					0, this->aerofly.maxCloudsHeight,
					wxDefaultPosition, wxDefaultSize,
					wxSL_HORIZONTAL | wxSL_VALUE_LABEL
				);
				hbox8->Add(this->clouds[i].heightInput, 1, wxALIGN_CENTER_VERTICAL);

				hbox8->Add(10, -1);

				wxStaticText *cloudsDensityLabel = new wxStaticText(panel, wxID_ANY, cloudName + " density (%)");
				hbox8->Add(cloudsDensityLabel, 1, wxALIGN_CENTER_VERTICAL, labelBorder);

				this->clouds[i].densityInput = new wxSlider(panel, Frame::EL_CTRL_SLIDER, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
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

			this->windDirectionInput = new wxSlider(panel, Frame::EL_CTRL_SLIDER, 180, 0, 359, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
			hbox5->Add(this->windDirectionInput, 1, wxALIGN_CENTER_VERTICAL);

			hbox5->Add(10, -1);

			wxStaticText *windStrengthLabel = new wxStaticText(panel, wxID_ANY, wxT("Wind strength (kts)"));
			hbox5->Add(windStrengthLabel, 1, wxALIGN_CENTER_VERTICAL, labelBorder);

			this->windStrengthInput = new wxSlider(panel, Frame::EL_CTRL_SLIDER, 4, 0, 48, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
			hbox5->Add(this->windStrengthInput, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox5, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);

		// build hbox6
		wxBoxSizer *hbox6 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *windTurbulenceLabel = new wxStaticText(panel, wxID_ANY, wxT("Turbulence (%)"));
			hbox6->Add(windTurbulenceLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			this->windTurbulenceInput = new wxSlider(panel, Frame::EL_CTRL_SLIDER, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
			hbox6->Add(this->windTurbulenceInput, 1, wxALIGN_CENTER_VERTICAL);

			hbox6->Add(10, -1);

			wxStaticText *thermalActivityLabel = new wxStaticText(panel, wxID_ANY, wxT("Thermal activity (%)"));
			hbox6->Add(thermalActivityLabel, 1, wxALIGN_CENTER_VERTICAL, labelBorder);

			this->thermalActivityInput = new wxSlider(panel, Frame::EL_CTRL_SLIDER, 50, 0, 100, wxDefaultPosition, wxDefaultSize, wxSL_HORIZONTAL | wxSL_VALUE_LABEL);
			hbox6->Add(this->thermalActivityInput, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox6, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);

		// build hbox7
		wxBoxSizer *hbox7 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText* pressureLabel = new wxStaticText(panel, wxID_ANY, wxT("Pressure (hPa)"));
			hbox7->Add(pressureLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			this->pressureInput = new wxSlider(
				panel, Frame::EL_CTRL_SLIDER,
				1013,
				this->aerofly.minPressure, this->aerofly.maxPressure,
				wxDefaultPosition, wxDefaultSize,
				wxSL_HORIZONTAL | wxSL_VALUE_LABEL
			);
			hbox7->Add(this->pressureInput, 1, wxALIGN_CENTER_VERTICAL);

			hbox7->Add(10, -1);

			wxStaticText* visbilityLabel = new wxStaticText(panel, wxID_ANY, wxT("Visibility (m)"));
			hbox7->Add(visbilityLabel, 1, wxRIGHT | wxALIGN_CENTER_VERTICAL, labelBorder);

			this->visbilityInput = new wxSlider(
				panel, Frame::EL_CTRL_SLIDER,
				this->aerofly.maxVisibility,
				0, this->aerofly.maxVisibility,
				wxDefaultPosition, wxDefaultSize,
				wxSL_HORIZONTAL | wxSL_VALUE_LABEL
			);
			hbox7->Add(this->visbilityInput, 1, wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox7, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);

		// build hbox8
		wxBoxSizer* hbox8 = new wxBoxSizer(wxHORIZONTAL);
		{
			wxStaticText *noLabel = new wxStaticText(panel, wxID_ANY, wxT(""));
			hbox8->Add(noLabel, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);

			this->saveButton = new wxButton(panel, wxID_SAVE, this->EL_BUTTON_SAVE_LABEL_DIRTY, wxPoint(5, 5));
			hbox8->Add(this->saveButton, 1, wxLEFT | wxALIGN_CENTER_VERTICAL);
		}
		vbox->Add(hbox8, 0, wxEXPAND | wxLEFT | wxRIGHT, 10);
		vbox->Add(-1, 10);
	}
	panel->SetSizer(vbox);

	// ----------------------------------------------------

	this->currentMcfFilename = this->argumentor.filename;
	this->loadMainMcf();
	if (strlen(this->argumentor.metarfile) != 0) {
		this->loadMetarFile(this->argumentor.metarfile);
	}
	this->addIcaoChoice("KDEN (Denver International, USA)");
	this->addIcaoChoice("KJFK (New York/John F Kennedy International, USA)");
	this->addIcaoChoice("KLAS (Las Vegas/McCarran International, USA)");
	this->addIcaoChoice("KLAX (Los Angeles International, USA)");
	this->addIcaoChoice("KMIA (Miami International, USA)");
	this->addIcaoChoice("KORD (Chicago O'Hare International, USA)");
	this->addIcaoChoice("KSFO (San Francisco International, USA)");
}

Frame::~Frame()
{

}

wxString Frame::getIcaoFromInput()
{
	wxString icao = this->icaoInput->GetValue();
	return icao.BeforeFirst(' ');
}

// --------------------------------------------------------------------------------------

void Frame::fromObjectToInput(bool ignoreDate = false)
{
	this->icaoInput->SetValue(wxString(this->aerofly.nearestAirport));

	if (!ignoreDate) {
		// Discard date from Aerofly date, instead set it to today...
		this->utcDateValue = wxDateTime::Now().MakeUTC();

		// ...but keep the time
		auto hour = floor(this->aerofly.hours);
		this->utcDateValue.SetHour(hour);
		this->utcDateValue.SetMinute(round((this->aerofly.hours - hour) * 60));
		this->utcDateValue.SetSecond(0);

		if (this->utcDateValue.IsLaterThan(wxDateTime::Now().MakeUTC())) {
			// Substract one day if current datetime is in future
			this->utcDateValue = this->utcDateValue.Subtract(wxDateSpan::Day());
		}

		this->utcTimeInput->SetValue(this->utcDateValue);
		this->utcDateInput->SetValue(this->utcDateValue);
	}


	this->windDirectionInput->SetValue(this->aerofly.windDirection);
	this->windStrengthInput->SetValue(ceil(this->aerofly.getWindKts()));

	this->windTurbulenceInput->SetValue(ceil(this->aerofly.windTurbulence * 100));
	this->thermalActivityInput->SetValue(ceil(this->aerofly.thermalActivity * 100));

	this->pressureInput->SetValue(floor(this->aerofly.getPressureHpa()));
	this->visbilityInput->SetValue(floor(this->aerofly.getVisbilityMeters()));

	for (int i = 0; i < 3; ++i) {
		this->clouds[i].heightInput->SetValue(floor(this->aerofly.getCloudHeightFt(i)));
		this->clouds[i].densityInput->SetValue(ceil(this->aerofly.clouds[i].density * 100));
	}

	this->updateFlightCategory();
}

void Frame::fromInputToObject()
{
	auto date = this->utcDateInput->GetValue();
	this->aerofly.year = date.GetYear();
	this->aerofly.month = (date.GetMonth() + 1);
	this->aerofly.day = date.GetDay();

	auto time = this->utcTimeInput->GetValue();
	this->aerofly.hours = time.GetHour() + time.GetMinute() / 60.0 + time.GetSecond() / (60.0 * 60.0);

	this->aerofly.setWind(this->windStrengthInput->GetValue(), this->windDirectionInput->GetValue());

	this->aerofly.windTurbulence = this->windTurbulenceInput->GetValue() / 100.0;
	this->aerofly.thermalActivity = this->thermalActivityInput->GetValue() / 100.0;

	this->aerofly.setPressureHpa(this->pressureInput->GetValue());
	this->aerofly.setVisibility(this->visbilityInput->GetValue());

	for (int i = 0; i < 3; ++i) {
		this->aerofly.setCloudPercent(i, this->clouds[i].heightInput->GetValue(), this->clouds[i].densityInput->GetValue() / 100.0);
	}
}

/**
 * @see https://www.thinkaviation.net/levels-of-vfr-ifr-explained/
 * @see https://en.wikipedia.org/wiki/Ceiling_(cloud)
 */
void Frame::updateFlightCategory()
{
	std::string flightCategory = "LIFR";
	std::string icaoFlightCategory = "IFR";

	double visibilityStatuteMiles = this->visbilityInput->GetValue() / 1609.344;
	double ceilingFeet = 9999.99;

	for (int i = 0; i < 3; ++i) {
		auto cloudHeightFeet = this->clouds[i].heightInput->GetValue();
		auto cloudDensityPercent = this->clouds[i].densityInput->GetValue();
		if (cloudDensityPercent > 50 && cloudHeightFeet > 0 && cloudHeightFeet < ceilingFeet) {
			ceilingFeet = cloudHeightFeet;
		}
	}

	if (visibilityStatuteMiles > 5.0 && (ceilingFeet > 3000)) {
		flightCategory = "VFR";
	}
	else if (visibilityStatuteMiles >= 3.0 && (ceilingFeet >= 1000)) {
		flightCategory = "MVFR";
	}
	else if (visibilityStatuteMiles >= 1.0 && (ceilingFeet >= 500)) {
		flightCategory = "IFR";
	}

	if (this->visbilityInput->GetValue() >= 5000 && (ceilingFeet > 1500)) {
		icaoFlightCategory = "VFR";
	}

	SetStatusText("Flight rules: " + flightCategory + " (ICAO: " + icaoFlightCategory + ")", 1);
}

void Frame::markAsClean()
{
	this->saveButton->SetLabel(this->EL_BUTTON_SAVE_LABEL);
	SetStatusText(wxT("Saved"), 2);
}

void Frame::markAsDirty()
{
	for (int i = 0; i < 3; ++i) {
		if (this->clouds[i].heightInput->GetValue() % 100) {
			this->clouds[i].heightInput->SetValue(
				round(this->clouds[i].heightInput->GetValue() / 100.0) * 100.0
			);
		}
	}
	if (this->visbilityInput->GetValue() % 100) {
		this->visbilityInput->SetValue(
			round(this->visbilityInput->GetValue() / 100.0) * 100.0
		);
	}
	this->saveButton->SetLabel(this->EL_BUTTON_SAVE_LABEL_DIRTY);
	SetStatusText(wxT("Unsaved changes"), 2);
}

void Frame::loadMainMcf()
{
	try {
		this->mainConfig.setFilename(this->currentMcfFilename.ToStdString());
		this->mainConfig.load();
		this->mainConfig.getToAeroflyObject(this->aerofly);
		this->utcDateValue.SetToCurrent();
		this->metarInput->SetValue("");
		this->addIcaoChoice(this->argumentor.icaoCode);
		std::string origin;
		std::string destination;
		std::tie(origin, destination) = this->mainConfig.getFlightplan();
		if (origin != "") {
			strcpy(this->aerofly.nearestAirport, origin.c_str());
			this->addIcaoChoice(origin.c_str());
		}
		else {
			strcpy(this->aerofly.nearestAirport, this->argumentor.icaoCode);
		}
		if (destination != "" && origin != destination) {
			strcpy(this->aerofly.nearestAirport, destination.c_str());
			this->addIcaoChoice(destination.c_str());
		}
		this->fromObjectToInput();
		this->markAsClean();
	}
	catch (std::invalid_argument& e) {
		wxLogError(e.what());
		return;
	}
}

// --------------------------------------------------------------------------------------

void Frame::actionFetch(wxCommandEvent& WXUNUSED(event))
{
	auto icaoCode = this->getIcaoFromInput();
	if (icaoCode == "") {
		wxLogError(_("No ICAO airport code given"));
		return;
	}

	auto searchDate = this->utcDateInput->GetValue().FormatISODate().ToStdString()
		.append("T")
		.append(
			this->utcTimeInput->GetValue().FormatISOTime().ToStdString()
		)
		.append("Z");

	this->metarInput->SetValue("Loading...");
	FetchUrl urlFetcher;
	try {
		auto metarString = urlFetcher.fetch(
			this->argumentor.url,
			icaoCode.ToStdString(),
			searchDate,
			this->argumentor.response,
			this->argumentor.apikey
		);
		this->metarInput->SetValue(metarString);
		this->saveButton->SetFocus();
		this->markAsDirty();
	}
	catch (std::invalid_argument& e) {
		wxLogError(e.what());
		this->metarInput->SetValue("");
	}
}

void Frame::actionParse(wxCommandEvent& WXUNUSED(event))
{
	auto metarInput = this->metarInput->GetValue();
	if (metarInput.length() < 14) {
		return;
	}
	try {
		this->aerofly.setFromMetarString(metarInput.ToStdString());
		this->fromObjectToInput(true);
	}
	catch (std::invalid_argument& e) {
		wxLogError(e.what());
	}
}

void Frame::loadMetarFile(wxString filename) {
	wxTextFile tfile;
	tfile.Open(filename);

	auto metarString = tfile.GetFirstLine();
	if (!tfile.Eof()) {
		metarString += "\n" + tfile.GetNextLine();
	}
	this->metarInput->SetValue(metarString);
	this->saveButton->SetFocus();
	this->markAsDirty();
}

void Frame::actionSave(wxCommandEvent& WXUNUSED(event))
{
	if (!this->argumentor.isDryRun) {
		try {
			this->fromInputToObject();
			this->mainConfig.setFromAeroflyObject(this->aerofly);
			this->mainConfig.save();
		}
		catch (std::invalid_argument& e) {
			wxLogError(e.what());
			return;
		}
	}
	this->markAsClean();
}

void Frame::actionExit(wxCommandEvent& WXUNUSED(event))
{
	Close(true);
}

void Frame::actionAbout(wxCommandEvent& WXUNUSED(event))
{
	wxAboutDialogInfo aboutInfo;
	aboutInfo.SetName("Aerofly Wetterger\u00E4t");
	aboutInfo.SetVersion(this->argumentor.APP_VERSION + std::string(" (") + this->argumentor.APP_TARGET + std::string(")"));
	aboutInfo.SetDescription(
		_("Copy METAR weather information into IPCAS' Aerofly FS 2 and Aerofly FS 4.\n")
		+ _("\nCurrent MCF file:\n") + this->currentMcfFilename
		+ _("\nCurrent API URL:\n") + this->argumentor.url
		+ _("\nCurrent API Key:\n") + this->argumentor.apikey
	);
	aboutInfo.SetCopyright("\u00A9 2019-2024");
	aboutInfo.SetWebSite("https://github.com/fboes/aerofly-wettergeraet");
	aboutInfo.AddDeveloper("Frank Bo\u00EBs");
	aboutInfo.SetIcon(wxICON(APPICON));
	aboutInfo.SetLicence("MIT");

	wxAboutBox(aboutInfo);
}

void Frame::actionOpenMetarFile(wxCommandEvent& WXUNUSED(event))
{
	wxFileDialog openFileDialog(this, _("Open METAR file"), "", "", "Text files (*.txt, *.rwx)|*.txt;*.rwx", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	return this->loadMetarFile(openFileDialog.GetPath());
}

void Frame::actionSaveMetarFile(wxCommandEvent& WXUNUSED(event))
{
	auto date = this->utcDateInput->GetValue();
	auto year = date.GetYear();
	auto month = date.GetMonth() + 1;
	auto day = date.GetDay();

	auto time = this->utcTimeInput->GetValue();
	auto hour = time.GetHour();
	auto minute = time.GetMinute();

	char timestamp[25];
	sprintf(timestamp, "%04d-%02d-%02d_%02d%02dZ", year, month, day, hour, minute);
	wxString defaultFilename = this->getIcaoFromInput() + "_" + timestamp + ".rwx";

	wxFileDialog saveFileDialog(this, _("Save METAR file"), "", defaultFilename, "Text files (*.txt, *.rwx)|*.txt;*.rwx", wxFD_SAVE);
	if (saveFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	sprintf(timestamp, "%04d/%02d/%02d %02d:%02d", year, month, day, hour, minute);
	auto metarInput = this->metarInput->GetValue();
	wxTextFile tfile(saveFileDialog.GetPath());
	if (!tfile.Exists()) {
		tfile.Create();
	}
	else {
		tfile.Open();
		tfile.Clear();
	}

	tfile.AddLine(timestamp);
	tfile.AddLine(metarInput);

	tfile.Write();
	tfile.Close();
}

void Frame::actionSaveClipFile(wxCommandEvent& WXUNUSED(event))
{
	wxString defaultFilename = "clip.txt";

	wxFileDialog saveFileDialog(this, _("Save Clipboard File"), "", defaultFilename, "Text files (*.txt)|*.txt", wxFD_SAVE);
	if (saveFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}

	wxString header = "METAR: ";
	auto headerLength = header.length();
	auto metarInput = header + this->metarInput->GetValue();
	bool replace = false;

	wxTextFile tfile(saveFileDialog.GetPath());
	if (!tfile.Exists()) {
		tfile.Create();
		tfile.AddLine(metarInput);
	}
	else {
		tfile.Open();

		for (size_t n = 0; n < tfile.GetLineCount(); ++n)
		{
			wxString line = tfile.GetLine(n);
			if (line.Left(headerLength) == header) {
				tfile.RemoveLine(n);
				tfile.InsertLine(metarInput, n);
				replace = true;
			}
		}
		if (!replace) {
			tfile.AddLine("");
			tfile.AddLine(metarInput);
		}
	}

	tfile.Write();
	tfile.Close();
}

void Frame::actionLoadMainMcf(wxCommandEvent& WXUNUSED(event))
{
	wxFileName dingdong = wxFileName(this->currentMcfFilename);
	auto dirname = dingdong.GetPath();
	auto filename = dingdong.GetFullName();

	wxFileDialog openFileDialog(
		this,
		_("Open Aerofly configuration file"),
		dirname,
		filename,
		"Aerofly Configuration File (*.mcf)|*.mcf",
		wxFD_OPEN | wxFD_FILE_MUST_EXIST
	);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		return;
	}
	this->currentMcfFilename = openFileDialog.GetPath();
	return this->loadMainMcf();
}

void Frame::actionReloadMainMcf(wxCommandEvent& WXUNUSED(event))
{
	return this->loadMainMcf();
}

void Frame::actionUpdate(wxCommandEvent& WXUNUSED(event))
{
	wxLaunchDefaultBrowser("https://github.com/fboes/aerofly-wettergeraet/releases/latest");
}

void Frame::actionHelp(wxCommandEvent& WXUNUSED(event))
{
	wxLaunchDefaultBrowser("https://github.com/fboes/aerofly-wettergeraet/blob/master/README.md");
}

void Frame::setDateTimeToCurrent(wxCommandEvent& WXUNUSED(event)) {
	this->utcDateValue.SetToCurrent().MakeUTC();
	this->utcTimeInput->SetValue(this->utcDateValue);
	this->utcDateInput->SetValue(this->utcDateValue);
	this->markAsDirty();
}

void Frame::actionGetAirportPlates(wxCommandEvent& WXUNUSED(event))
{
	wxString url = "https://skyvector.com/airport";
	auto icaoCode = this->getIcaoFromInput();
	if (icaoCode != "") {
		url += "/" + icaoCode;
	}
	else {
		url += "s";
	}
	wxLaunchDefaultBrowser(url);
}

void Frame::actionOpenWorldClockLink(wxCommandEvent& WXUNUSED(event))
{
	wxString url = "https://www.worldtimebuddy.com/";
	wxLaunchDefaultBrowser(url);
}

void Frame::actionMarkAsDirty(wxCommandEvent& WXUNUSED(event))
{
	this->updateFlightCategory();
	this->markAsDirty();
}

wxBEGIN_EVENT_TABLE(Frame, wxFrame)
EVT_BUTTON(Frame::EL_BUTTON_FETCH, Frame::actionFetch)
EVT_BUTTON(wxID_SAVE, Frame::actionSave)
EVT_MENU(Frame::EL_MENU_GET_PLATES, Frame::actionGetAirportPlates)
EVT_MENU(Frame::EL_MENU_OPEN_WORLDCLOCK, Frame::actionOpenWorldClockLink)
EVT_MENU(Frame::EL_MENU_RESET_DATE_TIME, Frame::setDateTimeToCurrent)
EVT_MENU(wxID_HELP, Frame::actionHelp)
EVT_MENU(Frame::EL_MENU_UPDATE, Frame::actionUpdate)
EVT_MENU(wxID_ABOUT, Frame::actionAbout)
EVT_MENU(wxID_EXIT, Frame::actionExit)
EVT_MENU(wxID_OPEN, Frame::actionOpenMetarFile)
EVT_MENU(wxID_SAVEAS, Frame::actionSaveMetarFile)
EVT_MENU(Frame::EL_MENU_SAVECLIP, Frame::actionSaveClipFile)
EVT_MENU(Frame::EL_MENU_LOAD, Frame::actionLoadMainMcf)
EVT_MENU(Frame::EL_MENU_RELOAD, Frame::actionReloadMainMcf)
EVT_MENU(wxID_SAVE, Frame::actionSave)
EVT_TEXT(Frame::EL_CTRL_METAR, Frame::actionParse)
EVT_SLIDER(Frame::EL_CTRL_SLIDER, Frame::actionMarkAsDirty)
//EVT_DATE_CHANGED(Frame::EL_CTRL_DATETIME, Frame::actionMarkAsDirty)
wxEND_EVENT_TABLE()
