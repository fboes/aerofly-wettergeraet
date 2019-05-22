#pragma once

#define wxUSE_DATEPICKCTRL 1

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include "WettergeraetDesktop.h"
#include "Argumentor.h"
#include "AeroflyConfigFile.h"
#include <map>

class Frame : public wxFrame
{
private:
	Argumentor argumentor;
	AeroflyConfigFile mainConfig;
	AeroflyWeather aerofly;
	wxDateTime utcDateValue;
	wxString currentMcfFilename;

	// Represents a cloud layer
	struct cloudsInputs {
		wxSlider *densityInput;
		wxSlider *heightInput;
	};

	wxComboBox *icaoInput;
	wxButton *fetchButton;
	wxTextCtrl *metarInput;
	wxTimePickerCtrl *utcTimeInput;
	wxDatePickerCtrl *utcDateInput;
	wxSlider *windDirectionInput;
	wxSlider *windStrengthInput;
	wxSlider *windTurbulenceInput;
	wxSlider *thermalActivityInput;
	wxSlider *visbilityInput;
	cloudsInputs clouds[3]; // "cirrus", "cumulus", "cumulus_mediocris"
	wxButton *saveButton;

	static const char* EL_BUTTON_SAVE_LABEL;
	static const char* EL_BUTTON_SAVE_LABEL_DIRTY;

	std::map<std::string, int> icaoChoices;

	// Add ICAO code to ICAO code combobox choices
	void addIcaoChoice(char const icaoCode[8]);

	DECLARE_EVENT_TABLE()

public:
	static const unsigned short EL_BUTTON_FETCH = 1;
	static const unsigned short EL_BUTTON_PARSE = 2;
	static const unsigned short EL_CTRL_METAR = 4;
	static const unsigned short EL_CTRL_DATETIME = 5;
	static const unsigned short EL_CTRL_SLIDER = 6;
	static const unsigned short EL_MENU_UPDATE = 7;
	static const unsigned short EL_MENU_FIND_ICAO = 8;
	static const unsigned short EL_MENU_RELOAD = 9;
	static const unsigned short EL_MENU_LOAD = 10;

	Frame(const wxString& title, int argc, char * argv[]);
	virtual ~Frame();

	// Copy state of objects to form
	virtual void fromObjectToInput();

	// Copy state of form to objects
	virtual void fromInputToObject();

	// Show user that current state has been saved.
	virtual void markAsClean();

	// Show user that current state has not yet been saved.
	virtual void markAsDirty();

	virtual void loadMainMcf();

	// Fetch new METAR data
	virtual void actionFetch(wxCommandEvent&);

	// Parse METAR data into Aerofly values
	virtual void actionParse(wxCommandEvent&);

	// Copy METAR information from file to METAR input field
	virtual void loadMetarFile(wxString filename);

	// Save Aerofly values into config file
	virtual void actionSave(wxCommandEvent&);

	virtual void actionExit(wxCommandEvent&);

	virtual void actionAbout(wxCommandEvent&);

	// Show file dialogue, copy METAR information from file to METAR input field
	virtual void actionOpenMetarFile(wxCommandEvent&);

	// Show file dialogue, copy METAR informtation from METAR input field to file
	virtual void actionSaveMetarFile(wxCommandEvent&);

	// Change main.mcf file location
	virtual void actionLoadMainMcf(wxCommandEvent&);

	// Reload main.mcf and copy values to main windows
	virtual void actionReloadMainMcf(wxCommandEvent&);

	virtual void actionUpdate(wxCommandEvent&);

	virtual void actionHelp(wxCommandEvent&);

	virtual void actionFindIcao(wxCommandEvent&);

	virtual void actionMarkAsDirty(wxCommandEvent&);

};
