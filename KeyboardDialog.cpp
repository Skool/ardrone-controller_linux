//////////////////////////////////////////////////////////////////////////////
// Configuration dialog for keyboard input
//////////////////////////////////////////////////////////////////////////////

#include "KeyboardDialog.h"

#include "Utils.h"

BEGIN_EVENT_TABLE(CKeyboardDialog, wxDialog)
	EVT_CLOSE(CKeyboardDialog::OnClose)
	EVT_CHAR_HOOK(CKeyboardDialog::OnKey)
	EVT_BUTTON(ID_BUTTONOK, CKeyboardDialog::OnOk)
	EVT_BUTTON(ID_BUTTONCANCEL, CKeyboardDialog::OnCancel)
END_EVENT_TABLE()


/////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////
CKeyboardDialog::CKeyboardDialog(wxWindow *parent, CInput* pInput): wxDialog(parent, 1, "ConfigKeyboard", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
{	
	this->SetTitle(GetText("KeyboardConfig"));
	this->SetFont(FontNormal10);
	//this->SetForegroundColour(ColorBlueLite);	

	// Check parameter error
	if(NULL == pInput)
	{
		DoLog("Keyboard dialog has been called without Input object !", MSG_ERROR);
		return;
	}
	m_pInput = pInput;

	// No key selected yet
	m_iKey = -1;

	// Get the assigned key values
	wxKeyCode kUp, kDown;
	for(int i=0; i<4; i++)
	{
		m_pInput->GetKeysControl((eDirection)i, kDown, kUp);
		m_pKeys[i*2]=kUp;
		m_pKeys[i*2+1]=kDown;
	}

	// Create the main sizer
	wxBoxSizer*	pMainSizer = new wxBoxSizer(wxVERTICAL);
	pMainSizer->AddSpacer(10);

	wxString str = GetText("SelectKeyWarning");
	str.Replace("##", "\n\t");
	wxStaticText* pTitle = new wxStaticText(this, wxID_ANY, str, wxDefaultPosition, wxDefaultSize);
	pTitle->Wrap(400);
	pMainSizer->Add(pTitle, 0, wxEXPAND | wxALL, 15);

	pMainSizer->AddSpacer(15);

	// Create a flex grid sizer to display the graphical elements
	wxFlexGridSizer* pControlsSizer = new wxFlexGridSizer(8, 2, 5, 10);

	// Allow the columns to grow
	pControlsSizer->AddGrowableCol(0);
	pControlsSizer->AddGrowableCol(1);	

	wxString strLbl;
	wxSize ButMinSize(200, 25);

	for(int j=0; j<8; j++)
	{
		switch(j)
		{
			case 0:		strLbl = GetText("KeyMoveFwd");			break;
			case 1:		strLbl = GetText("KeyMoveBwd");			break;
			case 2:		strLbl = GetText("KeyMoveR");			break;
			case 3:		strLbl = GetText("KeyMoveL");			break;
			case 4:		strLbl = GetText("KeyMoveTR");			break;
			case 5:		strLbl = GetText("KeyMoveTL");			break;
			case 6:		strLbl = GetText("KeyMoveUp");			break;
			case 7:		strLbl = GetText("KeyMoveDown");		break;
			default:	strLbl = "";							break;
		}

		// Label to show the help text
		m_pLabels[j] = new wxStaticText(this, wxID_ANY, strLbl, wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT);
		pControlsSizer->Add(m_pLabels[j], 0, wxALIGN_CENTER_VERTICAL | wxEXPAND, 0);

		// Button to click
		m_pButton[j] = new wxButton(this, 2000+j, GetLabelString(j));
		m_pButton[j]->SetMinSize(ButMinSize);
		pControlsSizer->Add(m_pButton[j], 0, wxALIGN_CENTER | wxEXPAND, 0);
	}	
	pMainSizer->Add(pControlsSizer, 0, wxALIGN_CENTRE | wxEXPAND | wxALL, 15);

	// Create an horizontal sizer as we need to set 2 controls
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

	// Connect the buttons	
	Connect(2000, 2007, wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(CKeyboardDialog::OnButton));
	//this->SetFocus();

	this->SetSizerAndFit(pMainSizer);
	this->CentreOnParent();
}


/////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////
CKeyboardDialog::~CKeyboardDialog()
{
	Disconnect(2000, 2007, wxEVT_COMMAND_BUTTON_CLICKED);
}


/////////////////////////////////////////////////////////////
// Close the dialog
/////////////////////////////////////////////////////////////
void CKeyboardDialog::OnClose(wxCloseEvent& /*event*/)
{
	EndModal(1);
}


/////////////////////////////////////////////////////////////
// Click on the Ok button, check and save keyboard config
/////////////////////////////////////////////////////////////
void CKeyboardDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{		
	for(int i = 0; i<8; i++)
	{
		// Special key check
		if(m_pInput->IsSpecialKey(m_pKeys[i]))
		{
			DoMessage(GetText("ForbiddenKey"), MSG_ERROR);
			return;
		}

		// Check if a key has already been used for another direction
		for(int j=0; j<8; j++)
		{
			if(i != j)
			{
				if(m_pKeys[i] == m_pKeys[j])
				{
					DoMessage(GetText("RedundantKey"), MSG_ERROR);
					return;
				}
			}
		}
	}

	// Now set the keys to the input object
	for(int iDir=0; iDir<4; iDir++)
	{
		m_pInput->SetKeysControl((eDirection)iDir, m_pKeys[iDir*2+1], m_pKeys[iDir*2]);
	}

	EndModal(0);
}


/////////////////////////////////////////////////////////////
// Click on the cancel button closes the dialog,
// changes are lost
/////////////////////////////////////////////////////////////
void CKeyboardDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(1);
}


/////////////////////////////////////////////////////////////////////////////
// Set a key
/////////////////////////////////////////////////////////////////////////////
void CKeyboardDialog::OnButton(wxCommandEvent& event)
{
	// Another button has been pressed before, and no key has been 
	// pressed for it so reset the label
	if( (m_iKey > -1) && (m_iKey < 8) )
	{
		m_pButton[m_iKey]->SetLabel(GetLabelString(m_iKey));
	}

	// Get the button
	wxButton *pButton = (wxButton*)event.GetEventObject();
	// Get the ID of the button
	m_iKey = (pButton->GetId() - 2000);
	// Set the text
	m_pButton[m_iKey]->SetLabel(GetText("AssignKey"));
}


/////////////////////////////////////////////////////////////////////////////
// Pressed a key
/////////////////////////////////////////////////////////////////////////////
void CKeyboardDialog::OnKey(wxKeyEvent& event)
{
	if(m_iKey >= 0 && m_iKey < 8)
	{
		m_pKeys[m_iKey] = (wxKeyCode)event.GetKeyCode();
		m_pButton[m_iKey]->SetLabel(GetLabelString(m_iKey));
		m_iKey = -1;
	}
	else
	{
		event.Skip();
	}
}


/////////////////////////////////////////////////////////////////////////////
// Get the formatted text to display on the button
/////////////////////////////////////////////////////////////////////////////
wxString CKeyboardDialog::GetLabelString(int iButtonId)
{
	return wxString::Format("%d [%c]", (int)m_pKeys[iButtonId], (wxChar16)m_pKeys[iButtonId]);
}
