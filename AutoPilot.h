//////////////////////////////////////////////////////////////////////////////
// Autopilot declaration - Allows the drone to come back to landing point
//////////////////////////////////////////////////////////////////////////////

#ifndef __HEADER_AUTOPILOT__
#define __HEADER_AUTOPILOT__


#include "wx/wx.h"
#include <wx/thread.h>

// Properties that can be displayed for debug informations
enum eDBGPROPERTY
{
	DBG_POSITIONX,
	DBG_POSITIONY,
	DBG_ALTITUDE,
	DBG_ALTITUDEMAX,
	DBG_CURRENTDRONEANGLE,
	DBG_HOMEANGLE,
	DBG_CURRENTLATITUDE,
	DBG_CURRENTLONGITUDE,
};

// Radius of the earth in meters
const double dEarthRadius =  6372795.0f;

class CAutoPilot
{
public:
	// Constructor
	CAutoPilot();
	// Destructor
	~CAutoPilot();

	// Reset all values
	void ResetAutoPilot(wxLongLong llUpdateTime = 0, bool bHasGps = false, double dStartLatitude = 0.0f, double dStartLongitude = 0.0f);

	// Update specific informations for GPS
	void UpdateGps(double dLatitude, double dLongitude);

	// Update drone informations
	void Update(const wxLongLong llUpdateTime, double dDroneAngle360, double dVelocityForward, double dVelocitySide, double dAltitude, bool bAutopilotIsOn=false);	

	// Find out the way to go home
	void ComputeWayToHome(double& dRotationSpeed, double& dMovementSpeed, double& dAltitudeSpeed);	

	// Gps status
	bool HasGps();

	// Distance to drone in meters
	double GetDistance();

	// Get one of the property (for debug informations)
	double GetProperty(eDBGPROPERTY Property);
	
private:
// Note : Following functions could be made static, if needed
	// Get an angle between 0 and 360
	double GetAngle360(double dAngle);

	// Get an angle between -180 end 180
	double GetAngle180(double dAngle);

	// Get difference between two angles (absolute difference in degrees)
	double GetAngleDif(double dAngle1, double dAngle2);
	
private:
// Common values
	// Critical section to protect data
	wxCriticalSection	m_CSAutoPilot;
	// Distance from home position to drone in meters
	double				m_dDistance;
	// Current altitude of the drone
	double				m_dCurrentAltitude;
	// Max altitude reached by the drone while user was flying
	double				m_dMaxAltitude;
	// Current direction (angle) given by the drone
	double				m_dCurrentAngleDrone;
	// Current angle to home (calculated using gps or sensors)
	double				m_dCurrentHomeAngle;	
	// The flag to identify if a GPS is present or not
	bool				m_bHasGps;

// For RTH based on sensors	
	// Last forward velocity
	double				m_dLastVelocityForward;
	// Last lateral velocity
	double				m_dLastVelocityLateral;
	// Position X in mm from starting point
	double				m_dPositionX;
	// Position Y in mm from starting point
	double				m_dPositionY;
	// Last update time in microseconds
	wxLongLong			m_llLastUpdate;

// For RTH based on gps
	// The latitude of the starting point (y)
	double				m_dStartLatitude;
	// The longitude of the starting point (X)
	double				m_dStartLongitude;
	// The last latitude (Y)
	double				m_dCurrentLatitude;
	// The last longitude (X)
	double				m_dCurrentLongitude;
};

#endif