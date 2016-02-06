//////////////////////////////////////////////////////////////////////////////
// About dialog containing author, version and link to homepage
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wx/wx.h"

// Declaration of the about dialog class
class CAboutDialog : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		// Constructor and destructor
		CAboutDialog(wxWindow *parent);
		virtual ~CAboutDialog();

	private:
		void OnClose(wxCloseEvent& event);

		// Click on the Ok button closes the about box
		void OnOk(wxCommandEvent& event);	
		
	private:
		enum
		{
			ID_LINK				= 1006,
			ID_BUTTONOK			= 1004,			
		};
	
	private:
		
};
