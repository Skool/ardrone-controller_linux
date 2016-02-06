///////////////////////////////////////////////////////////
// Manages screens, needed to change resolution on full
// screen mode, on "slow" computers
///////////////////////////////////////////////////////////

#pragma once

#include <windows.h>

class CScreenManager
{
public:
	// Constructor and destructor
	CScreenManager();
	~CScreenManager();
	
	// Set low resolution (1366x768 or 1024x768)
	bool SetLowResolution();
	// Reset to "normal" resolution
	void ResetResolution();

private:
	// The main screen where the resolution has to be changed
	DISPLAY_DEVICE	m_MainDevice;
	// Initialisation status
	bool			m_bInit;
	// Is low resolution in use or not
	bool			m_bIsLowRes;
};