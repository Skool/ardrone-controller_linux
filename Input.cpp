/////////////////////////////////////////////////////////////////////////////
// Manage input : Handles all key states and joystick positions
/////////////////////////////////////////////////////////////////////////////

#include <wx/textfile.h>

#include "Input.h"
#include "AppConfig.h"
#include "Utils.h"

/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CInput::CInput()
{
	// Key status
	m_usKeyStatus		= KEY_NONE;
	m_usFlagStatus		= KEY_NONE;

	// FPS
	m_lFps				= 0;
	// Start timer
	m_lTime				= 0;

	// Max values
	m_dMaxSpeed			= 0.5f;
	m_dMaxAcceleration	= 0.5f;
	m_dMaxDecceleration	= 0.5f;

	// Set fonction keys	
	m_kEmergency		= WXK_ESCAPE;
	m_kTakeOff			= WXK_RETURN;
	m_kCamera			= wxKeyCode('C');
	m_kReturnHome		= wxKeyCode('H');
	m_kFullscreen		= WXK_CONTROL_F;
	m_kRecord			= WXK_CONTROL_R;
	m_kScreenshot		= WXK_CONTROL_P;

	// Init joystick buttons to -1 (Means none)
	m_iEmergency		= -1;
	m_iTakeOff			= -1;
	m_iCamera			= -1;
	m_iReturnHome		= -1;
	m_iFullscreen		= -1;
	m_iRecord			= -1;
	m_iScreenshot		= -1;
	
	// Init joystick
	m_pJoystick			= NULL;
	int iJoysticks = wxJoystick::GetNumberJoysticks();
	if(iJoysticks > 0)
	{
		DoLog(wxString::Format("Found %d joysticks connected", iJoysticks));
		if(iJoysticks > 1)
		{
			DoLog("Note: only the first joystick will be available !", MSG_WARNING);
		}
		m_pJoystick = new CJoystick();

		// Set default axes
		m_Directions[IDX_FORWARD].SetJoystick(AXIS_Y, m_pJoystick, true);
		m_Directions[IDX_SIDE].SetJoystick(AXIS_X, m_pJoystick, false);
	}

	// Set default keys for forward and side movements
	m_Directions[IDX_FORWARD].SetKeys(WXK_DOWN, WXK_UP);
	m_Directions[IDX_SIDE].SetKeys(WXK_LEFT, WXK_RIGHT);

	// Call refresh layout, which will set the right keys for altitude and yaw
	// depending on current language
	RefreshLayout();
	
	// Try to load config for keyboard and joystick
	LoadFromFile((eInputFileType)(FILE_KEYBOARD | FILE_JOYSTICK));
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
CInput::~CInput()
{
	if(NULL != m_pJoystick)
	{
		delete m_pJoystick;
		m_pJoystick = NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Set speed and acceleration limits
/////////////////////////////////////////////////////////////////////////////
void CInput::SetLimits(double dSpeedLimit, double dAccelerationLimit)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	m_dMaxSpeed = dSpeedLimit;
	m_dMaxAcceleration = dAccelerationLimit;
	m_dMaxDecceleration = dAccelerationLimit;
}


/////////////////////////////////////////////////////////////////////////////
// Refresh key states
// Return true if a function key (Emergency, Land...) has been pressed
/////////////////////////////////////////////////////////////////////////////
void CInput::Update(long lTimeNew)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	long lTimediff	= lTimeNew-m_lTime;

	// Avoid division by zero
	if(0 == lTimediff)
	{
		lTimediff = 1;
	}

	// calculate FPS
	m_lFps = 1000/lTimediff;

	// Calculate multiplicator (Drone movement speed should not depend on computer performance)
	double dDeltaTime =((double)lTimediff)/1000.0f;
	m_lTime = lTimeNew;

	double dAcceleration = dDeltaTime * m_dMaxAcceleration;
	double dDecceleration = dDeltaTime * m_dMaxDecceleration;

	// *Note*: Special keys have a double check, so if the user press a key, the action will only be done
	// one time, even if the key stay pressed down.

	// Emergency
	if( wxGetKeyState(m_kEmergency) || IsJoystickButtonPressed(KEY_EMERGENCY) )
	{
		if(!IsPressed(KEY_EMERGENCY))
		{
			SetKey(KEY_EMERGENCY);
			SetFlag(KEY_EMERGENCY);
		}
	}
	else if(IsPressed(KEY_EMERGENCY))
	{
		ResetKey(KEY_EMERGENCY);
	}
	
	// Fullscreen
	if( wxGetKeyState(m_kFullscreen) || IsJoystickButtonPressed(KEY_FULLSCREEN) )
	{
		if(!IsPressed(KEY_FULLSCREEN))
		{
			SetKey(KEY_FULLSCREEN);
			SetFlag(KEY_FULLSCREEN);
		}
	}
	else if(IsPressed(KEY_FULLSCREEN))
	{
		ResetKey(KEY_FULLSCREEN);
	}

	// Camera
	if( wxGetKeyState(m_kCamera)  || IsJoystickButtonPressed(KEY_CAMERA) )
	{
		if(!IsPressed(KEY_CAMERA))
		{
			SetKey(KEY_CAMERA);
			SetFlag(KEY_CAMERA);
		}
	}
	else if(IsPressed(KEY_CAMERA))
	{
		ResetKey(KEY_CAMERA);
	}

	// Take off / land
	if( wxGetKeyState(m_kTakeOff)  || IsJoystickButtonPressed(KEY_TAKEOFF) )
	{
		if(!IsPressed(KEY_TAKEOFF))
		{
			SetKey(KEY_TAKEOFF);
			SetFlag(KEY_TAKEOFF);
		}
	}
	else if(IsPressed(KEY_TAKEOFF))
	{
		ResetKey(KEY_TAKEOFF);
	}

	// Start/Stop recording
	if( wxGetKeyState(m_kRecord)  || IsJoystickButtonPressed(KEY_RECORD) )
	{
		if(!IsPressed(KEY_RECORD))
		{
			SetKey(KEY_RECORD);
			SetFlag(KEY_RECORD);
		}
	}
	else if(IsPressed(KEY_RECORD))
	{
		ResetKey(KEY_RECORD);
	}

	// Take a screenshot
	if( wxGetKeyState(m_kScreenshot)  || IsJoystickButtonPressed(KEY_SCREENSHOT) )
	{
		if(!IsPressed(KEY_SCREENSHOT))
		{
			SetKey(KEY_SCREENSHOT);
			SetFlag(KEY_SCREENSHOT);
		}
	}
	else if(IsPressed(KEY_SCREENSHOT))
	{
		ResetKey(KEY_SCREENSHOT);
	}

	// Return to home
	if( wxGetKeyState(m_kReturnHome)  || IsJoystickButtonPressed(KEY_RETURNHOME) )
	{
		if(!IsPressed(KEY_RETURNHOME))
		{
			SetKey(KEY_RETURNHOME);
			SetFlag(KEY_RETURNHOME);
		}
	}
	else if(IsPressed(KEY_RETURNHOME))
	{
		ResetKey(KEY_RETURNHOME);
	}

	// Refresh the values of all axes
	for(int i=0; i<4; i++)
	{
		m_Directions[i].UpdateDirection(dAcceleration, dDecceleration, m_dMaxSpeed);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Check if an update is needed
/////////////////////////////////////////////////////////////////////////////
bool CInput::IsUpdateNeeded()
{
	wxCriticalSectionLocker Lock(m_CSInput);

	return (0 != m_usFlagStatus);
}


/////////////////////////////////////////////////////////////////////////////
// Check joystick presence
/////////////////////////////////////////////////////////////////////////////
bool CInput::HasJoystick()
{
	wxCriticalSectionLocker Lock(m_CSInput);

	return (NULL != m_pJoystick);
}


/////////////////////////////////////////////////////////////////////////////
// Get pointer to joystick
/////////////////////////////////////////////////////////////////////////////
CJoystick* CInput::GetJoystick()
{
	wxCriticalSectionLocker Lock(m_CSInput);

	return m_pJoystick;
}


/////////////////////////////////////////////////////////////////////////////
// Is a joystick button pressed
/////////////////////////////////////////////////////////////////////////////
bool CInput::IsJoystickButtonPressed(eKey Key)
{		
	if(NULL == m_pJoystick)
	{
		return false;
	}

	bool bPressed = false;

	switch(Key)
	{
		case KEY_EMERGENCY:
		{
			if(m_iEmergency > -1)
			{
				bPressed = m_pJoystick->GetButtonState((unsigned int)m_iEmergency);
			}
		}
		break;

		case KEY_FULLSCREEN:
		{
			if(m_iFullscreen > -1)
			{
				bPressed = m_pJoystick->GetButtonState((unsigned int)m_iFullscreen);
			}
		}
		break;

		case KEY_CAMERA:
		{
			if(m_iCamera > -1)
			{
				bPressed = m_pJoystick->GetButtonState((unsigned int)m_iCamera);
			}
		}
		break;

		case KEY_TAKEOFF:
		{
			if(m_iTakeOff > -1)
			{
				bPressed = m_pJoystick->GetButtonState((unsigned int)m_iTakeOff);
			}
		}
		break;

		case KEY_RECORD:
		{
			if(m_iRecord > -1)
			{
				bPressed = m_pJoystick->GetButtonState((unsigned int)m_iRecord);
			}
		}
		break;

		case KEY_SCREENSHOT:
		{
			if(m_iScreenshot > -1)
			{
				bPressed = m_pJoystick->GetButtonState((unsigned int)m_iScreenshot);
			}
		}
		break;

		case KEY_RETURNHOME:
		{
			if(m_iReturnHome > -1)
			{
				bPressed = m_pJoystick->GetButtonState((unsigned int)m_iReturnHome);
			}
		}
		break;

		default:
			break;
	}

	return bPressed;
}


/////////////////////////////////////////////////////////////////////////////
// Load settings from file (Joystick and keyboard)
/////////////////////////////////////////////////////////////////////////////
bool CInput::LoadFromFile(eInputFileType FileType)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	bool bSuccess = true;

	if(FileType & FILE_KEYBOARD)
	{
		// Text file containing the Keyboard configuration
		wxTextFile KeyboardFile("Data\\Config\\Keyboard.ini");
	
		// Verify if the file exist
		if(KeyboardFile.Exists())
		{
			// If the file exist, open it
			if(KeyboardFile.Open())
			{
				// Temporary values
				wxKeyCode		kValueUp;
				wxKeyCode		kValueDown;

				// Iterate over the 4 axes
				for(int iDirection=0; iDirection<4; iDirection++)
				{
					// Set keyboard values to the corresponding axe
					kValueUp	= (wxKeyCode)ToLong(KeyboardFile.GetLine(iDirection*2));
					kValueDown	= (wxKeyCode)ToLong(KeyboardFile.GetLine(iDirection*2+1));
					m_Directions[iDirection].SetKeys(kValueDown, kValueUp);
				}

				// Close the file
				KeyboardFile.Close();
			}
			else
			{
				DoLog("Failed to restore the saved keyboard configuration !", MSG_ERROR);
				DoMessage(GetText("FailedRestoreKey"), MSG_ERROR);
				bSuccess = false;
			}
		}
	}

	if((FileType & FILE_JOYSTICK) && HasJoystick())
	{
		// The text file containing the joystick configuration
		wxTextFile JoystickFile("Data\\Config\\" + m_pJoystick->GetConfigFileName());
		// Verify if the file exist
		if(JoystickFile.Exists())
		{
			// If the file exist, open it
			if(JoystickFile.Open())
			{
				// Temporary values
				eJoystickAxis	JoystickAxe;
				bool			bInverted;

				// Iterate over the 4 axes
				for(int iDirection=0; iDirection<4; iDirection++)
				{					
					JoystickAxe = (eJoystickAxis)ToLong(JoystickFile.GetLine(iDirection*2));
					bInverted	= (JoystickFile.GetLine(iDirection*2+1) == "1") ? true:false;
					// Set axis and inverted flag (axis will also be checked for presence)
					m_Directions[iDirection].SetJoystick(JoystickAxe, this->GetJoystick(), bInverted);
				}

				
				m_iEmergency		= (int)ToLong(JoystickFile.GetLine(8));
				m_iTakeOff			= (int)ToLong(JoystickFile.GetLine(9));
				m_iCamera			= (int)ToLong(JoystickFile.GetLine(10));
				m_iFullscreen		= (int)ToLong(JoystickFile.GetLine(11));
				m_iRecord			= (int)ToLong(JoystickFile.GetLine(12));
				m_iScreenshot		= (int)ToLong(JoystickFile.GetLine(13));
				m_iReturnHome		= (int)ToLong(JoystickFile.GetLine(14));				

				// Close the file
				JoystickFile.Close();
			}
			else
			{
				DoLog("Failed to restore the saved joystick configuration !", MSG_ERROR);
				DoMessage(GetText("FailedRestoreJoy"), MSG_ERROR);
				bSuccess = false;
			}
		}
		else
		{
			// A joystick is connected, but no configuration file exist for it
			DoMessage(GetText("NoJoystickConfig"), MSG_INFO);
		}
	}
	
	return bSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// Save current settings to file (Joystick or keyboard)
/////////////////////////////////////////////////////////////////////////////
bool CInput::SaveToFile(eInputFileType FileType)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	if(FILE_KEYBOARD == FileType)
	{
		// Text file containing the Keyboard configuration
		wxTextFile KeyboardFile("Data\\Config\\Keyboard.ini");
	
		// Verify if the file exist
		if(KeyboardFile.Exists())
		{
			// If the file exist, open it
			if(!KeyboardFile.Open())
			{	
				return false;
			}
		}
		else
		{
			// Create the file
			if(!KeyboardFile.Create())
			{
				return false;	
			}
		}
	
		// Clear the file
		KeyboardFile.Clear();
	
		// Iterate over the 4 directions and save the keys
		for(int iDirection=0; iDirection<4; iDirection++)
		{
			KeyboardFile.AddLine(ToString((long)m_Directions[iDirection].GetKeyUp()));
			KeyboardFile.AddLine(ToString((long)m_Directions[iDirection].GetKeyDown()));
		}

		// Save the changes
		if(!KeyboardFile.Write())
		{
			return false;
		}
		// Close the file
		if(!KeyboardFile.Close())
		{
			return false;
		}
	}	
	else if((FILE_JOYSTICK == FileType) && HasJoystick())
	{
		// Text file containing the Keyboard configuration
		wxTextFile JoystickFile("Data\\Config\\" + m_pJoystick->GetConfigFileName());
	
		// Verify if the file exist
		if(JoystickFile.Exists())
		{
			// If the file exist, open it
			if(!JoystickFile.Open())
			{	
				return false;
			}
		}
		else
		{
			// Create the file
			if(!JoystickFile.Create())
			{
				return false;	
			}
		}
	
		// Clear the file
		JoystickFile.Clear();

		// Iterate over the 4 directions and save the joystick axis
		for(int iDirection=0; iDirection<4; iDirection++)
		{
			JoystickFile.AddLine(ToString((long)m_Directions[iDirection].GetJoystickAxe()));
			JoystickFile.AddLine(m_Directions[iDirection].GetJoystickInvertedFlag() ? "1":"0");
		}

		// Save the joystick buttons
		JoystickFile.AddLine(ToString(m_iEmergency));
		JoystickFile.AddLine(ToString(m_iTakeOff));
		JoystickFile.AddLine(ToString(m_iCamera));	
		JoystickFile.AddLine(ToString(m_iFullscreen));
		JoystickFile.AddLine(ToString(m_iRecord));
		JoystickFile.AddLine(ToString(m_iScreenshot));
		JoystickFile.AddLine(ToString(m_iReturnHome));
	
		// Save the changes
		if(!JoystickFile.Write())
		{
			return false;
		}
		// Close the file
		if(!JoystickFile.Close())
		{
			return false;
		}
	}

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// As long keybord input has not been saved we need to adapt keyboard layout
/////////////////////////////////////////////////////////////////////////////
void CInput::RefreshLayout()
{
	if(::wxFileExists("Data\\Config\\Keyboard.ini"))
	{
		return;
	}
	
	// Only for French language, use French keyboard layout (azerty, instead of qwerty, qwertz...)
	wxString strLang = CConfig::GetSingleton()->GetLanguage();
	
	strLang.MakeUpper();
	if(0 == strLang.Find("FRAN"))
	{
		m_Directions[IDX_TURN].SetKeys(wxKeyCode('Q'), wxKeyCode('D'));
		m_Directions[IDX_ALTITUDE].SetKeys(WXK_CONTROL_S, WXK_CONTROL_Z);
	}
	else
	{
		m_Directions[IDX_TURN].SetKeys(wxKeyCode('A'), wxKeyCode('D'));
		m_Directions[IDX_ALTITUDE].SetKeys(WXK_CONTROL_S, WXK_CONTROL_W);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Get the current vector af a specific axe
/////////////////////////////////////////////////////////////////////////////
const double& CInput::GetDirectionValue(eDirection Direction)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	return m_Directions[Direction].GetVectorValue();
}


/////////////////////////////////////////////////////////////////////////////
// Get FPS
/////////////////////////////////////////////////////////////////////////////
long CInput::GetFPS()
{
	wxCriticalSectionLocker Lock(m_CSInput);

	return m_lFps;
}


/////////////////////////////////////////////////////////////////////////////
// Set a key to "pressed"
/////////////////////////////////////////////////////////////////////////////
void CInput::SetKey(eKey Key)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	m_usKeyStatus |= Key;
}


/////////////////////////////////////////////////////////////////////////////
// Reset a key to "not pressed"
/////////////////////////////////////////////////////////////////////////////
void CInput::ResetKey(eKey Key)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	m_usKeyStatus &= ~Key;
}


/////////////////////////////////////////////////////////////////////////////
// Check key status
/////////////////////////////////////////////////////////////////////////////
bool CInput::IsPressed(eKey Key)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	return ((m_usKeyStatus & Key) == Key);
}


/////////////////////////////////////////////////////////////////////////////
// Set a flag
/////////////////////////////////////////////////////////////////////////////
void CInput::SetFlag(eKey Key)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	m_usFlagStatus |= Key;
}


