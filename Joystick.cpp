/////////////////////////////////////////////////////////////////////////////
// Manage one input axe
/////////////////////////////////////////////////////////////////////////////

#include "Joystick.h"
#include "Utils.h"


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CJoystick::CJoystick()
{
	// Init axis to none
	m_usAvailableJoystickAxis = AXIS_NONE;

	// Init number of buttons to 0
	m_iNumberOfButtons = 0;

	// Initialize joystick
	if( (wxJoystick::GetNumberJoysticks() > 0) && (this->IsOk()) )
	{		
		// Add X and Y as default
		m_usAvailableJoystickAxis = AXIS_X | AXIS_Y;

		// Check for other axis
		if(HasRudder())
		{
			m_usAvailableJoystickAxis|=AXIS_RUDDER;
			DoLog("Joystick has rudder");
		}
		
		if(HasU())
		{
			m_usAvailableJoystickAxis|=AXIS_U;
			DoLog("Joystick has axe U");
		}

		if(HasV())
		{
			m_usAvailableJoystickAxis|=AXIS_V;
			DoLog("Joystick has axe V");
		}

		if(HasZ())
		{
			m_usAvailableJoystickAxis|=AXIS_Z;
			DoLog("Joystick has axe Z");
		}

		// Get number of buttons
		m_iNumberOfButtons = this->GetNumberButtons();
		DoLog(wxString::Format("Joystick has %d available buttons", m_iNumberOfButtons));

		// Create the joystick configuration file name to use
		m_strConfigFileName = wxString::Format("Joystick-%d-%d.ini", this->GetManufacturerId(), this->GetProductId());
	}
	else
	{
		DoLog("Invalid joystick state !", MSG_ERROR);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
CJoystick::~CJoystick()
{
}


/////////////////////////////////////////////////////////////////////////////
// Get current value of a specific axe
/////////////////////////////////////////////////////////////////////////////
int CJoystick::GetRawValue(eJoystickAxis Axe)
{
	int		iValue = 0;
	wxPoint Point(0,0);

	if((m_usAvailableJoystickAxis & Axe) == Axe)
	{
		switch(Axe)
		{
			case AXIS_X:
			{
				Point = this->GetPosition();
				iValue = Point.x;
			}
			break;

			case AXIS_Y:
			{
				Point = this->GetPosition();
				iValue = Point.y;
			}
			break;

			case AXIS_RUDDER:
			{
				iValue = this->GetRudderPosition();
			}
			break;

			case AXIS_V:
			{
				iValue = this->GetVPosition();
			}
			break;

			case AXIS_U:
			{
				iValue = this->GetUPosition();
			}
			break;

			case AXIS_Z:
			{
				iValue = this->GetZPosition();
			}
			break;

			default:
				break;
		}
	}

	return iValue;	
}


/////////////////////////////////////////////////////////////////////////////
// Check presence of axis
/////////////////////////////////////////////////////////////////////////////
bool CJoystick::HasAxis(eJoystickAxis Axe)
{
	return ((m_usAvailableJoystickAxis & Axe) == Axe);
}


/////////////////////////////////////////////////////////////////////////////
// Get the name of an axis from his index
// Note: Static function
/////////////////////////////////////////////////////////////////////////////
wxString CJoystick::GetAxisName(eJoystickAxis AxisIndex)
{
	wxString strName;

	switch(AxisIndex)
	{
		case AXIS_X:
		{
			strName = "X";
		}
		break;

		case AXIS_Y:
		{
			strName = "Y";
		}
		break;

		case AXIS_RUDDER:
		{
			strName = "RUDDER";
		}
		break;

		case AXIS_U:
		{
			strName = "U";
		}
		break;

		case AXIS_V:
		{
			strName = "V";
		}
		break;

		case AXIS_Z:
		{
			strName = "Z";
		}
		break;

		default:
		{
			strName = "NONE";
		}
		break;
	}

	return strName;
}


/////////////////////////////////////////////////////////////////////////////
// Get the index of an axis from his name
// Note: Static function
/////////////////////////////////////////////////////////////////////////////
eJoystickAxis CJoystick::GetAxisIndex(const wxString& strAxisName)
{
	if("X" == strAxisName)
	{
		return AXIS_X;
	}
	else if("Y" == strAxisName)
	{
		return AXIS_Y;
	}
	else if("RUDDER" == strAxisName)
	{
		return AXIS_RUDDER;
	}
	else if("U" == strAxisName)
	{
		return AXIS_U;
	}
	else if("V" == strAxisName)
	{
		return AXIS_V;
	}
	else if("Z" == strAxisName)
	{
		return AXIS_Z;
	}
	else
	{
		return AXIS_NONE;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Get the joystick configuration file name
/////////////////////////////////////////////////////////////////////////////
wxString& CJoystick::GetConfigFileName()
{
	return m_strConfigFileName;
}