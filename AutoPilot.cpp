//////////////////////////////////////////////////////////////////////////////
// Autopilot definition - Allows the drone to come back to landing point
//////////////////////////////////////////////////////////////////////////////

#include "AutoPilot.h"
#include "Utils.h"


//////////////////////////////////////////////////////////////////////////////
// Constructor
//////////////////////////////////////////////////////////////////////////////
CAutoPilot::CAutoPilot()
{
	ResetAutoPilot();
}


//////////////////////////////////////////////////////////////////////////////
// Destructor
//////////////////////////////////////////////////////////////////////////////
CAutoPilot::~CAutoPilot()
{
}


//////////////////////////////////////////////////////////////////////////////
// Reset all values
//////////////////////////////////////////////////////////////////////////////
void CAutoPilot::ResetAutoPilot(wxLongLong llUpdateTime, bool bHasGps, double dStartLatitude, double dStartLongitude)
{
	wxCriticalSectionLocker Lock(m_CSAutoPilot);

	m_llLastUpdate				= llUpdateTime;

	m_dLastVelocityForward		= 0.0f;
	m_dLastVelocityLateral		= 0.0f;
	m_dPositionX				= 0.0f;
	m_dPositionY				= 0.0f;
	m_dDistance					= 0.0f;

	m_dCurrentAltitude			= 0.0f;
	m_dMaxAltitude				= 0.0f;
	
	m_dCurrentAngleDrone		= 0.0f;
	m_dCurrentHomeAngle			= 0.0f;

	m_bHasGps					= bHasGps;

	m_dStartLatitude			= dStartLatitude;
	m_dStartLongitude			= dStartLongitude;

	m_dCurrentLatitude			= 0.0f;
	m_dCurrentLongitude			= 0.0f;

	if(m_bHasGps)
	{
		DoLog(wxString::Format("Autopilot reseted with gps, home coordinates: Lat %f - lon %f", m_dStartLatitude, m_dStartLongitude));
	}
}


//////////////////////////////////////////////////////////////////////////////
// Update specific informations for GPS
//////////////////////////////////////////////////////////////////////////////
void CAutoPilot::UpdateGps(double dLatitude, double dLongitude)
{
	wxCriticalSectionLocker Lock(m_CSAutoPilot);

	m_dCurrentLatitude = dLatitude;
	m_dCurrentLongitude = dLongitude;
}