/////////////////////////////////////////////////////////////////////////////
// Reset a flag
/////////////////////////////////////////////////////////////////////////////
void CInput::ResetFlag(eKey Key)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	m_usFlagStatus &= ~Key;
}


/////////////////////////////////////////////////////////////////////////////
// Check for flag
/////////////////////////////////////////////////////////////////////////////
bool CInput::HasFlag(eKey Key)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	return ((m_usFlagStatus & Key) == Key);
}


/////////////////////////////////////////////////////////////////////////////
// Set the joystick axe to use to control the direction
/////////////////////////////////////////////////////////////////////////////
bool CInput::SetJoystickControl(eDirection Direction, eJoystickAxis JoystickAxeToUse, bool bIsInverted)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	if( (NULL == m_pJoystick) || (!m_pJoystick->HasAxis(JoystickAxeToUse)) )
	{
		return false;
	}
	
	m_Directions[Direction].SetJoystick(JoystickAxeToUse, m_pJoystick, bIsInverted);

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Has the direction the inverted flag
/////////////////////////////////////////////////////////////////////////////
bool CInput::HasInvertedFlag(eDirection Direction)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	return m_Directions[Direction].GetJoystickInvertedFlag();
}


/////////////////////////////////////////////////////////////////////////////
// Assigned joystick axe for direction
/////////////////////////////////////////////////////////////////////////////
eJoystickAxis CInput::GetJoystickAxe(eDirection Direction)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	return m_Directions[Direction].GetJoystickAxe();
}


