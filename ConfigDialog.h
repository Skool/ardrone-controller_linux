//////////////////////////////////////////////////////////////////////////////
// Configuration dialog to set application options
//////////////////////////////////////////////////////////////////////////////

#ifndef __HEADER_CONFIGDIALOG__
#define __HEADER_CONFIGDIALOG__

#include <wx/wx.h>
#include <wx/clrpicker.h>

// Declaration of the about dialog class
class CConfigDialog : public wxDialog
{
	private:
		DECLARE_EVENT_TABLE();
		
	public:
		// Constructor and destructor
		CConfigDialog(wxWindow *parent, bool bIsConnected);
		virtual ~CConfigDialog();

	private:
		void OnClose(wxCloseEvent& event);

		// Click on the Ok button, check and save config
		void OnOk(wxCommandEvent& event);
		// Click on the cancel button closes the dialog, changes are lost
		void OnCancel(wxCommandEvent& event);	
	
	// Controls of the dialog
	private:
		wxComboBox*			m_pLanguage;
		wxCheckBox*			m_pPlaySounds;
		wxComboBox*			m_pCodec;
		wxComboBox*			m_pRecordOptions;
		wxComboBox*			m_pHudStyle;
		wxColourPickerCtrl*	m_pHudColor;
		wxComboBox*			m_pHudLineSize;
		wxCheckBox*			m_pUSUnit;
		wxCheckBox*			m_pUseLowRes;
		wxSlider*			m_pMaxSpeed;
		wxSlider*			m_pMaxAcceleration;
		wxSlider*			m_pMaxAltitude;		

		enum
		{			
			ID_BUTTONOK			= 1010,
			ID_BUTTONCANCEL
		};
};

#endif