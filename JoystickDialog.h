//////////////////////////////////////////////////////////////////////////////
// Configuration dialog for joysticks
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wx/wx.h"

#include "Input.h"

// Declaration of the joystick dialog class
class CJoystickDialog : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		// Constructor and destructor
		CJoystickDialog(wxWindow *parent, CInput* pInput);
		virtual ~CJoystickDialog();

	private:
		void OnClose(wxCloseEvent& event);

		// Click on the Ok button, check and save config
		void OnOk(wxCommandEvent& event);
		// Click on the cancel button closes the dialog, changes are lost
		void OnCancel(wxCommandEvent& event);

		// Update values of joystick axis
		void OnTimerUpdate(wxTimerEvent& WXUNUSED(event));
	
	// Controls of the dialog
	private:
		// Labels for axe values
		wxTextCtrl*		m_pAxeLabels[12];
		// Comboboxes for axe selection
		wxComboBox*		m_pAxes[4];		
		// Checkboxes for inverted axes
		wxCheckBox*		m_pInverted[4];		
		
		// Labels for buttons states		
		wxTextCtrl**	m_ppButtonLabels;
		// Comboboxes for button selection
		wxComboBox*		m_pButtons[7];
		// Number of buttons
		int				m_iButtons;

		// Timer for value update
		wxTimer			m_Timer;
		// Input object
		CInput*			m_pInput;

		enum
		{			
			ID_BUTTONOK			= 1010,
			ID_BUTTONCANCEL,
			TIMER_UPDATE
		};	
};