//////////////////////////////////////////////////////////////////////////////
// Update drone informations
// Note: 
//    - Velocity X is forward flying direction
//    - Velocity Y is side flying direction
//////////////////////////////////////////////////////////////////////////////
void CAutoPilot::Update(wxLongLong llUpdateTime, double dDroneAngle360, double dVelocityForward, double dVelocitySide, double dAltitude, bool bAutopilotIsOn)
{
	wxCriticalSectionLocker Lock(m_CSAutoPilot);

	// Set new angle
	m_dCurrentAngleDrone = dDroneAngle360;

	// Set current altitude
	m_dCurrentAltitude =  dAltitude;

	// If we are higher that the last max altitude, save the new value
	// But only if autopilot is off, otherwise the drone will never stop to go up in autopilot mode !
	if( (!bAutopilotIsOn) && (dAltitude > m_dMaxAltitude) )
	{
		m_dMaxAltitude = dAltitude;
	}

	// In case of GPS we only need to calculate the current distance, witout GPS we also need to estimate the new position
	if(m_bHasGps)
	{
		// Convert to radian
		double dA = m_dStartLatitude*dPIover180;
		double dB = m_dCurrentLatitude*dPIover180;
		double dC = m_dStartLongitude*dPIover180;
		double dD = m_dCurrentLongitude*dPIover180;

		// Calculate distance to drone in meters
		m_dDistance = dEarthRadius*acos(cos(dA)*cos(dB)*cos(dC-dD)+sin(dA)*sin(dB));
	}
	else
	{
		// Get the time difference since last update in microseconds
		wxLongLong llDiffTime = llUpdateTime - m_llLastUpdate;

		// Transform microseconds to seconds
		double dTimeDiff = llDiffTime.ToDouble() * 0.000001f;

		// Keep update time
		m_llLastUpdate = llUpdateTime;

		// Calculate angle over pi/180 to get value in radian
		double dAngleOverPi = dDroneAngle360 * dPIover180;

		// Get vectors from current angle
		double dVectorY = cos(dAngleOverPi);
		double dVectorX = sin(dAngleOverPi);

		// Average forward and lateral speed
		double dForwardSpeed = (dVelocityForward+m_dLastVelocityForward)/2.0f;
		double dLateralSpeed = (dVelocitySide+m_dLastVelocityLateral)/2.0f;

		// Save current velocity
		m_dLastVelocityForward = dVelocityForward;
		m_dLastVelocityLateral = dVelocitySide;

		// Direction * Speed * Time
		m_dPositionY += dVectorY*dForwardSpeed*dTimeDiff;
		m_dPositionX += dVectorX*dForwardSpeed*dTimeDiff;

		// Add 90° to add also side moves
		dAngleOverPi = (dDroneAngle360 + 90.0f) * dPIover180;

		// Do same calculation for dVelocityY !
		dVectorY = cos(dAngleOverPi);
		dVectorX = sin(dAngleOverPi);
	
		m_dPositionY += dVectorY*dLateralSpeed*dTimeDiff;
		m_dPositionX += dVectorX*dLateralSpeed*dTimeDiff;

		// Estimate the distance to the drone in meters
		m_dDistance = sqrt((m_dPositionX*m_dPositionX) + (m_dPositionY*m_dPositionY));
	}
}


//////////////////////////////////////////////////////////////////////////////
// Find out the way to go home
//////////////////////////////////////////////////////////////////////////////
void CAutoPilot::ComputeWayToHome(double& dRotationSpeed, double& dMovementSpeed, double& dAltitudeSpeed)
{
	wxCriticalSectionLocker Lock(m_CSAutoPilot);

	// Drone is less than 3 meters away
	if(m_dDistance < 3.0f)
	{
		// If the drone is high, bring him down
		if(m_dCurrentAltitude > 3.0f)
		{
			dAltitudeSpeed = -0.25f;
		}
		else
		{
			dAltitudeSpeed = 0.0f;
		}
	
		// The drone is very close, nothing more to do
		if( m_dDistance < 1.0f )
		{
			dRotationSpeed = 0.0f;
			dMovementSpeed = 0.0f;
			return;
		}
	}
	else
	{
		// The drone is still far away, so if the drone is lower than max altitude, let him go up
		if(m_dCurrentAltitude < m_dMaxAltitude)
		{
			dAltitudeSpeed = 0.25f;
		}
		else if(m_dCurrentAltitude > (m_dMaxAltitude+1.5f))
		{
			// If the drone is too high (1.5 meters over max) bring him down
			dAltitudeSpeed = -0.25f;
		}
	}

	if(m_bHasGps)
	{
		// First, get the vector to home
		double dVectY = m_dStartLatitude - m_dCurrentLatitude;
		double dVectX = m_dStartLongitude - m_dCurrentLongitude;

		// Get the absolute home angle, needed to go home
		m_dCurrentHomeAngle = GetAngle360(atan2(dVectX, dVectY)*d180overPI);
	}
	else
	{
		// Position X.Y is the vector to the drone, to get the vector to home, we need to invert the direction
		// Warning : X position is first !!
		m_dCurrentHomeAngle = GetAngle360(atan2(-m_dPositionX, -m_dPositionY)*d180overPI);		
	}

	// Find out if the drone is looking in the right direction
	dRotationSpeed = GetAngle180(m_dCurrentHomeAngle-m_dCurrentAngleDrone);

	// Get the importance of rotation
	double dRotValue = abs(dRotationSpeed);
	if( dRotValue > 25.0f )
	{
		// The angle is higher than 25°, stop the drone mouvement and turn it until he looks at the right direction
		dMovementSpeed = 0.0f;
		if(dRotationSpeed > 0.5f)
		{
			dRotationSpeed = 0.5f;
		}
		else if(dRotationSpeed < -0.5f)
		{
			dRotationSpeed = -0.5f;
		}
	}
	else
	{
		// Get rotation speed to use, exple: 20°/100 = 0.2, 5°/100 = 0.05
		dRotationSpeed = dRotationSpeed/100.0f;
		// Get movement speed to use, exple: (30-20°)/100 = 0.1, (30-5°)/100 = 0.25;
		dMovementSpeed = (30.0f - dRotValue)/100.0f;
	}

	// If the drone is close to landing point, avoid high speed
	if( dMovementSpeed > 0.09f )
	{
		if(m_dDistance < 2.0f)
		{
			dMovementSpeed = 0.05f;
		}
		else if(m_dDistance < 4.0f)
		{
			dMovementSpeed = 0.075f;
		}
		else if(m_dDistance < 6.0f)
		{
			dMovementSpeed = 0.1f;
		}
	}
}


