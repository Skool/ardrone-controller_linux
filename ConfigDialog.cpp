//////////////////////////////////////////////////////////////////////////////
// Configuration dialog to set application options
//////////////////////////////////////////////////////////////////////////////

#include "ConfigDialog.h"

#include "Utils.h"
#include "AppConfig.h"
#include "Ressources.h"

BEGIN_EVENT_TABLE(CConfigDialog, wxDialog)
	EVT_CLOSE(CConfigDialog::OnClose)
	EVT_BUTTON(ID_BUTTONOK, CConfigDialog::OnOk)
	EVT_BUTTON(ID_BUTTONCANCEL, CConfigDialog::OnCancel)
END_EVENT_TABLE()


/////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////
CConfigDialog::CConfigDialog(wxWindow *parent, bool bIsConnected): wxDialog(parent, 1, "Config", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
{
	this->SetTitle(GetText("Config"));
	this->SetFont(FontNormal10);
	//this->SetForegroundColour(ColorBlueLite);

	// Create the main sizer
	wxBoxSizer*	pMainSizer = new wxBoxSizer(wxVERTICAL);
	pMainSizer->AddSpacer(10);

	// Get all available languages
	wxArrayString arLang;
	CConfig::GetSingleton()->GetPossibleLanguages(&arLang);

	// Available record options
	wxArrayString arRecordOptions;
	arRecordOptions.Add("USB");
	arRecordOptions.Add("USB+PC");
	arRecordOptions.Add("PC");

	// Current Options
	wxString strRecordOptions;
	switch(CConfig::GetSingleton()->GetRecordOption())
	{
		case 2:
			strRecordOptions = "USB+PC";
			break;

		case 3:
			strRecordOptions = "PC";
			break;

		default:
			strRecordOptions = "USB";
			break;
	}	

	// Available resolutions
	wxArrayString arResolutions;
	arResolutions.Add("360");
	arResolutions.Add("720");

	// Current resolution
	wxString strResolution("360");
	if(CConfig::GetSingleton()->GetVideoCodec() == Codec_H264_720P)
	{
		strResolution = "720";
	}	

	// Available HUD styles
	wxArrayString arHUDStyles;
	arHUDStyles.Add("HUD1");
	arHUDStyles.Add("HUD2");

	// Get Current HUD style
	wxString strHudStyle("HUD1");
	if(CConfig::GetSingleton()->GetHudStyle() == 2)
	{
		strHudStyle = "HUD2";
	}

	// Available line sizes for HUD
	wxArrayString arLineSizes;
	arLineSizes.Add("1");
	arLineSizes.Add("2");
	arLineSizes.Add("3");
	arLineSizes.Add("4");

	// Current line size
	wxString strLineSize = ToString(CConfig::GetSingleton()->GetHudLineSize());

	// Create a sizer that contains all the controls
	wxFlexGridSizer* pControls = new wxFlexGridSizer(12, 2, 15, 20);

	// Allow the columns to grow
	pControls->AddGrowableCol(0);
	pControls->AddGrowableCol(1);	

	// Help text for language
	wxStaticText* pLangText = new wxStaticText(this, wxID_ANY, GetText("SelectLang"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pControls->Add(pLangText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);		

	// Combobox to select language
	m_pLanguage = new wxComboBox(this, wxID_ANY, CConfig::GetSingleton()->GetLanguage(), wxDefaultPosition, wxDefaultSize, arLang, wxCB_READONLY);
	pControls->Add(m_pLanguage, 0, wxEXPAND | wxALL, 2);

	// Help text for the sound checkbox
	wxStaticText* pSoundText = new wxStaticText(this, wxID_ANY, GetText("PlaySounds"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pControls->Add(pSoundText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	// Checkbox, if alarm sounds should be played or not
	m_pPlaySounds = new wxCheckBox(this, wxID_ANY, "", wxDefaultPosition);
	m_pPlaySounds->SetValue(CConfig::GetSingleton()->HasSoundFlag());
	pControls->Add(m_pPlaySounds, 0, wxALIGN_CENTER | wxALL, 2);

	// Help text for the video resolution
	wxStaticText* pCodecText = new wxStaticText(this, wxID_ANY, GetText("SelectCodec"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pControls->Add(pCodecText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	// Combobox for resolution
	m_pCodec = new wxComboBox(this, wxID_ANY, strResolution, wxDefaultPosition, wxDefaultSize, arResolutions, wxCB_READONLY);
	pControls->Add(m_pCodec, 0, wxEXPAND | wxALL, 2);

	// Help text for record options
	wxStaticText* pRecordOptionsText = new wxStaticText(this, wxID_ANY, GetText("RecordOptions"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pControls->Add(pRecordOptionsText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	// Combobox for record options
	m_pRecordOptions = new wxComboBox(this, wxID_ANY, strRecordOptions, wxDefaultPosition, wxDefaultSize, arRecordOptions, wxCB_READONLY);
	pControls->Add(m_pRecordOptions, 0, wxEXPAND | wxALL, 2);

	// Help text for the hud style
	wxStaticText* pHudStyleText = new wxStaticText(this, wxID_ANY, GetText("HudStyle"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pControls->Add(pHudStyleText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	// Combobox for HUD style
	m_pHudStyle = new wxComboBox(this, wxID_ANY, strHudStyle, wxDefaultPosition, wxDefaultSize, arHUDStyles, wxCB_READONLY);
	pControls->Add(m_pHudStyle, 0, wxEXPAND | wxALL, 2);

	// Help text for the hud color
	wxStaticText* pHudColorText = new wxStaticText(this, wxID_ANY, GetText("HudColor"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pControls->Add(pHudColorText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	// Color picker
	m_pHudColor = new wxColourPickerCtrl(this, wxID_ANY, CConfig::GetSingleton()->GetHudColor());
	pControls->Add(m_pHudColor, 0, wxEXPAND | wxALL, 2);

	// Help text for the line size
	wxStaticText* pHudLineText = new wxStaticText(this, wxID_ANY, GetText("HudLine"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pControls->Add(pHudLineText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	// Combobox for line size
	m_pHudLineSize = new wxComboBox(this, wxID_ANY, strLineSize, wxDefaultPosition, wxDefaultSize, arLineSizes, wxCB_READONLY);
	pControls->Add(m_pHudLineSize, 0, wxEXPAND | wxALL, 2);

	// Help text for the US unit checkbox
	wxStaticText* pUSUnitText = new wxStaticText(this, wxID_ANY, GetText("USUnit"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pControls->Add(pUSUnitText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	// Checkbox, if US units should be use or not
	m_pUSUnit = new wxCheckBox(this, wxID_ANY, "", wxDefaultPosition);
	m_pUSUnit->SetValue(CConfig::GetSingleton()->HasUSUnit());
	pControls->Add(m_pUSUnit, 0, wxALIGN_CENTER | wxALL, 2);

	// Help text for the low res checkbox
	wxStaticText* pLowResText = new wxStaticText(this, wxID_ANY, GetText("LowRes"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pControls->Add(pLowResText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	// Checkbox, if low res should be use or not in fullscreen
	m_pUseLowRes = new wxCheckBox(this, wxID_ANY, "", wxDefaultPosition);
	m_pUseLowRes->SetValue(CConfig::GetSingleton()->UseLowRes());
	pControls->Add(m_pUseLowRes, 0, wxALIGN_CENTER | wxALL, 2);

	//////////////////////////////////////////////////////////////////////////////
	// Note : Sliders seems to be bugged in version 2.9.1 with flag wxSL_LABELS 
	// (value cannot be displayed at the bottom, size is always to big) so we 
	// set a fixed size
	//////////////////////////////////////////////////////////////////////////////

	wxSize SliderSize(180, 20);

	// Slider for speed limit
	wxStaticText* pSpeedText = new wxStaticText(this, wxID_ANY, GetText("SetMaxSpeed"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxALIGN_CENTRE);
	pControls->Add(pSpeedText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	m_pMaxSpeed = new wxSlider(this, wxID_ANY, CConfig::GetSingleton()->GetSpeedLimit(), 10, 100, wxDefaultPosition, SliderSize, wxSL_LABELS);
	m_pMaxSpeed->SetFont(FontNormal8);
	pControls->Add(m_pMaxSpeed, 0, wxEXPAND | wxALL, 2);

	// Slider for acceleration limit
	wxStaticText* pAcceletationText = new wxStaticText(this, wxID_ANY, GetText("SetMaxAcc"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxALIGN_CENTRE);
	pControls->Add(pAcceletationText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	m_pMaxAcceleration = new wxSlider(this, wxID_ANY, CConfig::GetSingleton()->GetAccelerationLimit(), 10, 100, wxDefaultPosition, SliderSize, wxSL_LABELS);
	m_pMaxAcceleration->SetFont(FontNormal8);
	pControls->Add(m_pMaxAcceleration, 0, wxEXPAND | wxALL, 2);

	// Slider for altitude limit
	wxStaticText* pAltitudeText = new wxStaticText(this, wxID_ANY, GetText("SetMaxAlt"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE | wxALIGN_CENTRE);
	pControls->Add(pAltitudeText, 0, wxALIGN_CENTER_VERTICAL | wxALIGN_LEFT, 0);

	m_pMaxAltitude = new wxSlider(this, wxID_ANY, CConfig::GetSingleton()->GetAltitudeLimit(), 3, 300, wxDefaultPosition, SliderSize, wxSL_LABELS);
	m_pMaxAltitude->SetFont(FontNormal8);
	pControls->Add(m_pMaxAltitude, 0, wxEXPAND | wxALL, 2);	

	pMainSizer->Add(pControls, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);	
	pMainSizer->AddSpacer(10);	

	// Horizontal sizer for the buttons
	wxBoxSizer* pTmpSizer = new wxBoxSizer(wxHORIZONTAL);

	// Create the ok button
	wxButton *pButtonOk = new wxButton(this, ID_BUTTONOK, GetText("Apply"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	pButtonOk->SetForegroundColour(ColorGreenLite);
	pTmpSizer->Add(pButtonOk, 0, wxALL, 5);

	// Create the cancel button
	wxButton *pButtonCancel = new wxButton(this, ID_BUTTONCANCEL, GetText("Cancel"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	pButtonCancel->SetForegroundColour(ColorRedLite);
	pButtonCancel->SetFocus();
	pTmpSizer->Add(pButtonCancel, 0, wxALL, 5);

	pMainSizer->Add(pTmpSizer, 0, wxALIGN_CENTRE | wxALL, 10);	

	SetIcon(wxICON(IcoMain));

	this->SetSizerAndFit(pMainSizer);
	this->CentreOnParent();
}


/////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////
CConfigDialog::~CConfigDialog()
{
}


/////////////////////////////////////////////////////////////
// Close the dialog
/////////////////////////////////////////////////////////////
void CConfigDialog::OnClose(wxCloseEvent& /*event*/)
{
	EndModal(1);
}


/////////////////////////////////////////////////////////////
// Click on the Ok button, check and save config
/////////////////////////////////////////////////////////////
void CConfigDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
	// Save all values
	CConfig::GetSingleton()->SetLanguage(m_pLanguage->GetValue());
	CConfig::GetSingleton()->SetSoundFlag(m_pPlaySounds->GetValue());

	if(m_pRecordOptions->GetValue() == "USB+PC")
	{
		CConfig::GetSingleton()->SetRecordOption(2);
	}
	else if(m_pRecordOptions->GetValue() == "PC")
	{
		CConfig::GetSingleton()->SetRecordOption(3);
	}
	else
	{
		CConfig::GetSingleton()->SetRecordOption(1);
	}

	if(m_pCodec->GetValue() == "720")
	{
		CConfig::GetSingleton()->SetVideoCodec(Codec_H264_720P);
	}
	else
	{
		CConfig::GetSingleton()->SetVideoCodec(Codec_H264_360P);
	}

	if(m_pHudStyle->GetValue() == "HUD1")
	{
		CConfig::GetSingleton()->SetHudStyle(1);
	}
	else
	{
		CConfig::GetSingleton()->SetHudStyle(2);
	}

	CConfig::GetSingleton()->SetHudColor(m_pHudColor->GetColour());
	CConfig::GetSingleton()->SetHudLineSize(ToLong(m_pHudLineSize->GetValue()));
	CConfig::GetSingleton()->SetUsUnitFlag(m_pUSUnit->GetValue());
	CConfig::GetSingleton()->SetLowResFlag(m_pUseLowRes->GetValue());
	CConfig::GetSingleton()->SetSpeedLimit(m_pMaxSpeed->GetValue());
	CConfig::GetSingleton()->SetAccelerationLimit(m_pMaxAcceleration->GetValue());
	CConfig::GetSingleton()->SetAltitudeLimit(m_pMaxAltitude->GetValue());

	// Save to file
	CConfig::GetSingleton()->SaveConfig();

	EndModal(0);
}


/////////////////////////////////////////////////////////////
// Click on the cancel button closes the dialog,
// changes are lost
/////////////////////////////////////////////////////////////
void CConfigDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(1);
}