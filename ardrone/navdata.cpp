// -------------------------------------------------------------------------
// CV Drone (= OpenCV + AR.Drone)
// Copyright(C) 2013 puku0x
// https://github.com/puku0x/cvdrone
//
// This source file is part of CV Drone library.
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of EITHER:
// (1) The GNU Lesser General Public License as published by the Free
//     Software Foundation; either version 2.1 of the License, or (at
//     your option) any later version. The text of the GNU Lesser
//     General Public License is included with this library in the
//     file cvdrone-license-LGPL.txt.
// (2) The BSD-style license that is included with this library in
//     the file cvdrone-license-BSD.txt.
// 
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the files
// cvdrone-license-LGPL.txt and cvdrone-license-BSD.txt for more details.
// -------------------------------------------------------------------------

#include "ardrone.h"

// --------------------------------------------------------------------------
// ARDrone::initNavdata()
// Description  : Initialize Navdata.
// Return value : SUCCESS: 1  FAILURE: 0 
// --------------------------------------------------------------------------
int ARDrone::initNavdata(void)
{
	DoLog("Initialize navdata");

    // Open the IP address and port
    if (!sockNavdata.open(ip, ARDRONE_NAVDATA_PORT))
	{
        CVDRONE_ERROR("UDPSocket::open(port=%d) was failed. (%s, %d)\n", ARDRONE_NAVDATA_PORT, __FILE__, __LINE__);
        return 0;
    }

    // Clear Navdata
    ZeroMemory(&navdata, sizeof(ARDRONE_NAVDATA));

    // Start Navdata
    sockNavdata.sendf("\x01\x00\x00\x00");

    // AR.Drone 2.0
    if (version.major == ARDRONE_VERSION_2)
	{
		// Enable the lines below, and comment the rest of the lines, to get all data
		/*
		sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", ++seq, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);        
        sockCommand.sendf("AT*CONFIG=%d,\"general:navdata_demo\",\"FALSE\"\r", ++seq);        
        Sleep(100);

        // Seed ACK
        sockCommand.sendf("AT*CTRL=%d,0\r", ++seq);
		*/
		
        // Disable BOOTSTRAP mode
        sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
        sockCommand.sendf("AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r", seq++);		
		Sleep(100);
		
		// Seed ACK
        sockCommand.sendf("AT*CTRL=%d,0\r", seq++);
		
		// Create options		
		//UINT uiOptions = (1 << ARDRONE_NAVDATA_DEMO_TAG) | (1 << ARDRONE_NAVDATA_HDVIDEO_STREAM_TAG) | (1 << ARDRONE_NAVDATA_WIFI_TAG) | (1 << ARDRONE_NAVDATA_GPS_TAG);
		UINT uiOptions = (1 << ARDRONE_NAVDATA_DEMO_TAG) | (1 << ARDRONE_NAVDATA_HDVIDEO_STREAM_TAG) | (1 << ARDRONE_NAVDATA_GPS_TAG);
		sockCommand.sendf("AT*CONFIG_IDS=%d,\"%s\",\"%s\",\"%s\"\r", seq++, ARDRONE_SESSION_ID, ARDRONE_PROFILE_ID, ARDRONE_APPLOCATION_ID);
		sockCommand.sendf("AT*CONFIG=%d,\"general:navdata_options\",\"%u\"\r", seq++, uiOptions);
        Sleep(100);
    }
    // AR.Drone 1.0
    else
	{
        // Disable BOOTSTRAP mode
        sockCommand.sendf("AT*CONFIG=%d,\"general:navdata_demo\",\"TRUE\"\r", seq++);

        // Send ACK
        sockCommand.sendf("AT*CTRL=%d,0\r", seq++);
    }

    // Enable thread loop
    bflagNavdata = true;

    // Create a thread
    UINT id;
	threadNavdata = (HANDLE)_beginthreadex(NULL, 0, runNavdata, this, 0, &id);
    if (threadNavdata == INVALID_HANDLE_VALUE)
	{
        CVDRONE_ERROR("_beginthreadex() was failed. (%s, %d)\n", __FILE__, __LINE__);
        return 0;
    }

	DoLog("Navdata Initialized");
    return 1;
}