//////////////////////////////////////////////////////////////////////////////
// Gps status
//////////////////////////////////////////////////////////////////////////////
bool CAutoPilot::HasGps()
{
	wxCriticalSectionLocker Lock(m_CSAutoPilot);
	return m_bHasGps;
}


//////////////////////////////////////////////////////////////////////////////
// Distance to drone in meters
//////////////////////////////////////////////////////////////////////////////
double CAutoPilot::GetDistance()
{
	wxCriticalSectionLocker Lock(m_CSAutoPilot);
	return m_dDistance;
}


//////////////////////////////////////////////////////////////////////////////
// Get one of the property (for debug informations)
//////////////////////////////////////////////////////////////////////////////
double CAutoPilot::GetProperty(eDBGPROPERTY Property)
{
	wxCriticalSectionLocker Lock(m_CSAutoPilot);

	switch(Property)
	{
		case DBG_POSITIONX:					return m_dPositionX;						break;
		case DBG_POSITIONY:					return m_dPositionY;						break;
		case DBG_ALTITUDE:					return m_dCurrentAltitude;					break;
		case DBG_ALTITUDEMAX:				return m_dMaxAltitude;						break;
		case DBG_CURRENTDRONEANGLE:			return m_dCurrentAngleDrone;				break;
		case DBG_HOMEANGLE:					return m_dCurrentHomeAngle;					break;
		case DBG_CURRENTLATITUDE:			return m_dCurrentLatitude;					break;
		case DBG_CURRENTLONGITUDE:			return m_dCurrentLongitude;					break;
		default:							return 0.0f;								break;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Get an angle between 0 and 360
//////////////////////////////////////////////////////////////////////////////
double CAutoPilot::GetAngle360(double dAngle)
{
	if(dAngle > 360.f)
	{
		dAngle-=360.0f;
	}
	else if(dAngle < 0.0f)
	{
		dAngle+=360.0f;
	}
	return dAngle;
}


//////////////////////////////////////////////////////////////////////////////
// Get an angle between -180 end 180
//////////////////////////////////////////////////////////////////////////////
double CAutoPilot::GetAngle180(double dAngle)
{
	if(dAngle > 180.0f)
	{
		dAngle -= 360.0f;
	}
	else if(dAngle < -180.0f)
	{
		dAngle += 360.0f;
	}
	return dAngle;
}


//////////////////////////////////////////////////////////////////////////////
// Get difference between two angles (absolute difference in degrees)
//////////////////////////////////////////////////////////////////////////////
double CAutoPilot::GetAngleDif(double dAngle1, double dAngle2)
{
	return (180.0f - abs(abs(dAngle1 - dAngle2) - 180.0f));
}
