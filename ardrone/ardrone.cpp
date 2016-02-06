// -------------------------------------------------------------------------
// CV Drone (= OpenCV + AR.Drone)
// Copyright(C) 2013 puku0x
// https://github.com/puku0x/cvdrone
//
// This program is free software; you can redistribute it and/or
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
// ARDrone::ARDrone()
// Description : Constructor of ARDrone class.
// --------------------------------------------------------------------------
ARDrone::ARDrone()
{
    // Init members
	Clear();
}

// --------------------------------------------------------------------------
// ARDrone::~ARDrone()
// Description : Destructor of ARDrone class.
// --------------------------------------------------------------------------
ARDrone::~ARDrone()
{
    if(bflagCommand || bflagNavdata || bflagVideo || bflagWatchdog)
	{
		// Finalize the AR.Drone
		close();
	}
}


/////////////////////////////////////////////////////////////////////////////
// Clear members
/////////////////////////////////////////////////////////////////////////////
void ARDrone::Clear()
{
	// IP Address
    strncpy(ip, ARDRONE_DEFAULT_ADDR, 16);

    // Sequence number
    seq = 1;

    // Camera image
    img = NULL;

     // Version information
    memset(&version, 0, sizeof(ARDRONE_VERSION));

    // Navdata
    memset(&navdata, 0, sizeof(ARDRONE_NAVDATA));

    // Configurations
    memset(&config, 0, sizeof(ARDRONE_CONFIG));

    // Video
    pFormatCtx  = NULL;
    pCodecCtx   = NULL;
    pFrame      = NULL;
    pFrameBGR   = NULL;
    bufferBGR   = NULL;
    pConvertCtx = NULL;

    // Thread for commnad
    bflagCommand  = false;
	bNeedCommandRestart = false;
    threadCommand = INVALID_HANDLE_VALUE;
	lCommandUpdateTime = 0;

    // Thread for navdata
    bflagNavdata  = false;
	bNeedNavdataRestart = false;
    threadNavdata = INVALID_HANDLE_VALUE;
	lNavdataUpdateTime = 0;

    // Thread for video
    bflagVideo  = false;
	bNeedVideoRestart = false;
    threadVideo = INVALID_HANDLE_VALUE;
	lVideoUpdateTime = 0;

	// Watchdog
	bflagWatchdog = false;
	threadWatchdog = INVALID_HANDLE_VALUE;
}


// --------------------------------------------------------------------------
// ARDrone::open(IP address of AR.Drone)
// Description  : Initialize the AR.Drone.
// Return value : SUCCESS: 1  FAILURE: 0
// --------------------------------------------------------------------------
int ARDrone::open(wxStopWatch* pWatch, const char *ardrone_addr)
{
	DoLog(wxString::Format("Try to open a connection to %s", ardrone_addr));

	// Set the stop watch
	if(NULL == pWatch)
	{
		return 0;
	}
	m_pWatch = pWatch;

	// Initialize WSA
    WSAData wsaData;
    WSAStartup(MAKEWORD(1,1), &wsaData);
	
	DoLog("Winsock data initialized");

    // Initialize FFmpeg
    av_register_all();
    avformat_network_init();
    av_log_set_level(AV_LOG_QUIET);

	DoLog("FFMPEG initialized");

    // Save IP address
    strncpy(ip, ardrone_addr, 16);

    // Get version informations
    if (!getVersionInfo())
	{
		DoLog("Failed to get version info", MSG_ERROR);
		return 0;
	}
	
	DoLog(wxString::Format("AR Drone Version: %d.%d.%d\n", version.major, version.minor, version.revision));	
	
	// Initialize AT Command
	if (!initCommand()) 
	{
		return 0;
	}

	// Initialize Navdata
	if (!initNavdata()) 
	{
		return 0;
	}

    // Initialize Video
    if (!initVideo())
	{
		return 0;
	}

	// Initialize Watchdog
	if(!initWatchdog())
	{
		return 0;
	}

    // Wait for updating the state
    Sleep(500);

	// Reset emergency
	resetWatchDog();
	resetEmergency();

	DoLog(wxString::Format("Connection to %s succesfully opened", ardrone_addr));
    return 1;
}


// --------------------------------------------------------------------------
// ARDrone::close()
// Description  : Finalize
// Return value : NONE
// --------------------------------------------------------------------------
void ARDrone::close(void)
{
	DoLog("Try to close connection");

    // Stop AR.Drone
	if (!onGround())
	{
		DoLog("Drone flying, need to land");
		landing();
	}

	// Finalize watchdog thread
	finalizeWatchdog();

    // Finalize video
    finalizeVideo();

	// Finalize Navdata
	finalizeNavdata();

	// Finalize AT command
	finalizeCommand();

    // Finalize WSA
    WSACleanup();

	// Clear the members
	Clear();

	DoLog("Connection closed");
}


// Lock and unlock navdata mutex
void ARDrone::LockNavdata()
{
	CSNavdata.Enter();
}

void ARDrone::UnlockNavdata()
{
	CSNavdata.Leave();
}

// Lock and unlock command mutex
void ARDrone::LockCommand()
{
	CSCommand.Enter();
}

void ARDrone::UnlockCommand()
{
	CSCommand.Leave();
}

// Lock and unlock video mutex
void ARDrone::LockVideo()
{
	CSVideo.Enter();
}

void ARDrone::UnlockVideo()
{
	CSVideo.Leave();
}