// --------------------------------------------------------------------------
// ARDrone::loopNavdata()
// Description  : Thread function for Navdata.
// Return value : SUCCESS:0
// --------------------------------------------------------------------------
UINT ARDrone::loopNavdata(void)
{
    while (bflagNavdata)
	{
        // Get Navdata
        if (!getNavdata())
		{
			bNeedNavdataRestart = true;
			break;
		}

		// Set the update time
		CSNavdataWD.Enter();
		lNavdataUpdateTime = m_pWatch->Time();
		CSNavdataWD.Leave();

		Sleep(50);
    }

    // Disable thread loop
    bflagNavdata = false;

	return 0;
}


// --------------------------------------------------------------------------
// ARDrone::getNavdata()
// Description  : Get current navigation data of AR.Drone.
// Return value : SUCCESS: 1  FAILURE: 0
// --------------------------------------------------------------------------
int ARDrone::getNavdata(void)
{
    // Send a request
    if(!sockNavdata.sendf("\x01\x00\x00\x00"))
	{
		DoLog("Failed to send data over the navdata socket", MSG_ERROR);
		return 0;
	}

    // Receive data
    char buf[4096] = {'\0'};
    int size = sockNavdata.receive((void*)&buf, sizeof(buf));

    // Received something
    if (size > 0) 
	{
        LockNavdata();

        // Header
        int index = 0;
        memcpy((void*)&(navdata.header),         (const void*)(buf + index), 4); index += 4;
        memcpy((void*)&(navdata.ardrone_state),  (const void*)(buf + index), 4); index += 4;
        memcpy((void*)&(navdata.sequence),       (const void*)(buf + index), 4); index += 4;
        memcpy((void*)&(navdata.vision_defined), (const void*)(buf + index), 4); index += 4;

        // Parse navdata
        while (index < size) 
		{
            // Tag and data size
            unsigned short tmp_tag, tmp_size;
            memcpy((void*)&tmp_tag,  (const void*)(buf + index), 2); index += 2;  // tag
            memcpy((void*)&tmp_size, (const void*)(buf + index), 2); index += 2;  // size
            index -= 4;

            // Copy to NAVDATA structure
            switch (tmp_tag) {
                case ARDRONE_NAVDATA_DEMO_TAG:
                    memcpy((void*)&(navdata.demo),            (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.demo)));
                    break;
                /*
				case ARDRONE_NAVDATA_TIME_TAG:
                    memcpy((void*)&(navdata.time),            (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.time)));
                    break;
                case ARDRONE_NAVDATA_RAW_MEASURES_TAG:
                    memcpy((void*)&(navdata.raw_measures),    (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.raw_measures)));
                    break;
                case ARDRONE_NAVDATA_PHYS_MEASURES_TAG:
                    memcpy((void*)&(navdata.phys_measures),   (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.phys_measures)));
                    break;
                case ARDRONE_NAVDATA_GYROS_OFFSETS_TAG:
                    memcpy((void*)&(navdata.gyros_offsets),   (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.gyros_offsets)));
                    break;
                case ARDRONE_NAVDATA_EULER_ANGLES_TAG:
                    memcpy((void*)&(navdata.euler_angles),    (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.euler_angles)));
                    break;
                case ARDRONE_NAVDATA_REFERENCES_TAG:
                    memcpy((void*)&(navdata.references),      (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.references)));
                    break;
                case ARDRONE_NAVDATA_TRIMS_TAG:
                    memcpy((void*)&(navdata.trims),           (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.trims)));
                    break;
                case ARDRONE_NAVDATA_RC_REFERENCES_TAG:
                    memcpy((void*)&(navdata.rc_references),   (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.rc_references)));
                    break;
                case ARDRONE_NAVDATA_PWM_TAG:
                    memcpy((void*)&(navdata.pwm),             (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.pwm)));
                    break;
                case ARDRONE_NAVDATA_ALTITUDE_TAG:
                    memcpy((void*)&(navdata.altitude),        (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.altitude)));
                    break;
                case ARDRONE_NAVDATA_VISION_RAW_TAG:
                    memcpy((void*)&(navdata.vision_raw),      (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.vision_raw)));
                    break;
                case ARDRONE_NAVDATA_VISION_OF_TAG:
                    memcpy((void*)&(navdata.vision_of),       (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.vision_of)));
                    break;
                case ARDRONE_NAVDATA_VISION_TAG:
                    memcpy((void*)&(navdata.vision),          (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.vision)));
                    break;
                case ARDRONE_NAVDATA_VISION_PERF_TAG:
                    memcpy((void*)&(navdata.vision_perf),     (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.vision_perf)));
                    break;
                case ARDRONE_NAVDATA_TRACKERS_SEND_TAG:
                    memcpy((void*)&(navdata.trackers_send),   (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.trackers_send)));
                    break;
                case ARDRONE_NAVDATA_VISION_DETECT_TAG:
                    memcpy((void*)&(navdata.vision_detect),   (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.vision_detect)));
                    break;
                case ARDRONE_NAVDATA_WATCHDOG_TAG:
                    memcpy((void*)&(navdata.watchdog),        (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.watchdog)));
                    break;
                case ARDRONE_NAVDATA_ADC_DATA_FRAME_TAG:
                    memcpy((void*)&(navdata.adc_data_frame),  (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.adc_data_frame)));
                    break;
                case ARDRONE_NAVDATA_VIDEO_STREAM_TAG:
                    memcpy((void*)&(navdata.video_stream),    (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.video_stream)));
                    break;
                case ARDRONE_NAVDATA_GAME_TAG:
                    memcpy((void*)&(navdata.games),           (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.games)));
                    break;
                case ARDRONE_NAVDATA_PRESSURE_RAW_TAG:
                    memcpy((void*)&(navdata.pressure_raw),    (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.pressure_raw)));
                    break;
                case ARDRONE_NAVDATA_MAGNETO_TAG:
                    memcpy((void*)&(navdata.magneto),         (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.magneto)));
                    break;
                case ARDRONE_NAVDATA_WIND_TAG:
                    memcpy((void*)&(navdata.wind),            (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.wind)));
                    break;
                case ARDRONE_NAVDATA_KALMAN_PRESSURE_TAG:
                    memcpy((void*)&(navdata.kalman_pressure), (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.kalman_pressure)));
                    break;
				*/
                case ARDRONE_NAVDATA_HDVIDEO_STREAM_TAG:
                    memcpy((void*)&(navdata.hdvideo_stream),  (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.hdvideo_stream)));
                    break;
                /*
				case ARDRONE_NAVDATA_WIFI_TAG:
                    memcpy((void*)&(navdata.wifi),            (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.wifi)));
                    break;
				*/
                case ARDRONE_NAVDATA_GPS_TAG:
                    if (version.major == 2 && version.minor == 4) memcpy((void*)&(navdata.gps),        (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.gps)));
                    else                                          memcpy((void*)&(navdata.zimmu_3000), (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.zimmu_3000)));
                    break;
                case 28:
                    break;
                case 29:
                    break;
                default:
                    memcpy((void*)&(navdata.cks),             (const void*)(buf + index), MIN(tmp_size, sizeof(navdata.cks)));
                    break;
            }
            index += tmp_size;
        }

        UnlockNavdata();
    }

    return 1;
}

