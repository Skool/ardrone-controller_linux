//////////////////////////////////////////////////////////////////////////////
// Custom drone definition
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "AppConfig.h"	// Codecs also defined here
#include "ardrone/ardrone.h"

// Our drone inherits from the default drone class
class CCustomDrone : public ARDrone
{
public:

	CCustomDrone();
	~CCustomDrone();
	
	// Check if the drone is in error state
	bool HasError();

	// Get the error text of the drone
	wxString GetErrorText();

	// Check if we have an usb key that we can use for record
	bool HasUsbKey();

	// Send trim command to the drone
	void Trim(void);
	// Calibrate magnetos (0=magneto, 1=device)
	void Calibrate(int iDevice = 0);

	// Roll angle  [Degre]
	double getRollDeg(void);
	// Pitch angle [Degre]
	double getPitchDeg(void);
	// Yaw angle   [degre]
	double getYawDeg(void);

	// Get horizontal speed in m/s
	double GetHorizontalVelocity();
	// Get horizontal velocity in m/s
	void GetHorizontalVelocity(double& dX, double& dY);

	// Set video codec to use
	void SetVideoCodec(eVideoCodec VideoCodec);

	// Set maximum altitude
	void SetMaxAltitude(long lMaxAltitude);

	// Our own movement (without gain, etc...)
	void CustomMove(float fX, float fY, float fZ, float fR);

	// Get intensity of wifi signal
	unsigned int GetWifiSignal();

	// Is a gps plugged or not
	bool HasGps();

	// Get gps coordinates
	void GetGpsPosition(double& dLat, double& dLong);

	// Get current gps angle. Note: The angle is only correct when the drone is moving !!
	double GetGpsAngle();
};
