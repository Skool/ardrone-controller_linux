//////////////////////////////////////////////////////////////////////////////
// Configuration dialog for joysticks
//////////////////////////////////////////////////////////////////////////////

#include "JoystickDialog.h"

#include "Utils.h"

BEGIN_EVENT_TABLE(CJoystickDialog, wxDialog)
	EVT_CLOSE(CJoystickDialog::OnClose)
	EVT_TIMER(TIMER_UPDATE, CJoystickDialog::OnTimerUpdate)
	EVT_BUTTON(ID_BUTTONOK, CJoystickDialog::OnOk)
	EVT_BUTTON(ID_BUTTONCANCEL, CJoystickDialog::OnCancel)
END_EVENT_TABLE()


// Number of options that can be assigned to joystick buttons (Record, land, emergency...)
// Update if you add (or remove) an option
const int iNumberOfOptions = 7;


/////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////
CJoystickDialog::CJoystickDialog(wxWindow *parent, CInput* pInput): wxDialog(parent, 1, "ConfigJoystick", wxDefaultPosition, wxDefaultSize, wxCAPTION | wxSYSTEM_MENU | wxCLOSE_BOX)
{	
	this->SetTitle(GetText("JoystickConfig"));
	this->SetFont(FontNormal10);
	//this->SetForegroundColour(ColorBlueLite);

	// Check parameter error
	if(NULL == pInput)
	{
		DoLog("Joystick dialog has been called without Input object !", MSG_ERROR);
		return;
	}
	m_pInput = pInput;
	
	// Joystick must exist
	CJoystick* pJoystick = m_pInput->GetJoystick();
	if(NULL == pJoystick)
	{
		DoLog("Joystick dialog has been called, but not joystick has been found !", MSG_ERROR);
		return;
	}

	// Create an array of available axis
	wxArrayString arAxis;
	arAxis.Add(CJoystick::GetAxisName(AXIS_NONE));
	arAxis.Add(CJoystick::GetAxisName(AXIS_X));
	arAxis.Add(CJoystick::GetAxisName(AXIS_Y));
	if(pJoystick->HasAxis(AXIS_RUDDER))
	{
		arAxis.Add(CJoystick::GetAxisName(AXIS_RUDDER));
	}
	if(pJoystick->HasAxis(AXIS_U))
	{
		arAxis.Add(CJoystick::GetAxisName(AXIS_U));
	}
	if(pJoystick->HasAxis(AXIS_V))
	{
		arAxis.Add(CJoystick::GetAxisName(AXIS_V));
	}
	if(pJoystick->HasAxis(AXIS_Z))
	{
		arAxis.Add(CJoystick::GetAxisName(AXIS_Z));
	}

	// Create the main sizer
	wxBoxSizer*	pMainSizer = new wxBoxSizer(wxVERTICAL);
	pMainSizer->AddSpacer(10);

	wxStaticBox *AxisBox = new wxStaticBox(this, wxID_ANY, GetText("Axis"));
	wxStaticBoxSizer * AxisBoxSizer = new wxStaticBoxSizer (AxisBox, wxVERTICAL);

	// Create a flex grid sizer to display axis values
	wxFlexGridSizer* pAxisValues = new wxFlexGridSizer(2, 6, 2, 2);

	// Allow the columns to grow
	pAxisValues->AddGrowableCol(0);
	pAxisValues->AddGrowableCol(1);
	pAxisValues->AddGrowableCol(2);
	pAxisValues->AddGrowableCol(3);
	pAxisValues->AddGrowableCol(4);
	pAxisValues->AddGrowableCol(5);

	wxSize LabelSize(100, 25);

	// Create the labels
	m_pAxeLabels[0] = new wxTextCtrl(this, wxID_ANY, CJoystick::GetAxisName(AXIS_X), wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	pAxisValues->Add(m_pAxeLabels[0], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[1] = new wxTextCtrl(this, wxID_ANY, CJoystick::GetAxisName(AXIS_Y), wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	pAxisValues->Add(m_pAxeLabels[1], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[2] = new wxTextCtrl(this, wxID_ANY, CJoystick::GetAxisName(AXIS_RUDDER), wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	pAxisValues->Add(m_pAxeLabels[2], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[3] = new wxTextCtrl(this, wxID_ANY, CJoystick::GetAxisName(AXIS_U), wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	pAxisValues->Add(m_pAxeLabels[3], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[4] = new wxTextCtrl(this, wxID_ANY, CJoystick::GetAxisName(AXIS_V), wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	pAxisValues->Add(m_pAxeLabels[4], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[5] = new wxTextCtrl(this, wxID_ANY, CJoystick::GetAxisName(AXIS_Z), wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	pAxisValues->Add(m_pAxeLabels[5], 0, wxALIGN_CENTER, 0);

	// Set grey backgroundcolor for the first line
	for(int i=0; i<6; i++)
	{
		m_pAxeLabels[i]->SetBackgroundColour(ColorGrey);
	}

	// Create the labels for value
	m_pAxeLabels[6] = new wxTextCtrl(this, wxID_ANY, "?", wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	if(pJoystick->HasAxis(AXIS_X))
	{
		m_pAxeLabels[6]->SetBackgroundColour(ColorGreenLite);
	}
	else
	{
		m_pAxeLabels[6]->SetBackgroundColour(ColorRedLite);
	}
	pAxisValues->Add(m_pAxeLabels[6], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[7] = new wxTextCtrl(this, wxID_ANY, "?", wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	if(pJoystick->HasAxis(AXIS_Y))
	{
		m_pAxeLabels[7]->SetBackgroundColour(ColorGreenLite);
	}
	else
	{
		m_pAxeLabels[7]->SetBackgroundColour(ColorRedLite);
	}
	pAxisValues->Add(m_pAxeLabels[7], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[8] = new wxTextCtrl(this, wxID_ANY, "?", wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	if(pJoystick->HasAxis(AXIS_RUDDER))
	{
		m_pAxeLabels[8]->SetBackgroundColour(ColorGreenLite);
	}
	else
	{
		m_pAxeLabels[8]->SetBackgroundColour(ColorRedLite);
	}
	pAxisValues->Add(m_pAxeLabels[8], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[9] = new wxTextCtrl(this, wxID_ANY, "?", wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	if(pJoystick->HasAxis(AXIS_U))
	{
		m_pAxeLabels[9]->SetBackgroundColour(ColorGreenLite);
	}
	else
	{
		m_pAxeLabels[9]->SetBackgroundColour(ColorRedLite);
	}
	pAxisValues->Add(m_pAxeLabels[9], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[10] = new wxTextCtrl(this, wxID_ANY, "?", wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	if(pJoystick->HasAxis(AXIS_V))
	{
		m_pAxeLabels[10]->SetBackgroundColour(ColorGreenLite);
	}
	else
	{
		m_pAxeLabels[10]->SetBackgroundColour(ColorRedLite);
	}
	pAxisValues->Add(m_pAxeLabels[10], 0, wxALIGN_CENTER, 0);

	m_pAxeLabels[11] = new wxTextCtrl(this, wxID_ANY, "?", wxDefaultPosition, LabelSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
	if(pJoystick->HasAxis(AXIS_Z))
	{
		m_pAxeLabels[11]->SetBackgroundColour(ColorGreenLite);
	}
	else
	{
		m_pAxeLabels[11]->SetBackgroundColour(ColorRedLite);
	}
	pAxisValues->Add(m_pAxeLabels[11], 0, wxALIGN_CENTER, 0);

	// Add to axis sizer
	AxisBoxSizer->Add(pAxisValues, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);		
	AxisBoxSizer->AddSpacer(20);

	AxisBoxSizer->Add(new wxStaticText(this, wxID_ANY, GetText("AxisSelection")), 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	
	// Create another flex grid sizer for selection
	wxFlexGridSizer* pAxisSelection = new wxFlexGridSizer(4, 3, 2, 2);

	// Allow the columns to grow
	pAxisSelection->AddGrowableCol(0);
	pAxisSelection->AddGrowableCol(1);
	pAxisSelection->AddGrowableCol(2);

	// Create the selection
	pAxisSelection->Add(new wxStaticText(this, wxID_ANY, GetText("AxeFwd"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pAxisSelection->Add(m_pAxes[IDX_FORWARD] = new wxComboBox(this, wxID_ANY, CJoystick::GetAxisName(m_pInput->GetJoystickAxe(IDX_FORWARD)), wxDefaultPosition, wxDefaultSize, arAxis, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);
	pAxisSelection->Add(m_pInverted[IDX_FORWARD] = new wxCheckBox(this, wxID_ANY, GetText("Invert")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 50);
	m_pInverted[IDX_FORWARD]->SetValue(m_pInput->HasInvertedFlag(IDX_FORWARD));

	pAxisSelection->Add(new wxStaticText(this, wxID_ANY, GetText("AxeSide"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pAxisSelection->Add(m_pAxes[IDX_SIDE] = new wxComboBox(this, wxID_ANY, CJoystick::GetAxisName(m_pInput->GetJoystickAxe(IDX_SIDE)), wxDefaultPosition, wxDefaultSize, arAxis, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);
	pAxisSelection->Add(m_pInverted[IDX_SIDE] = new wxCheckBox(this, wxID_ANY, GetText("Invert")), 0,wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 50);
	m_pInverted[IDX_SIDE]->SetValue(m_pInput->HasInvertedFlag(IDX_SIDE));

	pAxisSelection->Add(new wxStaticText(this, wxID_ANY, GetText("AxeTurn"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pAxisSelection->Add(m_pAxes[IDX_TURN] = new wxComboBox(this, wxID_ANY, CJoystick::GetAxisName(m_pInput->GetJoystickAxe(IDX_TURN)), wxDefaultPosition, wxDefaultSize, arAxis, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);
	pAxisSelection->Add(m_pInverted[IDX_TURN] = new wxCheckBox(this, wxID_ANY, GetText("Invert")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 50);
	m_pInverted[IDX_TURN]->SetValue(m_pInput->HasInvertedFlag(IDX_TURN));

	pAxisSelection->Add(new wxStaticText(this, wxID_ANY, GetText("AxeAlt"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pAxisSelection->Add(m_pAxes[IDX_ALTITUDE] = new wxComboBox(this, wxID_ANY, CJoystick::GetAxisName(m_pInput->GetJoystickAxe(IDX_ALTITUDE)), wxDefaultPosition, wxDefaultSize, arAxis, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);
	pAxisSelection->Add(m_pInverted[IDX_ALTITUDE] = new wxCheckBox(this, wxID_ANY, GetText("Invert")), 0, wxALIGN_RIGHT | wxALIGN_CENTER_VERTICAL, 50);
	m_pInverted[IDX_ALTITUDE]->SetValue(m_pInput->HasInvertedFlag(IDX_ALTITUDE));

	// Add to axis sizer
	AxisBoxSizer->Add(pAxisSelection, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	pMainSizer->Add(AxisBoxSizer, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);
	pMainSizer->AddSpacer(15);


	// Create the button box and sizer
	wxStaticBox *ButtonBox = new wxStaticBox(this, wxID_ANY, GetText("Buttons"));
	wxStaticBoxSizer * ButtonBoxSizer = new wxStaticBoxSizer (ButtonBox, wxVERTICAL);	

	// Get number of buttons
	m_iButtons = pJoystick->GetNumberButtons();
	//m_ppButtonLabels = new wxStaticText*[m_iButtons];
	m_ppButtonLabels = new wxTextCtrl*[m_iButtons];

	// Create an array of available buttons
	wxArrayString arButtons;
	arButtons.Add("None");
	for(int i=1; i<=m_iButtons; i++)
	{
		arButtons.Add(wxString::Format("%d", i));
	}

	wxSize LabelButtonSize(50, 25);

	// Create an horizontal sizer to display button states
	wxBoxSizer* pButtonStates = new wxBoxSizer(wxHORIZONTAL);	

	// Fill the flex grid sizer with button labels
	for(int i=0; i<m_iButtons; i++)
	{
		m_ppButtonLabels[i] = new wxTextCtrl(this, wxID_ANY, wxString::Format("B %d", i+1), wxDefaultPosition, LabelButtonSize, wxTE_CENTRE | wxTE_READONLY | wxBORDER_SIMPLE);
		m_ppButtonLabels[i]->SetBackgroundColour(ColorGrey);
		pButtonStates->Add(m_ppButtonLabels[i], 0, wxEXPAND | wxALL, 5);
	}	

	ButtonBoxSizer->Add(pButtonStates, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);	
	ButtonBoxSizer->AddSpacer(20);

	ButtonBoxSizer->Add(new wxStaticText(this, wxID_ANY, GetText("ButtonSelection")), 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	// Add the comboboxes
	wxFlexGridSizer* pButtonSelection = new wxFlexGridSizer(iNumberOfOptions, 2, 2, 2);

	// Allow the columns to grow
	pButtonSelection->AddGrowableCol(0);
	pButtonSelection->AddGrowableCol(1);

	// Create the selection
	pButtonSelection->Add(new wxStaticText(this, wxID_ANY, GetText("Emergency"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pButtonSelection->Add(m_pButtons[0] = new wxComboBox(this, wxID_ANY, pInput->GetButtonAsString(KEY_EMERGENCY), wxDefaultPosition, wxDefaultSize, arButtons, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);	

	pButtonSelection->Add(new wxStaticText(this, wxID_ANY, GetText("LandTakeOff"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pButtonSelection->Add(m_pButtons[1] = new wxComboBox(this, wxID_ANY, pInput->GetButtonAsString(KEY_TAKEOFF), wxDefaultPosition, wxDefaultSize, arButtons, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);	

	pButtonSelection->Add(new wxStaticText(this, wxID_ANY, GetText("Camera"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pButtonSelection->Add(m_pButtons[2] = new wxComboBox(this, wxID_ANY, pInput->GetButtonAsString(KEY_CAMERA), wxDefaultPosition, wxDefaultSize, arButtons, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);	

	pButtonSelection->Add(new wxStaticText(this, wxID_ANY, GetText("Fullscreen"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pButtonSelection->Add(m_pButtons[3] = new wxComboBox(this, wxID_ANY, pInput->GetButtonAsString(KEY_FULLSCREEN), wxDefaultPosition, wxDefaultSize, arButtons, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);	

	pButtonSelection->Add(new wxStaticText(this, wxID_ANY, GetText("Record"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pButtonSelection->Add(m_pButtons[4] = new wxComboBox(this, wxID_ANY, pInput->GetButtonAsString(KEY_RECORD), wxDefaultPosition, wxDefaultSize, arButtons, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);	

	pButtonSelection->Add(new wxStaticText(this, wxID_ANY, GetText("Screenshot"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pButtonSelection->Add(m_pButtons[5] = new wxComboBox(this, wxID_ANY, pInput->GetButtonAsString(KEY_SCREENSHOT), wxDefaultPosition, wxDefaultSize, arButtons, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);	

	pButtonSelection->Add(new wxStaticText(this, wxID_ANY, GetText("Home"), wxDefaultPosition, wxDefaultSize, wxALIGN_LEFT), 0, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL, 50);
	pButtonSelection->Add(m_pButtons[6] = new wxComboBox(this, wxID_ANY, pInput->GetButtonAsString(KEY_RETURNHOME), wxDefaultPosition, wxDefaultSize, arButtons, wxCB_READONLY), 0, wxALIGN_CENTER_HORIZONTAL | wxALIGN_CENTER_VERTICAL | wxLEFT | wxRIGHT, 10);	

	ButtonBoxSizer->Add(pButtonSelection, 0, wxALIGN_CENTER_HORIZONTAL | wxALL, 5);

	pMainSizer->Add(ButtonBoxSizer, 0, wxEXPAND | wxALL, 5);
	pMainSizer->AddSpacer(15);

	// Create an horizontal sizer as we need to set 2 buttons in one line
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

	// Init the timer to 1 second
	m_Timer.SetOwner(this, TIMER_UPDATE);
	m_Timer.Start(50);
}


/////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////
CJoystickDialog::~CJoystickDialog()
{
	m_Timer.Stop();
}


/////////////////////////////////////////////////////////////
// Close the dialog
/////////////////////////////////////////////////////////////
void CJoystickDialog::OnClose(wxCloseEvent& /*event*/)
{
	EndModal(1);
}


/////////////////////////////////////////////////////////////
// Click on the Ok button, check and save joystick config
/////////////////////////////////////////////////////////////
void CJoystickDialog::OnOk(wxCommandEvent& WXUNUSED(event))
{	
	unsigned short			usUsedAxis		= 0;			// List of used axis
	eJoystickAxis			JoystickAxe		= AXIS_NONE;	// Last axis 
	eJoystickAxis			AxisToSet[4];					// List of joysticks axis for each direction

	// Control if no axis is selected 2 times
	for(int i=0; i<4; i++)
	{
		// Get selected joystick axis
		JoystickAxe = CJoystick::GetAxisIndex(m_pAxes[i]->GetValue());
		if( (AXIS_NONE != JoystickAxe) && (usUsedAxis & JoystickAxe) )
		{
			DoMessage(GetText("AxeMultiAssign"), MSG_ERROR);
			return;
		}
		else
		{
			usUsedAxis |= JoystickAxe;
			AxisToSet[i] = JoystickAxe;
		}
	}

	// Control done, set values (get inverted flag from checkbox)
	for(int j=0; j<4; j++)
	{
		m_pInput->SetJoystickControl((eDirection)j, AxisToSet[j], m_pInverted[j]->GetValue());
	}

	// Check if no button is selected twice
	for(int k=0; k<iNumberOfOptions; k++)
	{
		for(int l=0; l<iNumberOfOptions; l++)
		{
			if( (k!=l) && (m_pButtons[k]->GetValue() != "None") )
			{
				if(m_pButtons[k]->GetValue() == m_pButtons[l]->GetValue())
				{
					DoMessage(GetText("ButtonMultiAssign"), MSG_ERROR);
					return;
				}
			}
		}
	}

	// Save the values
	m_pInput->AssignButtonToAction(ToLong(m_pButtons[0]->GetValue())-1, KEY_EMERGENCY);
	m_pInput->AssignButtonToAction(ToLong(m_pButtons[1]->GetValue())-1, KEY_TAKEOFF);
	m_pInput->AssignButtonToAction(ToLong(m_pButtons[2]->GetValue())-1, KEY_CAMERA);
	m_pInput->AssignButtonToAction(ToLong(m_pButtons[3]->GetValue())-1, KEY_FULLSCREEN);
	m_pInput->AssignButtonToAction(ToLong(m_pButtons[4]->GetValue())-1, KEY_RECORD);
	m_pInput->AssignButtonToAction(ToLong(m_pButtons[5]->GetValue())-1, KEY_SCREENSHOT);
	m_pInput->AssignButtonToAction(ToLong(m_pButtons[6]->GetValue())-1, KEY_RETURNHOME);

	EndModal(0);
}


/////////////////////////////////////////////////////////////
// Click on the cancel button closes the dialog,
// changes are lost
/////////////////////////////////////////////////////////////
void CJoystickDialog::OnCancel(wxCommandEvent& WXUNUSED(event))
{
	EndModal(1);
}


/////////////////////////////////////////////////////////////
// Update values of joystick axis
/////////////////////////////////////////////////////////////
void CJoystickDialog::OnTimerUpdate(wxTimerEvent& WXUNUSED(event))
{	
	CJoystick* pJoystick = m_pInput->GetJoystick();
	if(NULL == pJoystick)
	{
		return;
	}
	
	// Refresh axis values
	if(pJoystick->HasAxis(AXIS_X))
	{
		m_pAxeLabels[6]->SetValue(ToString(pJoystick->GetRawValue(AXIS_X)));
	}
	if(pJoystick->HasAxis(AXIS_Y))
	{
		m_pAxeLabels[7]->SetValue(ToString(pJoystick->GetRawValue(AXIS_Y)));
	}
	if(pJoystick->HasAxis(AXIS_RUDDER))
	{
		m_pAxeLabels[8]->SetValue(ToString(pJoystick->GetRawValue(AXIS_RUDDER)));
	}
	if(pJoystick->HasAxis(AXIS_U))
	{
		m_pAxeLabels[9]->SetValue(ToString(pJoystick->GetRawValue(AXIS_U)));
	}
	if(pJoystick->HasAxis(AXIS_V))
	{
		m_pAxeLabels[10]->SetValue(ToString(pJoystick->GetRawValue(AXIS_V)));
	}
	if(pJoystick->HasAxis(AXIS_Z))
	{
		m_pAxeLabels[11]->SetValue(ToString(pJoystick->GetRawValue(AXIS_Z)));
	}

	// Refresh button states
	for(int i = 0; i<m_iButtons; i++)
	{
		if(pJoystick->GetButtonState(i))
		{			
			m_ppButtonLabels[i]->SetValue(wxString::Format(" B %d ", i+1));
			m_ppButtonLabels[i]->SetBackgroundColour(ColorGreenLite);
		}
		else
		{
			m_ppButtonLabels[i]->SetValue(wxString::Format("B %d", i+1));
			m_ppButtonLabels[i]->SetBackgroundColour(ColorGrey);
		}
	}
}