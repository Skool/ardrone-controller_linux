/////////////////////////////////////////////////////////////////////////////
// Manage one input direction
/////////////////////////////////////////////////////////////////////////////

#include "InputDirection.h"


/////////////////////////////////////////////////////////////////////////////
// Construction
/////////////////////////////////////////////////////////////////////////////
CInputDirection::CInputDirection()
{
	m_dVectorValue		= 0.0f;

	m_KeyDown			= WXK_NONE;
	m_KeyUp				= WXK_NONE;
	
	m_pJoystick			= NULL;
	m_Axe				= AXIS_NONE;
	m_bInverted			= false;
	m_dJoystickMin		= 0.0f;
	m_dJoystickMax		= 0.0f;
	m_dJoystickMiddle	= 0.0f;
	m_dDeadZoneMin		= 0.0f;
	m_dDeadZoneMax		= 0.0f;
}


/////////////////////////////////////////////////////////////////////////////
// Destruction
/////////////////////////////////////////////////////////////////////////////
CInputDirection::~CInputDirection()
{
}


/////////////////////////////////////////////////////////////////////////////
// Init the control for keys
/////////////////////////////////////////////////////////////////////////////
void CInputDirection::SetKeys(const wxKeyCode& KeyDown, const wxKeyCode& KeyUp)
{
	m_KeyDown = KeyDown;
	m_KeyUp = KeyUp;
}


/////////////////////////////////////////////////////////////////////////////
// Init the controls for joystick
/////////////////////////////////////////////////////////////////////////////
void CInputDirection::SetJoystick(const eJoystickAxis& AxeToUse, CJoystick* pJoystick, bool bIsInverted)
{
	// Joystick not set, break
	if(NULL == pJoystick)
	{
		return;
	}

	if(AXIS_NONE == AxeToUse)
	{
		// Axis need to be reseted
		m_Axe = AXIS_NONE;
	}
	else if(pJoystick->HasAxis(AxeToUse))
	{
		m_pJoystick	= pJoystick;
		m_Axe		= AxeToUse;
		m_bInverted	= bIsInverted;
		
		// Get min and max value of the joystick
		switch(m_Axe)
		{
			case AXIS_X:
			{
				m_dJoystickMin = (double)m_pJoystick->GetXMin();
				m_dJoystickMax = (double)m_pJoystick->GetXMax();
			}
			break;

			case AXIS_Y:
			{
				m_dJoystickMin = (double)m_pJoystick->GetYMin();
				m_dJoystickMax = (double)m_pJoystick->GetYMax();
			}
			break;

			case AXIS_RUDDER:
			{
				m_dJoystickMin = (double)m_pJoystick->GetRudderMin();
				m_dJoystickMax = (double)m_pJoystick->GetRudderMax();
			}
			break;

			case AXIS_U:
			{
				m_dJoystickMin = (double)m_pJoystick->GetUMin();
				m_dJoystickMax = (double)m_pJoystick->GetUMax();
			}
			break;

			case AXIS_V:
			{
				m_dJoystickMin = (double)m_pJoystick->GetVMin();
				m_dJoystickMax = (double)m_pJoystick->GetVMax();
			}
			break;

			case AXIS_Z:
			{
				m_dJoystickMin = (double)m_pJoystick->GetZMin();
				m_dJoystickMax = (double)m_pJoystick->GetZMax();
			}
			break;

			default:
				break;
		}

		// Get middle position of the joystick
		m_dJoystickMiddle = (m_dJoystickMax - m_dJoystickMin)/2.0f;
		// Get a dead zone range (5% of middle position)
		double dDeadZone = m_dJoystickMiddle * 0.05f;
		// Set min and max value for the dead zone
		m_dDeadZoneMin = m_dJoystickMiddle - dDeadZone;
		m_dDeadZoneMax = m_dJoystickMiddle + dDeadZone;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Get the value up key
/////////////////////////////////////////////////////////////////////////////
wxKeyCode CInputDirection::GetKeyUp()
{
	return m_KeyUp;
}


/////////////////////////////////////////////////////////////////////////////
// Get the value down key
/////////////////////////////////////////////////////////////////////////////
wxKeyCode CInputDirection::GetKeyDown()
{
	return m_KeyDown;
}


/////////////////////////////////////////////////////////////////////////////
// Get joystick axe
/////////////////////////////////////////////////////////////////////////////
eJoystickAxis CInputDirection::GetJoystickAxe()
{
	return m_Axe;
}


/////////////////////////////////////////////////////////////////////////////
// Get the joystick inverted flag
/////////////////////////////////////////////////////////////////////////////
bool CInputDirection::GetJoystickInvertedFlag()
{
	return m_bInverted;
}


/////////////////////////////////////////////////////////////////////////////
// Update values
/////////////////////////////////////////////////////////////////////////////
void CInputDirection::UpdateDirection(const double& dAcceleration, const double& dDecceleration, const double& dMaxSpeed)
{
	bool bUseKeyboard = true;
	bool bLetValueGoDown = true;

	// If a joystick is set, check if it is currently in use
	if(m_Axe != AXIS_NONE)
	{
		double dValue = (double)m_pJoystick->GetRawValue(m_Axe);
		
		// Joystick is in use if the value is out of the dead zone
		if( (dValue > m_dDeadZoneMax) || (dValue < m_dDeadZoneMin) )
		{
			bUseKeyboard = false;
			bLetValueGoDown = false;

			dValue = (dValue-m_dJoystickMiddle)/m_dJoystickMiddle;

			// Inverted axe
			if(m_bInverted)
			{
				dValue = -1.0f * dValue;
			}

			// Reduce to max speed
			m_dVectorValue = dValue * dMaxSpeed;
		}
	}

	// Joystick is not present, not used for this axe or has not move, so check now for keys
	if(bUseKeyboard)
	{
		// Vector up key pressed
		if(wxGetKeyState(m_KeyUp))
		{
			bLetValueGoDown = false;
			if(m_dVectorValue < dMaxSpeed)
			{				
				m_dVectorValue += dAcceleration;
			}
		}
		// Vector down key pressed
		else if(wxGetKeyState(m_KeyDown))
		{
			bLetValueGoDown = false;
			if(m_dVectorValue > -dMaxSpeed)
			{
				m_dVectorValue -= dAcceleration;
			}
		}
	}

	// For this axe, the joystick and the keyboard are not used, so let the value go down
	if(bLetValueGoDown)
	{
		if(m_dVectorValue != 0.0f)
		{
			if(m_dVectorValue <= -0.01f)
			{
				m_dVectorValue += dDecceleration;
			}
			else if(m_dVectorValue >= 0.01f)
			{
				m_dVectorValue -= dDecceleration;
			}
			else
			{
				// Between -0,01 and 0,01 set the vector to 0, otherwise it may never be 0 because of rounding
				m_dVectorValue = 0.0f;
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// Get current vector value
/////////////////////////////////////////////////////////////////////////////
const double& CInputDirection::GetVectorValue()
{
	return m_dVectorValue;
}