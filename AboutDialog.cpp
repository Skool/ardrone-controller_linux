//////////////////////////////////////////////////////////////////////////////
// About dialog containing author, version and link to homepage
//////////////////////////////////////////////////////////////////////////////

#include "AboutDialog.h"
#include <wx/hyperlink.h>

#include "Utils.h"
#include "Ressources.h"

BEGIN_EVENT_TABLE(CAboutDialog, wxDialog)
	EVT_CLOSE(CAboutDialog::OnClose)
	EVT_BUTTON(ID_BUTTONOK, CAboutDialog::OnOk)
END_EVENT_TABLE()


/////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////
CAboutDialog::CAboutDialog(wxWindow *parent)
: wxDialog(parent, 1, "About", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxSYSTEM_MENU | wxDIALOG_NO_PARENT | wxCLOSE_BOX)
{
	this->SetTitle(GetText("About"));
	this->SetFont(FontNormal10);

	// Create the main sizer
	wxBoxSizer*	pMainSizer = new wxBoxSizer(wxVERTICAL);
	pMainSizer->AddSpacer(10);

	// Create an horizontal sizer for the bitmap an the version string
	wxBoxSizer*	pSubSizer1 = new wxBoxSizer(wxHORIZONTAL);

	// Create a little bitmap using the application icon
	wxStaticBitmap *pBitmap = new wxStaticBitmap(this, wxID_ANY, wxBitmap(wxICON(IcoMain)));	
	pBitmap->Enable(false);
	pSubSizer1->Add(pBitmap, 0, wxALIGN_CENTER_VERTICAL | wxALL, 10);

	// Create a vertical sizer for the strings
	wxBoxSizer*	pSubSizer2 = new wxBoxSizer(wxVERTICAL);
	
	// Create static text to put the informations
	wxStaticText* pText1 = new wxStaticText(this, wxID_ANY, VersionString, wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pSubSizer2->Add(pText1, 0, wxALIGN_CENTER_HORIZONTAL, 0);
	
	wxStaticText* pText2 = new wxStaticText(this, wxID_ANY, wxT("Patrick Christmann"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pSubSizer2->Add(pText2, 0, wxALIGN_CENTER_HORIZONTAL, 0);

	wxStaticText* pText3 = new wxStaticText(this, wxID_ANY, wxT("miniapps@free.fr"), wxDefaultPosition, wxDefaultSize, wxBORDER_NONE);
	pSubSizer2->Add(pText3, 0, wxALIGN_CENTER_HORIZONTAL, 0);

	// Add the text sizer to the sub sizer
	pSubSizer1->Add(pSubSizer2, 0, wxALIGN_CENTER_VERTICAL| wxALL, 10);

	// Add the sub sizer to the main sizer
	pMainSizer->Add(pSubSizer1, 0 , wxALIGN_CENTER_HORIZONTAL | wxLEFT | wxRIGHT, 15);

	// Create link to the homepage
	wxHyperlinkCtrl* pLink1 = new wxHyperlinkCtrl(this, wxID_ANY, wxT("http://miniapps.free.fr/"), wxT("http://miniapps.free.fr"), wxDefaultPosition, wxDefaultSize, wxNO_BORDER | wxHL_CONTEXTMENU);
	pLink1->SetNormalColour(ColorBlue);
	pLink1->SetFont(wxFont(wxSize(6,13), wxSWISS, wxNORMAL, wxNORMAL, true, wxT("MS Sans Serif")));	

	pMainSizer->Add(pLink1, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 10);

	// Create the ok button
	wxButton *pButton = new wxButton(this, ID_BUTTONOK, wxT("Ok"), wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE);
	pButton->SetForegroundColour(ColorGreenLite);
	pButton->SetFocus();
	pMainSizer->Add(pButton, 0, wxALIGN_CENTER_HORIZONTAL| wxALL, 10);

	pMainSizer->AddSpacer(10);

	SetIcon(wxICON(IcoMain));

	this->SetSizerAndFit(pMainSizer);
	this->CentreOnParent();
}


/////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////
CAboutDialog::~CAboutDialog()
{
} 


/////////////////////////////////////////////////////////////
// Close the dialog
/////////////////////////////////////////////////////////////
void CAboutDialog::OnClose(wxCloseEvent& /*event*/)
{
	EndModal(1);
}


/////////////////////////////////////////////////////////////
// Click on the Ok button closes the about box
/////////////////////////////////////////////////////////////
void CAboutDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{
	EndModal(0);
}