// --------------------------------------------------------------------------
// ARDrone::getRoll()
// Description  : Get current role angle of AR.Drone.
// Return value : Role angle [rad]
// --------------------------------------------------------------------------
double ARDrone::getRoll(void)
{
	LockNavdata();
    double dRoll = navdata.demo.phi * 0.001 * DEG_TO_RAD;
	UnlockNavdata();
	
	return dRoll;
}

// --------------------------------------------------------------------------
// ARDrone::getPitch()
// Description  : Get current pitch angle of AR.Drone.
// Return value : Pitch angle [rad]
// --------------------------------------------------------------------------
double ARDrone::getPitch(void)
{
	LockNavdata();
    double dPitch = navdata.demo.theta * 0.001 * DEG_TO_RAD;
	UnlockNavdata();

	return dPitch;
}

// --------------------------------------------------------------------------
// ARDrone::getYaw()
// Description  : Get current yaw angle of AR.Drone.
// Return value : Yaw angle [rad]
// --------------------------------------------------------------------------
double ARDrone::getYaw(void)
{
	LockNavdata();
    double dYaw = navdata.demo.psi * 0.001 * DEG_TO_RAD;
	UnlockNavdata();

	return dYaw;
}

// --------------------------------------------------------------------------
// ARDrone::getAltitude()
// Description  : Get current altitude of AR.Drone.
// Return value : Altitude [m]
// --------------------------------------------------------------------------
double ARDrone::getAltitude(void)
{
	LockNavdata();
    double dAltitude = ((double)navdata.demo.altitude) * 0.001f;
	UnlockNavdata();

	return dAltitude;
}

