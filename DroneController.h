/////////////////////////////////////////////////////////////////////////////
// Main application definition
/////////////////////////////////////////////////////////////////////////////

#pragma once

#include "wx/wx.h"
#include <wx/thread.h>
#include <wx/sound.h>
#include <wx/stopwatch.h>
// To simulate mouse move
#include <wx/uiaction.h>

// Test: try to veto sleep
#ifdef wxHAS_POWER_EVENTS
#include <wx/power.h>
#endif

#include "CustomDrone.h"
#include "Input.h"
#include "ScreenManager.h"
#include "WifiManager.h"
#include "AutoPilot.h"

// Global flag, if debug informations should be displayed
bool g_bDebug = false;

// Application states
enum eAppState
{
	STATE_NONE				= 0x0000,
	STATE_FULLSCREEN		= 0x0001,
	STATE_RECORDING			= 0x0002,
	STATE_RECORDINGONPC		= 0x0004,
	STATE_RECORDINGONUSB	= 0x0008,
	STATE_CONNECTEDTODRONE	= 0x0010,
	STATE_RETURNHOMEACTIVE	= 0x0020,
};

////////////////////////////////////////////////////////////////////////////
// Application entry point
/////////////////////////////////////////////////////////////////////////////
class CMainApp : public wxApp
{
public:
    virtual bool	OnInit();
	virtual int		OnExit();
};


/////////////////////////////////////////////////////////////////////////////
// Class for the main frame of the application
/////////////////////////////////////////////////////////////////////////////
class CDroneController: public wxFrame, public wxThreadHelper
{
public:
    CDroneController(const wxString& title);
    virtual	~CDroneController();

	// Initialize the drone controller object
	bool DoInit();	

protected:	
	// Close the window
	void OnCloseWindow(wxCloseEvent& event);
	// Resize the window
	void OnResize(wxSizeEvent& event);

	// Connect to the drone
	bool Connect();
	// Disconnect from drone
	bool Disconnect();

	// Start or stop recording
	void StartRecord();
	void StopRecord();

	// Update states
	void OnTimerUpdate(wxTimerEvent& WXUNUSED(event));

	// Menu options
	void OnGetDroneConfig(wxCommandEvent& WXUNUSED(event));
	void OnQuit(wxCommandEvent& WXUNUSED(event));
	void OnConfig(wxCommandEvent& WXUNUSED(event));
	void OnKeyboard(wxCommandEvent& WXUNUSED(event));
	void OnJoystick(wxCommandEvent& WXUNUSED(event));
	void OnIpAddress(wxCommandEvent& WXUNUSED(event));
	void OnAbout(wxCommandEvent& WXUNUSED(event));
	void OnEmergency(wxCommandEvent& WXUNUSED(event));
	void OnTrim(wxCommandEvent& WXUNUSED(event));
	void OnLandTakeOff(wxCommandEvent& WXUNUSED(event));	
	void OnRecord(wxCommandEvent& WXUNUSED(event));
	void OnScreenshot(wxCommandEvent& WXUNUSED(event));
	void OnFullscreen(wxCommandEvent& WXUNUSED(event));
	void OnCamera(wxCommandEvent& WXUNUSED(event));
	void OnHome(wxCommandEvent& WXUNUSED(event));
	void OnHomeReset(wxCommandEvent& WXUNUSED(event));
	void OnConnect(wxCommandEvent& WXUNUSED(event));
	void OnDisconnect(wxCommandEvent& WXUNUSED(event));
#ifdef wxHAS_POWER_EVENTS
	void OnSleep(wxPowerEvent& event);
#endif

private:
	// Set limits from config (Speed and altitude)
	void SetLimits();
	// Compute new positions of the graphical items after a resize event
	void ComputePositions(int iPanelWidth, int iPanelHeight);
	// The code executed by the thread (main loop)
	wxThread::ExitCode	Entry();
	// Draw the main panel with camera capture
	bool				DoRender();
	// Draw status informations (battery, wifi...)
	void				DrawStatus(wxDC& dc);
	// Draw the basic version of the HUD, and all screen informations
	void				DrawHUD1(wxDC& dc);
	// Draw the advanced version of the HUD, and all screen informations
	void				DrawHUD2(wxDC& dc);
	// Draw debug informations
	void				DrawDebugInfo(wxDC& dc);

	// Set speed an altitude text depending on current units (meters or feets)
	void SetUnitText();

