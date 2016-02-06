/////////////////////////////////////////////////////////////////////////////
// Manage Joystick
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wx/wx.h"
#include <wx/joystick.h>


// Enumeration of joystick axis
enum eJoystickAxis:unsigned __int16
{
	AXIS_NONE			= 0,
	AXIS_X				= 1,
	AXIS_Y				= 2,
	AXIS_RUDDER			= 4,
	AXIS_U				= 8,
	AXIS_V				= 16,
	AXIS_Z				= 32
};


class CJoystick : public wxJoystick
{
public:
	// Construction/Destruction
	CJoystick();
	~CJoystick();

	// Get current value of a specific axe
	int GetRawValue(eJoystickAxis Axe);
	// Check presence of an axis
	bool HasAxis(eJoystickAxis Axe);

	// Get the name of an axis from his index
	static wxString GetAxisName(eJoystickAxis AxisIndex);
	// Get the index of an axis from his name
	static eJoystickAxis GetAxisIndex(const wxString& strAxisName);

	// Get the joystick configuration file name
	wxString& GetConfigFileName();

private:
	// Cache : available axis
	unsigned short		m_usAvailableJoystickAxis;
	// Number of buttons
	int					m_iNumberOfButtons;

	// The name of the Joystick configuration file depends on Manufacturer and model
	wxString			m_strConfigFileName;
};