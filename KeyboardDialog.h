//////////////////////////////////////////////////////////////////////////////
// Configuration dialog for keyboard input
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wx/wx.h"

#include "Input.h"

// Declaration of the keyboard dialog class
class CKeyboardDialog : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		// Constructor and destructor
		CKeyboardDialog(wxWindow *parent, CInput* pInput);
		virtual ~CKeyboardDialog();

	private:
		void OnClose(wxCloseEvent& event);

		// Click on the Ok button, check and save config
		void OnOk(wxCommandEvent& event);
		// Click on the cancel button closes the dialog, changes are lost
		void OnCancel(wxCommandEvent& event);
		
		// Clicked on button to configure a key
		void OnButton(wxCommandEvent& event);
		// Pressed a key
		void OnKey(wxKeyEvent& event);

		// Get the formatted text to display on the button
		wxString GetLabelString(int iButtonId);
	
	// Controls of the dialog
	private:
		// Labels for direction
		wxStaticText*	m_pLabels[8];
		// Button for key selection
		wxButton*		m_pButton[8];		

		// Assigned values
		wxKeyCode		m_pKeys[8];
		// Input object
		CInput*			m_pInput;

		// Current key
		int				m_iKey;

		enum
		{			
			ID_BUTTONOK			= 1010,
			ID_BUTTONCANCEL		
		};
};