	// Read Set and reset an application status
	bool HasStatus(eAppState AppStatus);
	void SetStatus(eAppState AppStatus);
	void ResetStatus(eAppState AppStatus);

	// Refresh the text after a language change
	bool RefreshGUIText();

	// Start and stop recording
	void StartPCRecording();
	void StopPCRecording();
	void StartUSBRecording();
	void StopUSBRecording();

// NOTE (23.08.2014, Version 1.5.4) class CDroneController have now a lot of members,
// a refactoring should be done to keep the overview.
private:
	// The displayed image
	wxImage				m_Image;
	// Bitmap use as buffer for wxBufferedDC
	wxBitmap			m_BufferBitmap;
	// The main panel where we draw to
	wxPanel*			m_pPanel;
	// The toolbar
	wxToolBar*			m_pToolbar;
	// The menu bar
	wxMenuBar*			m_pMenu;
	// The status bar
	wxStatusBar*		m_pStatusBar;	
	// Input management for keyboard and joystick
	CInput				m_Input;
	// The drone instance
	CCustomDrone		m_Drone;
	// Wifi manager
	CWifiManager		m_Wifi;
	// Timer to update wifi status and control sounds
	wxTimer				m_Timer;
	// Simulator for mouse moves
	wxUIActionSimulator	m_ActionSimulator;
	// To alternate mouse direction - One time to the left, ane time to the right
	unsigned char		m_ucKeepAwake;

	// Sounds to play (one beep and multiple beeps)
	wxSound*			m_pBeep;
	wxSound*			m_pBeeps;

	// Battery, wifi, record and home info (position does never change)
	wxRect				m_BatteryRect;
	wxRect				m_WifiRect;
	wxRect				m_RecordRect;
	wxRect				m_HomeRect;

	// Bitmaps used for battery, wifi, record and home pictures
	wxBitmap			m_BatteryGreenBitmap;
	wxBitmap			m_BatteryYellowBitmap;
	wxBitmap			m_BatteryOrangeBitmap;
	wxBitmap			m_BatteryRedBitmap;
	wxBitmap			m_WifiGreenBitmap;
	wxBitmap			m_WifiYellowBitmap;
	wxBitmap			m_WifiOrangeBitmap;
	wxBitmap			m_WifiRedBitmap;
	wxBitmap			m_RecordBitmap;
	wxBitmap			m_HomeBitmap;

	// Text displayed when there is no connection
	wxString			m_strNoConnection;
	// Text for altitude
	wxString			m_strAltitude;
	// Text for speed
	wxString			m_strSpeed;
	// Text for debug output
	wxString			m_strDebug;

	// Application state
	unsigned short		m_usAppState;
	// Current camera mode
	int					m_iCameraMode;
	// Max altitude allowed
	double				m_dMaxAltitude;		

	// Sizes and positions
	int					m_iPanelWidth;
	int					m_iPanelHeight;
	int					m_iMiddleX;			// Horizontal middle
	int					m_iMiddleY;			// Vertical middle
	int					m_iRadius;			// Central circle radius
	double				m_dRadiusOver2;		// For HUD2 - 50% of radius
	double				m_dRadiusOver20;	// For HUD2 - 5% of radius

	// Text positions
	wxRect				m_SpeedInfoRect;
	wxRect				m_SpeedInfoTextRect;
	wxRect				m_AltInfoRect;
	wxRect				m_AltInfoTextRect;
	wxRect				m_DirInfoRect;
	wxRect				m_ErrorRect;
	wxRect				m_DebugRect;
	
	// Color, Pen and brush used to draw the HUD
	wxColour			m_HudColor;
	wxPen				m_HudPen;
	wxBrush				m_HudBrush;
	wxCriticalSection	m_CSDrawing;

	// Recording on file
	CvVideoWriter*		m_pVideoWriter;		// The writer
	wxCriticalSection	m_CSVideo;			// The critical section	
	wxLongLong			m_llNextFrameTime;	// Target time where we have to record the next frame

	// The watch used for time measurement
	wxStopWatch			m_Watch;
	// Another watch, used for flying time
	wxStopWatch			m_WatchFlyingTime;
	// State of the watch
	bool				m_bFlyingTimeWachActive;

	// The autopilot used for ReturnToHome functionnality
	CAutoPilot			m_AutoPilot;

	// Manages screen resolution
	CScreenManager		m_ScreenManager;

private:
    DECLARE_EVENT_TABLE()	
};


