/////////////////////////////////////////////////////////////////////////////
// Manage one input Direction
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wx/wx.h"
#include "Joystick.h"


class CInputDirection
{
public:
	// Construction/Destruction
	CInputDirection();
	~CInputDirection();

	// Init the control for keys
	void SetKeys(const wxKeyCode& KeyDown, const wxKeyCode& KeyUp);
	// Init the controls for joystick
	void SetJoystick(const eJoystickAxis& AxeToUse, CJoystick* pJoystick, bool bIsInverted);

	// Get keys
	wxKeyCode GetKeyUp();
	wxKeyCode GetKeyDown();

	// Get joystick axe
	eJoystickAxis GetJoystickAxe();
	// Get the joystick inverted flag
	bool GetJoystickInvertedFlag();

	// Update values
	void UpdateDirection(const double& dAcceleration, const double& dDecceleration, const double& dMaxSpeed);

	// Get current vector value
	const double& GetVectorValue();

private:
	// Current vector value
	double			m_dVectorValue;

	// The key to set the value down
	wxKeyCode		m_KeyDown;
	// The key to set the value up
	wxKeyCode		m_KeyUp;

	// Pointer to the joystick instance
	CJoystick*		m_pJoystick;
	// The assigned joystick axis
	eJoystickAxis	m_Axe;
	// If the Joystick axis need to be inverted
	bool			m_bInverted;
	// Minimum and maximum value of the joystick axe
	double			m_dJoystickMin;
	double			m_dJoystickMax;
	// Middle value of the joystick axe
	double			m_dJoystickMiddle;
	// Value of the "dead zone"
	double			m_dDeadZoneMin;
	double			m_dDeadZoneMax;
};