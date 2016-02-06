//////////////////////////////////////////////////////////////////////////////
// Custom drone implementation
//////////////////////////////////////////////////////////////////////////////

#include "Utils.h"
#include "CustomDrone.h"


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CCustomDrone::CCustomDrone():ARDrone()
{
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
CCustomDrone::~CCustomDrone()
{
}


/////////////////////////////////////////////////////////////////////////////
// Check if the drone in in error state
/////////////////////////////////////////////////////////////////////////////
bool CCustomDrone::HasError()
{
	// Get state
	LockNavdata();
	unsigned int uiState =navdata.ardrone_state;
	UnlockNavdata();

	// Check for errors
	if( (uiState & ARDRONE_MOTORS_MASK)			||
		(uiState & ARDRONE_CUTOUT_MASK)			||
		(uiState & ARDRONE_ANGLES_OUT_OF_RANGE) ||
		(uiState & ARDRONE_ULTRASOUND_MASK)		||
		(uiState & ARDRONE_COM_LOST_MASK)		||
		(uiState & ARDRONE_COM_WATCHDOG_MASK)	||
		(uiState & ARDRONE_EMERGENCY_MASK)		||
		(uiState & ARDRONE_USER_EL)	)
		//(uiState & ARDRONE_VBAT_LOW) )
	{
		return true;
	}
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// Get the error text of the drone
/////////////////////////////////////////////////////////////////////////////
wxString CCustomDrone::GetErrorText()
{
	// Get state
	LockNavdata();
	unsigned int uiState = navdata.ardrone_state;
	UnlockNavdata();

	wxString strError = "";

	// Get corresponding text
	if(uiState & ARDRONE_MOTORS_MASK)
	{
		strError = GetText("ErrorMotors");
	}	
	else if(uiState & ARDRONE_CUTOUT_MASK)
	{
		strError = GetText("ErrorMotorBlocked");
	}
	else if(uiState & ARDRONE_ANGLES_OUT_OF_RANGE)
	{
		strError = GetText("ErrorAngles");
	}
	else if(uiState & ARDRONE_ULTRASOUND_MASK)
	{
		strError = GetText("ErrorUltrasound");
	}
	else if(uiState & ARDRONE_COM_LOST_MASK)
	{
		strError = GetText("ErrorLostComm");
	}	
	else if(uiState & ARDRONE_COM_WATCHDOG_MASK)
	{
		strError = GetText("ErrorWatchdog");
	}
	else if(uiState & ARDRONE_EMERGENCY_MASK)
	{
		strError = GetText("ErrorEM");
	}
	else if(uiState & ARDRONE_USER_EL)
	{
		strError = GetText("ErrorUserEM");
	}
	else if(uiState & ARDRONE_VBAT_LOW)
	{
		strError = GetText("ErrorBattery");
	}

	return strError;
}


/////////////////////////////////////////////////////////////////////////////
// Check if we have an usb key that we can use for record
/////////////////////////////////////////////////////////////////////////////
bool CCustomDrone::HasUsbKey()
{
	bool bHasUsb = false;

	// ArDrone 1 don't have usb
	if( version.major == ARDRONE_VERSION_2 )
	{
		LockNavdata();		
		bHasUsb = ( (navdata.ardrone_state & ARDRONE_USB_MASK) && (navdata.hdvideo_stream.usbkey_freespace > 10000) && (navdata.hdvideo_stream.usbkey_remaining_time > 120 ) );
		UnlockNavdata();
	}

	return bHasUsb;
}


/////////////////////////////////////////////////////////////////////////////
// Send trim command to the drone
/////////////////////////////////////////////////////////////////////////////
void CCustomDrone::Trim(void)
{
	if (onGround())
	{
		LockCommand();
		sockCommand.sendf("AT*FTRIM=%d,\r", seq++);
		UnlockCommand();
		Sleep(100);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Calibrate magnetos (0=magneto, 1=device)
/////////////////////////////////////////////////////////////////////////////
void CCustomDrone::Calibrate(int iDevice)
{	
    if (!onGround())
	{
        LockCommand();
        sockCommand.sendf("AT*CALIB=%d,%d\r", seq++, iDevice);
        UnlockCommand();
    }
}


//////////////////////////////////////////////////////////////////////////////
// Obtaining role angle in degre.
// Return value Role angle [Degre]
//////////////////////////////////////////////////////////////////////////////
double CCustomDrone::getRollDeg(void)
{
	LockNavdata();
    double dRoll = navdata.demo.phi * 0.001;
	UnlockNavdata();

	return dRoll;
}


//////////////////////////////////////////////////////////////////////////////
// Obtaining pitch angle in degre.
// Return value Pitch angle [Degre]
//////////////////////////////////////////////////////////////////////////////
double CCustomDrone::getPitchDeg(void)
{
	LockNavdata();
    double dPitch = navdata.demo.theta * 0.001;
	UnlockNavdata();

	return dPitch;
}


//////////////////////////////////////////////////////////////////////////////
// Obtaining yaw angle in degre.
// Return value Yaw angle [Degre]
//////////////////////////////////////////////////////////////////////////////
double CCustomDrone::getYawDeg(void)
{
	LockNavdata();
    double dYaw = navdata.demo.psi * 0.001;
	UnlockNavdata();

	// We dont wont negative values but values from 0° to 360°
	if(dYaw < 0.0f)
	{
		dYaw += 360.0f;
	}

	return dYaw;
}


//////////////////////////////////////////////////////////////////////////////
// Get horizontal speed in m/s
// Speed of x and y, z (altitude) is ignored
//////////////////////////////////////////////////////////////////////////////
double CCustomDrone::GetHorizontalVelocity()
{
	LockNavdata();
	double dVelocity = (sqrt( (navdata.demo.vx*navdata.demo.vx) + (navdata.demo.vy*navdata.demo.vy) ) * 0.001f);
	UnlockNavdata();

	return dVelocity;
}


//////////////////////////////////////////////////////////////////////////////
// Get horizontal velocity in m/s
//////////////////////////////////////////////////////////////////////////////
void CCustomDrone::GetHorizontalVelocity(double& dX, double& dY)
{
	LockNavdata();
	dX = (double)navdata.demo.vx * 0.001f;
	dY = (double)navdata.demo.vy * 0.001f;
	UnlockNavdata();
}


//////////////////////////////////////////////////////////////////////////////
// Set video codec to use
//////////////////////////////////////////////////////////////////////////////
void CCustomDrone::SetVideoCodec(eVideoCodec VideoCodec)
{
	if (version.major == ARDRONE_VERSION_2) 
	{
        // Finalize video
        finalizeVideo();

		LockCommand();

        // Output video with selected codec
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"video:video_codec\",\"%d\"\r", seq++, VideoCodec);
        
		UnlockCommand();
		
		Sleep(100);

        // Initialize video
		if(0 == initVideo())
		{
			DoLog("Failed to restart video after codec change, watchdog should catch this", MSG_ERROR);
			bNeedVideoRestart = true;
		}
    }
}


//////////////////////////////////////////////////////////////////////////////
// Set maximum altitude
//////////////////////////////////////////////////////////////////////////////
void CCustomDrone::SetMaxAltitude(long lMaxAltitude)
{
	// Altitude in meters has to be convert in millimeters
	if(lMaxAltitude > 100)
	{
		// 100m means unlimited
		lMaxAltitude = 100000;
	}
	else
	{
		lMaxAltitude *= 1000;
	}

	DoLog(wxString::Format("Set max altitude to %d m (%d mm)", lMaxAltitude/1000, lMaxAltitude));

	LockCommand();
	sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
	sockCommand.sendf("AT*CONFIG=%d,\"control:altitude_max\",\"%d\"\r",  seq++, lMaxAltitude);
	UnlockCommand();

	Sleep(100);
}


/////////////////////////////////////////////////////////////////////////////
// Our own movement (without gain, etc...)
/////////////////////////////////////////////////////////////////////////////
void CCustomDrone::CustomMove(float fX, float fY, float fZ, float fR)
{
	// Flight mode (0 = hovering)
	int iMode = ( (fabs(fX) > 0.0) || (fabs(fY) > 0.0) );

	// Invert only Y axis
	fY = -fY;

	LockCommand();
	sockCommand.sendf("AT*PCMD=%d,%d,%d,%d,%d,%d\r", seq++, iMode, *(int*)(&fX), *(int*)(&fY), *(int*)(&fZ), *(int*)(&fR));
	UnlockCommand();
}


/////////////////////////////////////////////////////////////////////////////
// Get intensity of wifi signal
/////////////////////////////////////////////////////////////////////////////
// Note: Drone always return 0 as signal, so don't use this yet !
unsigned int CCustomDrone::GetWifiSignal()
{
	unsigned int uiWifi = 0;

	LockNavdata();
	uiWifi = navdata.wifi.link_quality;
	UnlockNavdata();

	return uiWifi;
}


/////////////////////////////////////////////////////////////////////////////
// Is a gps plugged or not
/////////////////////////////////////////////////////////////////////////////
bool CCustomDrone::HasGps()
{
	bool bHasGps = false;
	
	LockNavdata();

	bHasGps = (navdata.gps.gps_plugged != 0);

	UnlockNavdata();

	return bHasGps;
}


/////////////////////////////////////////////////////////////////////////////
// Get gps coordinates
/////////////////////////////////////////////////////////////////////////////
void CCustomDrone::GetGpsPosition(double& dLat, double& dLong)
{
	LockNavdata();
	dLat = navdata.gps.lat;
	dLong = navdata.gps.lon;
	UnlockNavdata();
}


/////////////////////////////////////////////////////////////////////////////
// Get current gps angle. Note: The angle is only correct when the drone is moving !!
/////////////////////////////////////////////////////////////////////////////
double CCustomDrone::GetGpsAngle()
{
	double dDegree = 0.0f;
	
	LockNavdata();
	dDegree = navdata.gps.degree;
	UnlockNavdata();

	return dDegree;
}
