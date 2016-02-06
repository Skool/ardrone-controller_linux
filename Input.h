/////////////////////////////////////////////////////////////////////////////
// Manage input : Handles all key states and joystick positions
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wx/wx.h"
#include "InputDirection.h"

// Enum for observed keys
enum eKey:unsigned __int16
{
	KEY_NONE			= 0,
	KEY_EMERGENCY		= 1,
	KEY_TAKEOFF			= 2,
	KEY_CAMERA			= 4,
	KEY_FULLSCREEN		= 8,
	KEY_RECORD			= 16,
	KEY_SCREENSHOT		= 32,	// Note: handled in Render(), not in Entry()
	KEY_TRIM			= 64,
	KEY_RETURNHOME		= 128
};

// Indexes for the 4 available directions
enum eDirection
{
	IDX_FORWARD		= 0,
	IDX_SIDE		= 1,
	IDX_TURN		= 2,
	IDX_ALTITUDE	= 3
};

// Enum to identify what is to write
enum eInputFileType
{
	FILE_NONE		= 0,
	FILE_KEYBOARD	= 1,
	FILE_JOYSTICK	= 2,
};

class CInput
{
public:
	// Constructor and destructor
	CInput();
	~CInput();

	// Set speed and acceleration limits
	void SetLimits(double dSpeedLimit, double dAccelerationLimit);

	// Refresh key states
	void Update(long lTimeNew);
	// Check if an update is needed
	bool IsUpdateNeeded();

	// Check joystick presence
	bool HasJoystick();
	// Get pointer to joystick
	CJoystick* GetJoystick();
	// Is a joystick button pressed
	bool IsJoystickButtonPressed(eKey Key);

	// Load settings from file
	bool LoadFromFile(eInputFileType FileType);
	// Save current settings to file
	bool SaveToFile(eInputFileType FileType);

	// As long keybord input has not been saved we need to adapt keyboard layout
	void RefreshLayout();

	// Get the current vector for a specific direction
	const double& GetDirectionValue(eDirection Direction);

	// Get current FPS
	long GetFPS();	

	// Set a key to "pressed"
	void SetKey(eKey Key);
	// Reset a key to "not pressed"
	void ResetKey(eKey Key);
	// Check key status
	bool IsPressed(eKey Key);

	// Set a flag
	void SetFlag(eKey Key);
	// Reset a flag
	void ResetFlag(eKey Key);
	// Check for flag
	bool HasFlag(eKey Key);

	// Set the joystick axe to use to control the direction
	bool SetJoystickControl(eDirection Direction, eJoystickAxis JoystickAxeToUse, bool bIsInverted);
	// Has the direction the inverted flag
	bool HasInvertedFlag(eDirection Direction);
	// Assigned joystick axe for direction
	eJoystickAxis GetJoystickAxe(eDirection Direction);
	// Set the keys to use for a direction
	void SetKeysControl(eDirection Direction, wxKeyCode KeyValueDown, wxKeyCode KeyValueUp);
	// Get current keys used for a direction
	void GetKeysControl(eDirection Direction, wxKeyCode& KeyValueDown, wxKeyCode& KeyValueUp);

	// Set a button id to an action
	void AssignButtonToAction(int iButton, eKey Action);
	// Get button value as string
	wxString GetButtonAsString(eKey Button);

	// Does a KeyCode correspond to a special key ?
	bool IsSpecialKey(wxKeyCode Key);

private:
	// Get a value as string
	wxString GetValueAsString(int iValue);

	// Current key status (pressed or not)
	unsigned short		m_usKeyStatus;
	// Current flags (operation done or not)
	unsigned short		m_usFlagStatus;

	// Max speed (The drone accept speeds from -1.0 to 1.0)
	double				m_dMaxSpeed;
	// Max acceleration speed
	double				m_dMaxAcceleration;
	// Max decceleration speed
	double				m_dMaxDecceleration;

	// Last FPS
	long				m_lFps;	
	// Last time
	long				m_lTime;

	// Joystick controller
	CJoystick*			m_pJoystick;
	// Controls of all axes
	CInputDirection		m_Directions[4];

	// Code of assigned keys
	wxKeyCode			m_kEmergency;
	wxKeyCode			m_kTakeOff;
	wxKeyCode			m_kCamera;	
	wxKeyCode			m_kFullscreen;
	wxKeyCode			m_kRecord;
	wxKeyCode			m_kScreenshot;
	wxKeyCode			m_kReturnHome;

	// Index of assigned joystick button
	int					m_iEmergency;
	int					m_iTakeOff;
	int					m_iCamera;
	int					m_iFullscreen;
	int					m_iRecord;
	int					m_iScreenshot;
	int					m_iReturnHome;

	// Critical section to protect data (set from one thread, read from another)
	wxCriticalSection	m_CSInput;
};