// --------------------------------------------------------------------------
// ARDrone::getVelocity(X velocity[m/s], Y velocity[m/s], Z velocity[m/s])
// Description  : Get estimated velocity of AR.Drone.
// Return value : Velocity [m/s]
// --------------------------------------------------------------------------
double ARDrone::getVelocity(double *vx, double *vy, double *vz)
{
	LockNavdata();
    if (vx) *vx = navdata.demo.vx * 0.001;
    if (vy) *vy = navdata.demo.vy * 0.001;
    if (vz) *vz = navdata.demo.vz * 0.001;
    double dVelocity = sqrt(navdata.demo.vx*navdata.demo.vx + navdata.demo.vy*navdata.demo.vy + navdata.demo.vz*navdata.demo.vz);
	UnlockNavdata();

	return dVelocity;
}

// --------------------------------------------------------------------------
// ARDrone::getBatteryPercentage()
// Description  : Get current battery percentage of AR.Drone.
// Return value : Battery percentage [%]
// --------------------------------------------------------------------------
unsigned int ARDrone::getBatteryPercentage(void)
{
	LockNavdata();
    unsigned int uiBatt = (int)navdata.demo.vbat_flying_percentage;
	UnlockNavdata();

	return uiBatt;
}

// --------------------------------------------------------------------------
// ARDrone::onGround()
// Description  : Check whether AR.Drone is on ground.
// Return value : YES:true NO:false
// --------------------------------------------------------------------------
bool ARDrone::onGround(void)
{
	LockNavdata();
    bool bOnGround = ((navdata.ardrone_state & ARDRONE_FLY_MASK) == 0);
	UnlockNavdata();
    
	return bOnGround;
}

// --------------------------------------------------------------------------
// ARDrone::finalizeNavdata()
// Description  : Finalize Navdata.
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::finalizeNavdata(void)
{
    // Disable thread loop
    bflagNavdata = false;	

	DoLog("Finalize navdata");

    // Destroy the thread
    if (threadNavdata != INVALID_HANDLE_VALUE)
	{
        //WaitForSingleObject(threadNavdata, INFINITE);
		DWORD dwRes = WaitForSingleObject(threadNavdata, 2500);
		switch(dwRes)
		{
			case WAIT_TIMEOUT:
				DoLog("Wait for navdata: thread timed out !", MSG_ERROR);				
				TerminateThread(threadNavdata, -1);
				break;

			case WAIT_FAILED:
				DoLog("Wait for navdata: thread failed !", MSG_ERROR);				
				TerminateThread(threadNavdata, -1);
				break;

			default:
				break;
		}

        if(FALSE == CloseHandle(threadNavdata))
		{
			DoLog("Failed to close navdata thread handle", MSG_ERROR);
		}
        threadNavdata = INVALID_HANDLE_VALUE;
    }

    // Close the socket
    sockNavdata.close();

	DoLog("Navdata finalized");
}