#pragma once

#define wxUSE_DATEPICKCTRL 1

#include <wx/wx.h>
#include <wx/datectrl.h>
#include <wx/timectrl.h>
#include "WettergeraetDesktop.h"
#include "Argumentor.h"
#include "AeroflyConfigFile.h"

class Frame : public wxFrame
{
private:
	Argumentor argumentor;
	AeroflyConfigFile mainConfig;
	AeroflyWeather aerofly;
	wxDateTime utcDateValue;

	// Represents a cloud layer
	struct cloudsInputs {
		wxSlider *densityInput;
		wxSlider *heightInput;
	};

	wxTextCtrl *icaoInput;
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

	DECLARE_EVENT_TABLE()

public:
	static const unsigned short EL_BUTTON_FETCH = 1;
	static const unsigned short EL_BUTTON_PARSE = 2;
	static const unsigned short EL_CTRL_METAR = 4;
	static const unsigned short EL_CTRL_DATETIME = 5;
	static const unsigned short EL_CTRL_SLIDER = 6;
	static const unsigned short EL_MENU_UPDATE = 7;
	static const unsigned short EL_MENU_FIND_ICAO = 8;

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

	// Save Aerofly values into config file
	virtual void actionSave(wxCommandEvent&);

	virtual void actionExit(wxCommandEvent&);

	virtual void actionAbout(wxCommandEvent&);

	virtual void actionLoadMainMcf(wxCommandEvent&);

	virtual void actionUpdate(wxCommandEvent&);

	virtual void actionHelp(wxCommandEvent&);

	virtual void actionFindIcao(wxCommandEvent&);

	virtual void actionMarkAsDirty(wxCommandEvent&);

};