/////////////////////////////////////////////////////////////////////////////
// Set the keys to use for a direction
/////////////////////////////////////////////////////////////////////////////
void CInput::SetKeysControl(eDirection Direction, wxKeyCode KeyValueDown, wxKeyCode KeyValueUp)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	m_Directions[Direction].SetKeys(KeyValueDown, KeyValueUp);
}


/////////////////////////////////////////////////////////////////////////////
// Get current keys used for a direction
/////////////////////////////////////////////////////////////////////////////
void CInput::GetKeysControl(eDirection Direction, wxKeyCode& KeyValueDown, wxKeyCode& KeyValueUp)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	KeyValueDown = m_Directions[Direction].GetKeyDown();
	KeyValueUp = m_Directions[Direction].GetKeyUp();
}


/////////////////////////////////////////////////////////////////////////////
// Set a button id to an action
/////////////////////////////////////////////////////////////////////////////
void CInput::AssignButtonToAction(int iButton, eKey Action)
{
	switch(Action)
	{
		case KEY_EMERGENCY:
		{
			m_iEmergency = iButton;
		}
		break;

		case KEY_FULLSCREEN:
		{
			m_iFullscreen = iButton;
		}
		break;

		case KEY_CAMERA:
		{
			m_iCamera = iButton;
		}
		break;

		case KEY_TAKEOFF:
		{
			m_iTakeOff = iButton;
		}
		break;

		case KEY_RECORD:
		{
			m_iRecord = iButton;
		}
		break;

		case KEY_SCREENSHOT:
		{
			m_iScreenshot = iButton;
		}
		break;

		case KEY_RETURNHOME:
		{
			m_iReturnHome = iButton;
		}
		break;

		default:			
			break;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Get button value as string
/////////////////////////////////////////////////////////////////////////////
wxString CInput::GetButtonAsString(eKey Button)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	wxString str("None");
	
	switch(Button)
	{
		case KEY_EMERGENCY:
		{
			str = GetValueAsString(m_iEmergency);
		}
		break;

		case KEY_FULLSCREEN:
		{
			str = GetValueAsString(m_iFullscreen);
		}
		break;

		case KEY_CAMERA:
		{
			str = GetValueAsString(m_iCamera);
		}
		break;

		case KEY_TAKEOFF:
		{
			str = GetValueAsString(m_iTakeOff);
		}
		break;

		case KEY_RECORD:
		{
			str = GetValueAsString(m_iRecord);
		}
		break;

		case KEY_SCREENSHOT:
		{
			str = GetValueAsString(m_iScreenshot);
		}
		break;

		case KEY_RETURNHOME:
		{
			str = GetValueAsString(m_iReturnHome);
		}
		break;

		default:			
			break;
	}

	return str;
}


/////////////////////////////////////////////////////////////////////////////
// Does a KeyCode correspond to a special key ?
/////////////////////////////////////////////////////////////////////////////
bool CInput::IsSpecialKey(wxKeyCode Key)
{
	wxCriticalSectionLocker Lock(m_CSInput);

	if( (Key == m_kCamera)		||		
		(Key == m_kEmergency)	||
		(Key == m_kFullscreen)	||
		(Key == m_kRecord)		||
		(Key == m_kTakeOff)		||
		(Key == m_kScreenshot)  ||
		(Key == m_kReturnHome) )
	{
		return true;
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// Get a value as string
/////////////////////////////////////////////////////////////////////////////
wxString CInput::GetValueAsString(int iValue)
{
	wxString str("None");

	if(iValue >= 0)
	{
		// +1 -> Joystick begins at button 0, but we display buttons from 1
		str = wxString::Format("%d", iValue+1);
	}

	return str;
}
