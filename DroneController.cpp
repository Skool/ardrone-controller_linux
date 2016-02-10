/////////////////////////////////////////////////////////////////////////////
// Main application implementation
/////////////////////////////////////////////////////////////////////////////

#include <wx/dcbuffer.h>
#include <wx/tokenzr.h>

#include "DroneController.h"
#include "Utils.h"
#include "AppConfig.h"
#include "AboutDialog.h"
#include "ConfigDialog.h"
#include "JoystickDialog.h"
#include "KeyboardDialog.h"

IMPLEMENT_APP(CMainApp)

/////////////////////////////////////////////////////////////////////////////
// Identifier for events
/////////////////////////////////////////////////////////////////////////////
enum
{
	TIMER_UPDATE = 100,
	MENU_CONNECT,
	MENU_DISCONNECT,
	MENU_CFG,
	MENU_QUIT,
	MENU_CONFIG,
	MENU_KEYBOARD,
	MENU_JOYSTICK,
	MENU_IPADDRESS,
	MENU_ABOUT,
	MENU_EMERGENCY,
	MENU_TRIM,
	MENU_LANDTAKEOFF,
	MENU_RECORD,
	MENU_SCREENSHOT,
	MENU_FULLSCREEN,
	MENU_CAMERA,
	MENU_HOME,
	MENU_HOMERESET
};


/////////////////////////////////////////////////////////////////////////////
// Initialization of the application
/////////////////////////////////////////////////////////////////////////////
bool CMainApp::OnInit()
{
	// Set the debug flag if the file debug.txt exist
	// This allows to enable/disable debug informations without recompile
	if(wxFileExists("debug.txt"))
	{
		g_bDebug = true;
	}

	// Save up to 2 log files
	if(wxFileExists("Data/Log/Errors2.log"))
	{
		wxRemoveFile("Data/Log/Errors2.log");
	}
	if(wxFileExists("Data/Log/Errors1.log"))
	{
		wxRenameFile("Data/Log/Errors1.log", "Data/Log/Errors2.log");
	}
	if(wxFileExists("Data/Log/Errors.log"))
	{
		wxRenameFile("Data/Log/Errors.log", "Data/Log/Errors1.log");
	}

	DoLog("--------------------------------------------------------------");	
	DoLog(VersionString);
	DoLog(wxString::Format("OS: %s", wxGetOsDescription()));
	DoLog(wxNow());
	DoLog("--------------------------------------------------------------");

	// Load config and language
	CConfig* pConfig = CConfig::GetSingleton();
	if(NULL == pConfig)
	{
		DoLog("Memory error : Could not create main configuration object !", MSG_ERROR);
		DoMessage("Memory error : Could not create main configuration object !", MSG_ERROR);
		return false;
	}
	else
	{
		// Check that the installation is correct
		if(!pConfig->CheckConfig())
		{
			DoMessage("Installation is corrupted, please reinstall the application !", MSG_ERROR);
			return false;
		}
		if(!pConfig->LoadConfig())
		{
			DoLog("Failed to load saved configuration !", MSG_ERROR);
			DoMessage("Failed to load saved configuration !", MSG_ERROR);
			return false;
		}
		if(!pConfig->LoadLanguage())
		{
			DoLog("Failed to load language file !", MSG_ERROR);
			DoMessage("Failed to load language file !", MSG_ERROR);
			return false;
		}
	}
		
	if ( !wxApp::OnInit() )
	{
		DoMessage(GetText("AppLoadFailed"), MSG_ERROR);
		return false;
	}	

	// Needed for PNG files (used for transparancy)
	wxImage::AddHandler(new wxPNGHandler);

	CDroneController* pDroneApp = new CDroneController(VersionString);
	if(NULL == pDroneApp)
	{
		DoLog("Failed to create drone controller object !", MSG_ERROR);
		DoMessage("Failed to create drone controller object", MSG_ERROR);
		return false;
	}

	if(!pDroneApp->DoInit())
	{
		DoLog("Failed to initialize drone controller object !", MSG_ERROR);
		DoMessage("Failed to initialize drone controller object", MSG_ERROR);
		return false;
	}	

    return true;
}


/////////////////////////////////////////////////////////////////////////////
// Initialization of the application
/////////////////////////////////////////////////////////////////////////////
int CMainApp::OnExit()
{
	CConfig::KillSingleton();
	return 0;
}


/////////////////////////////////////////////////////////////////////////////
// Dialog event table
/////////////////////////////////////////////////////////////////////////////
BEGIN_EVENT_TABLE(CDroneController, wxFrame)
    EVT_CLOSE(CDroneController::OnCloseWindow)
	EVT_TIMER(TIMER_UPDATE, CDroneController::OnTimerUpdate)
	EVT_MENU(MENU_CONNECT, CDroneController::OnConnect)
	EVT_MENU(MENU_DISCONNECT, CDroneController::OnDisconnect)
	EVT_MENU(MENU_CFG, CDroneController::OnGetDroneConfig)
	EVT_MENU(MENU_QUIT, CDroneController::OnQuit)
	EVT_MENU(MENU_CONFIG, CDroneController::OnConfig)
	EVT_MENU(MENU_KEYBOARD, CDroneController::OnKeyboard)
	EVT_MENU(MENU_JOYSTICK, CDroneController::OnJoystick)
	EVT_MENU(MENU_IPADDRESS, CDroneController::OnIpAddress)
	EVT_MENU(MENU_ABOUT, CDroneController::OnAbout)
	EVT_MENU(MENU_EMERGENCY, CDroneController::OnEmergency)
	EVT_MENU(MENU_TRIM, CDroneController::OnTrim)
	EVT_MENU(MENU_LANDTAKEOFF, CDroneController::OnLandTakeOff)	
	EVT_MENU(MENU_RECORD, CDroneController::OnRecord)
	EVT_MENU(MENU_SCREENSHOT, CDroneController::OnScreenshot)
	EVT_MENU(MENU_FULLSCREEN, CDroneController::OnFullscreen)
	EVT_MENU(MENU_CAMERA, CDroneController::OnCamera)
	EVT_MENU(MENU_HOME, CDroneController::OnHome)
	EVT_MENU(MENU_HOMERESET, CDroneController::OnHomeReset)
#ifdef wxHAS_POWER_EVENTS
	EVT_POWER_SUSPENDING(CDroneController::OnSleep)
#endif
END_EVENT_TABLE()


/////////////////////////////////////////////////////////////////////////////
// Constructor
/////////////////////////////////////////////////////////////////////////////
CDroneController::CDroneController(const wxString& title) : wxFrame(NULL, wxID_ANY, title, wxDefaultPosition, wxSize(1280,720), wxDEFAULT_DIALOG_STYLE | wxRESIZE_BORDER | wxMINIMIZE_BOX | wxMAXIMIZE_BOX)
{
	// If needed, the dialogs will have a scrollbar (needed for small screens)
	wxDialog::EnableLayoutAdaptation(true);

	// Set the application icon
	SetIcon(wxICON(IcoMain));	

	// Create the main panel
	m_pPanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxBORDER_SIMPLE|wxTAB_TRAVERSAL);

	// Connect resize and paint events to the panel
	m_pPanel->Connect(wxEVT_SIZE, wxSizeEventHandler( CDroneController::OnResize ), NULL, this);	

	// Set minimum size
	this->SetMinSize(wxSize(640,360));

	// Get display size
	int iDisplayX=0, iDisplayY=0;
	::wxDisplaySize(&iDisplayX, &iDisplayY);

	// If screen size is equal or smaller than the default size, resize the window
	if( (iDisplayX <= 1280) || (iDisplayY <= 720) )
	{
		// New horizontal size is 75% of the current screen wide
		iDisplayX = (iDisplayX/4)*3;
		
		// For Height use the 16/9 format of V2 camera, 
		// even if we dont know at this time if  we are using drone V1 or V2
		iDisplayY = (iDisplayX/16)*9;

		this->SetSize(iDisplayX, iDisplayY);
	}

	// Create the main sub menu
	wxMenu *pSubMenuMain = new wxMenu;
	pSubMenuMain->Append( MENU_CFG, GetText("MainGetCfg") );
    pSubMenuMain->Append( MENU_QUIT, GetText("MainExit") );

	// Create the config sub menu
	wxMenu *pSubMenuConfig = new wxMenu;
    pSubMenuConfig->Append( MENU_CONFIG, GetText("ConfigApp") );
	pSubMenuConfig->Append( MENU_KEYBOARD, GetText("ConfigKey") );
	pSubMenuConfig->Append( MENU_JOYSTICK, GetText("ConfigJoy") );
	pSubMenuConfig->Append( MENU_IPADDRESS, GetText("ConfigIp") );

	// Create the sub menu
	wxMenu *pSubMenuAbout = new wxMenu;
	pSubMenuAbout->Append( MENU_ABOUT, GetText("About") );

    m_pMenu = new wxMenuBar(wxBORDER_NONE);
    m_pMenu->Append( pSubMenuMain, GetText("Main") );
	m_pMenu->Append( pSubMenuConfig, GetText("Config") );
	m_pMenu->Append( pSubMenuAbout, "?" );

    SetMenuBar( m_pMenu );

	// Create a toolbar
	m_pToolbar = new wxToolBar(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTB_FLAT | wxTB_HORIZONTAL);
	m_pToolbar->AddTool(MENU_CONNECT, wxEmptyString, wxBitmap(wxICON(IcoConnect)), GetText("Connect"));
	m_pToolbar->AddTool(MENU_DISCONNECT, wxEmptyString, wxBitmap(wxICON(IcoDisconnect)), GetText("Disconnect"));
	m_pToolbar->AddSeparator();
	m_pToolbar->AddTool(MENU_EMERGENCY, wxEmptyString, wxBitmap(wxICON(IcoEmergency)), GetText("Emergency"));
	m_pToolbar->AddSeparator();
	m_pToolbar->AddTool(MENU_TRIM, wxEmptyString, wxBitmap(wxICON(IcoTrim)), GetText("Trim"));
	m_pToolbar->AddTool(MENU_LANDTAKEOFF, wxEmptyString, wxBitmap(wxICON(IcoTakeOff)), GetText("LandTakeOff"));
	m_pToolbar->AddSeparator();	
	m_pToolbar->AddTool(MENU_FULLSCREEN, wxEmptyString, wxBitmap(wxICON(IcoFullscreen)), GetText("Fullscreen"));
	m_pToolbar->AddTool(MENU_CAMERA, wxEmptyString, wxBitmap(wxICON(IcoCamera)), GetText("Camera"));
	m_pToolbar->AddTool(MENU_RECORD, wxEmptyString, wxBitmap(wxICON(IcoRecord)), GetText("Record"));
	m_pToolbar->AddTool(MENU_SCREENSHOT, wxEmptyString, wxBitmap(wxICON(IcoScreenshot)), GetText("Screenshot"));
	m_pToolbar->AddSeparator();
	m_pToolbar->AddTool(MENU_HOME, wxEmptyString, wxBitmap(wxICON(IcoHome)), GetText("Home"));
	m_pToolbar->AddTool(MENU_HOMERESET, wxEmptyString, wxBitmap(wxICON(IcoHomeReset)), GetText("HomeReset"));
	m_pToolbar->Realize();

	// Disable the disconnect button
	m_pToolbar->EnableTool(MENU_DISCONNECT, false);

	SetToolBar(m_pToolbar);

	// Create the status bar
	m_pStatusBar = new wxStatusBar(this, wxID_ANY, wxSIMPLE_BORDER);
	m_pStatusBar->SetFieldsCount(3);
	m_pStatusBar->SetStatusText(wxT(""),0);
	m_pStatusBar->SetStatusText(wxT(""),1);
	m_pStatusBar->SetStatusText(wxT(""),2);

	// Set specific sizes
	int WxStatusBar_Widths[3];
	WxStatusBar_Widths[0] = 5;	// Fixed size
	WxStatusBar_Widths[1] = -1;	// Auto size
	WxStatusBar_Widths[2] = -1;	// Auto size
	m_pStatusBar->SetStatusWidths(3,WxStatusBar_Widths);
	
	SetStatusBar(m_pStatusBar);

	// Init all members
	m_usAppState		= STATE_NONE;
	m_iCameraMode		= 0;
	m_dMaxAltitude		= 3.0f;	
	m_iPanelWidth		= 0;
	m_iPanelHeight		= 0;
	m_iMiddleX			= 0;
	m_iMiddleY			= 0;
	m_HudColor			= CConfig::GetSingleton()->GetHudColor();
	m_HudPen			= wxPen(m_HudColor, CConfig::GetSingleton()->GetHudLineSize());
	m_HudBrush			= wxBrush(m_HudColor, wxBRUSHSTYLE_TRANSPARENT);

	// Text to display as long there is no connection to the drone
	m_strNoConnection	= GetText("NotConnected");

	// Set the unit text to the current unit set in config
	SetUnitText();

	// OpenCv video writer
	m_pVideoWriter		= NULL;

	if(!GetPngFromRessource("PngWifiGreen", m_WifiGreenBitmap))
	{
		DoLog("Failed to load the green wifi image", MSG_ERROR);
	}
	if(!GetPngFromRessource("PngWifiYellow", m_WifiYellowBitmap))
	{
		DoLog("Failed to load the yellow wifi image", MSG_ERROR);
	}
	if(!GetPngFromRessource("PngWifiOrange", m_WifiOrangeBitmap))
	{
		DoLog("Failed to load the orange wifi image", MSG_ERROR);
	}
	if(!GetPngFromRessource("PngWifiRed", m_WifiRedBitmap))
	{
		DoLog("Failed to load the red wifi image", MSG_ERROR);
	}

	if(!GetPngFromRessource("PngBatteryGreen", m_BatteryGreenBitmap))
	{
		DoLog("Failed to load the green battery image", MSG_ERROR);
	}
	if(!GetPngFromRessource("PngBatteryYellow", m_BatteryYellowBitmap))
	{
		DoLog("Failed to load the yellow battery image", MSG_ERROR);
	}
	if(!GetPngFromRessource("PngBatteryOrange", m_BatteryOrangeBitmap))
	{
		DoLog("Failed to load the orange battery image", MSG_ERROR);
	}
	if(!GetPngFromRessource("PngBatteryRed", m_BatteryRedBitmap))
	{
		DoLog("Failed to load the red battery image", MSG_ERROR);
	}

	if(!GetPngFromRessource("PngRecord", m_RecordBitmap))
	{
		DoLog("Failed to load the Record image", MSG_ERROR);
	}
	if(!GetPngFromRessource("PngHome", m_HomeBitmap))
	{
		DoLog("Failed to load the Home image", MSG_ERROR);
	}

	// Init time
	m_llNextFrameTime = 0;

	// Init mouse movement
	m_ucKeepAwake = 0;
	
	// Pause watch as we want to count only flying time
	m_WatchFlyingTime.Pause();
	m_bFlyingTimeWachActive = false;

	// That values never changes
	m_BatteryRect		= wxRect(10,10,32,32);
	m_WifiRect			= wxRect(10,60,32,32);
	m_RecordRect		= wxRect(10,110,32,32);
	m_HomeRect			= wxRect(10,160,32,32);

#ifdef _WIN32
	// Create sounds from ressource
	m_pBeep = new wxSound();
	if(!m_pBeep->Create("WavBeep", true))
	{
		DoLog("Failed to load sound WavBeep", MSG_ERROR);
		delete m_pBeep;
		m_pBeep = NULL;
	}

	m_pBeeps = new wxSound();
	if(!m_pBeeps->Create("WavBeeps", true))
	{
		DoLog("Failed to load sound WavBeeps", MSG_ERROR);
		delete m_pBeeps;
		m_pBeeps = NULL;
	}	
#endif
	// Display the dialog
	this->CenterOnScreen();
	Show();
}


/////////////////////////////////////////////////////////////////////////////
// Destructor
/////////////////////////////////////////////////////////////////////////////
CDroneController::~CDroneController()
{
	DoLog("Cleaning Main Frame instance");

	// Stop the worker thread
	if(NULL != m_thread)
	{
		m_thread->Delete();
	}
	
	// Disconnect size events
	m_pPanel->Disconnect(wxEVT_SIZE, wxSizeEventHandler( CDroneController::OnResize ), NULL, this);

	// Disconnect from drone, stop timer and clean wifi
	Disconnect();

	// Delete sound ressources
	if(NULL != m_pBeep)
	{
		delete m_pBeep;
		m_pBeep=NULL;
	}

	if(NULL != m_pBeeps)
	{
		delete m_pBeeps;
		m_pBeeps=NULL;
	}
}


/////////////////////////////////////////////////////////////////////////////
// Initialize the drone controller object
/////////////////////////////////////////////////////////////////////////////
bool CDroneController::DoInit()
{
	DoLog("Initialize main frame");

	// Calculate positions
	int X=0,Y=0;
	m_pPanel->GetSize(&X, &Y);
	ComputePositions(X, Y);

	// Saved data are already loaded, set the limits (speed, altitude...)
	SetLimits();

	// Create and start the worker thread
	if(CreateThread(wxTHREAD_DETACHED) != wxTHREAD_NO_ERROR)
	{
		DoLog("Failed to create the worker thread !", MSG_ERROR);
		DoMessage(GetText("AppLoadFailed"), MSG_ERROR);	
		return false;
	}
	if (GetThread()->Run() != wxTHREAD_NO_ERROR)
	{
		DoLog("Failed to start the worker thread !", MSG_ERROR);
		DoMessage(GetText("AppLoadFailed"), MSG_ERROR);	
		return false;	
	}

	// Init the timer
	m_Timer.SetOwner(this, TIMER_UPDATE);

	DoLog("Main frame initialized");

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Dialog is closed
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnCloseWindow(wxCloseEvent& WXUNUSED(event))
{
	if(HasStatus(STATE_CONNECTEDTODRONE))
	{
		if(!m_Drone.onGround())
		{
			DoLog("Close event while drone is flying, request confirmation");

			// Not on ground, ask for confirmation !
			if(wxYES != wxMessageBox(GetText("ExitFlying"), GetText("Warning"), wxICON_EXCLAMATION | wxYES_NO))
			{
				return;
			}
		}
	}
	
	DoLog("Program will be closed");

	Destroy();
}


/////////////////////////////////////////////////////////////////////////////
// Resize the window
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnResize(wxSizeEvent& WXUNUSED(event))
{
	int X=0, Y=0;	

	// Get current panel size	
	m_pPanel->GetSize(&X, &Y);	
	m_pPanel->Refresh();

	ComputePositions(X, Y);
}


/////////////////////////////////////////////////////////////////////////////
// Connect to the drone
/////////////////////////////////////////////////////////////////////////////
bool CDroneController::Connect()
{
	if(!HasStatus(STATE_CONNECTEDTODRONE))
	{
		// Network name
		wxString strNetName;

		// Reset all keys
		m_Input.ResetFlag((eKey)0xFFFF);

		// Init the drone and connect to it
		if(!m_Drone.open(CConfig::GetSingleton()->GetIpAddress().ToAscii()))
		{
			DoLog("Failed to connect to the drone, clean up !", MSG_ERROR);
			
			// Clean up the drone object
			m_Drone.close();
			
			DoMessage(GetText("ConnectFail"), MSG_ERROR);						
			return false;
		}

		// Send max altitude to the drone
		m_Drone.SetMaxAltitude(CConfig::GetSingleton()->GetAltitudeLimit());

		// Set the flag to connected
		SetStatus(STATE_CONNECTEDTODRONE);

		// Initialize Wifi
		/*eWifiErrors wErr = m_Wifi.Init();
		strNetName = m_Wifi.GetNetworkName();
		switch(wErr)
		{
			case WIFI_SUCCESS:
			{
				DoLog("Wifi initialized succesfully");
				DoLog("connected to: " + strNetName);
			}
			break;

			case WIFI_WRN_MULTIINTERFACES:
			{
				DoLog("Wifi initialized, but more than one connected interface has been found", MSG_WARNING);
				DoLog("connected to: " + strNetName);
			}
			break;

			case WIFI_WRN_MULTINETWORKS:
			{
				DoLog("Wifi initialized, but more than one connected network has been found", MSG_WARNING);
				DoLog("connected to: " + strNetName);
			}
			break;

			default:
			{
				// This is the only real error case: The program will start without wifi signal watching.
				DoMessage(GetText("WifiFailed"), MSG_ERROR);
				DoLog("Failed to initialize the wifi adapter !", MSG_ERROR);
				strNetName = "?";
			}
			break;
		}
		*/

		// Start the timer
		if(!m_Timer.Start(2000))
		{
			DoLog("Failed to start internal timer !", MSG_ERROR);
		}

		// Reset the watch to 0
		m_WatchFlyingTime.Start();
		// Pause it (it's running...)
		m_WatchFlyingTime.Pause();
		m_bFlyingTimeWachActive = false;

		// Set text to the status bar
		m_pStatusBar->SetStatusText(GetText("ConnectedTo") + strNetName, 1);
		m_pStatusBar->SetStatusText(GetText("DroneVersion") + wxString::Format("%d", m_Drone.getVersion()), 2);
	}

	// Disable the connect button and enable the disconnect button
	m_pToolbar->EnableTool(MENU_CONNECT, false);
	m_pToolbar->EnableTool(MENU_DISCONNECT, true);

	//m_pMenu->Enable(MENU_CFG, false);

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Disconnect from drone
/////////////////////////////////////////////////////////////////////////////
bool CDroneController::Disconnect()
{
	if(HasStatus(STATE_CONNECTEDTODRONE))
	{
		// Stop recording, if needed
		StopRecord();
		
		// Set the flag to disconnected
		ResetStatus(STATE_CONNECTEDTODRONE);

		// Stop the timer
		m_Timer.Stop();

		// Clean Wifi manager
		//m_Wifi.Clean();

		// Close connection to the drone
		m_Drone.close();

		// Reset the status bar text
		m_pStatusBar->SetStatusText("", 1);
		m_pStatusBar->SetStatusText("", 2);
	}

	// Enable the connect button and disable the disconnect button
	m_pToolbar->EnableTool(MENU_CONNECT, true);
	m_pToolbar->EnableTool(MENU_DISCONNECT, false);

	//m_pMenu->Enable(MENU_CFG, true);

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Start recording
/////////////////////////////////////////////////////////////////////////////
void CDroneController::StartRecord()
{
	if(!HasStatus(STATE_RECORDING))
	{
		if(m_Drone.HasUsbKey())
		{
			switch(CConfig::GetSingleton()->GetRecordOption())
			{
				case 3:		//PC
					StartPCRecording();
					break;
				case 2:		// USB + PC
					StartUSBRecording();
					msleep(100);
					StartPCRecording();
					break;
				default:	// USB
					StartUSBRecording();
					break;
			}
		}
		else
		{
			// Without usb key, only pc recording is possible
			StartPCRecording();
		}

		SetStatus(STATE_RECORDING);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Stop recording
/////////////////////////////////////////////////////////////////////////////
void CDroneController::StopRecord()
{
	if(HasStatus(STATE_RECORDING))
	{
		if(HasStatus(STATE_RECORDINGONPC))
		{
			StopPCRecording();
		}

		if(HasStatus(STATE_RECORDINGONUSB))
		{
			StopUSBRecording();
		}

		ResetStatus(STATE_RECORDING);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Connect to the drone
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnConnect(wxCommandEvent& WXUNUSED(event))
{
	Connect();
}


/////////////////////////////////////////////////////////////////////////////
// Disconnect from drone
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnDisconnect(wxCommandEvent& WXUNUSED(event))
{
	if(HasStatus(STATE_CONNECTEDTODRONE))
	{	
		if(!m_Drone.onGround())
		{
			// Not on ground, ask for confirmation !
			if(wxYES != wxMessageBox(GetText("DisconnectFlying"), GetText("Warning"), wxICON_EXCLAMATION | wxYES_NO))
			{
				return;
			}
		}
		Disconnect();
	}
}


/////////////////////////////////////////////////////////////////////////////
// Update states (called every 2 seconds)
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnTimerUpdate(wxTimerEvent& WXUNUSED(event))
{
	bool bPlayBeep	= false;
	bool bPlayBeeps	= false;

	// Increase count
	m_ucKeepAwake++;
	if(10 == m_ucKeepAwake)
	{
		// After 20 seconds, move to left
		wxPoint MousePos = wxGetMousePosition();
		MousePos.x--;
		m_ActionSimulator.MouseMove(MousePos);
	}
	else if(m_ucKeepAwake >= 20)
	{
		// After 20 more seconds, move to right
		wxPoint MousePos = wxGetMousePosition();
		MousePos.x++;
		m_ActionSimulator.MouseMove(MousePos);

		m_ucKeepAwake = 0;		
	}

	// Get current wifi signal
	/*if(m_Wifi.Update())
	{
		long lSignal = m_Wifi.GetSignalIntensity();
		if(lSignal < lWifiCritical)
		{
			bPlayBeeps = true;
		}
		else if(lSignal < lWifiDanger)
		{
			bPlayBeep = true;
		}
	}
	else
	{
		DoLog("Failed to update wifi signal !", MSG_ERROR);
	}*/
	
	// Play sound if authorized
	if( CConfig::GetSingleton()->HasSoundFlag() )
	{
		// Get current battery level
		int iBat = m_Drone.getBatteryPercentage();
		if(iBat < iBatCritical)
		{
			bPlayBeeps = true;
		}
		else if(iBat < iBatDanger)
		{
			bPlayBeep = true;
		}

		// Check if it is needed to play a sound
		if(bPlayBeeps && (NULL != m_pBeeps))
		{
			m_pBeeps->Play(wxSOUND_ASYNC);
		}
		else if(bPlayBeep && (NULL != m_pBeep))
		{
			m_pBeep->Play(wxSOUND_ASYNC);
		}
	}	
}


/////////////////////////////////////////////////////////////////////////////
// Dump the drone config to file
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnGetDroneConfig(wxCommandEvent& WXUNUSED(event))
{
    DoMessage(GetText("Dump not implemented yet"), MSG_INFO);
    // TODO: correct this
    /*
	if(1 == m_Drone.getConfig())
	{
		DoMessage(GetText("DumpSuccess"), MSG_INFO);
	}
	else
	{
		DoMessage(GetText("DumpFailed"), MSG_ERROR);
	}*/
}


/////////////////////////////////////////////////////////////////////////////
// Close the application
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnQuit(wxCommandEvent& WXUNUSED(event))
{
	// Note that this will send a close event -> See "OnCloseWindow()"
	Close();
}


/////////////////////////////////////////////////////////////////////////////
// Open the main configuration dialog
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnConfig(wxCommandEvent& WXUNUSED(event))
{
	// Save the current video codec
	eVideoCodec SavedCodec = CConfig::GetSingleton()->GetVideoCodec();
	// Save the current language
	wxString strlng = CConfig::GetSingleton()->GetLanguage();

	CConfigDialog ConfigDialog(this, HasStatus(STATE_CONNECTEDTODRONE));
	if(0 == ConfigDialog.ShowModal())
	{
		m_CSDrawing.Enter();

		// Language has changed, refresh the GUI
		if(CConfig::GetSingleton()->GetLanguage() != strlng)
		{
			RefreshGUIText();
		}

		// Maybe values has changed, set limits again
		SetLimits();

		// Set unit text
		SetUnitText();

		// Set new color and line size for the hud
		m_HudColor	= CConfig::GetSingleton()->GetHudColor();
		m_HudPen	= wxPen(m_HudColor, CConfig::GetSingleton()->GetHudLineSize());
		m_HudBrush	= wxBrush(m_HudColor, wxBRUSHSTYLE_TRANSPARENT);

		m_CSDrawing.Leave();
		
		if(HasStatus(STATE_CONNECTEDTODRONE))
		{
			// Send max altitude to the drone
			m_Drone.SetMaxAltitude(CConfig::GetSingleton()->GetAltitudeLimit());

			// If recording is active, don't send the new codec to the drone:
			// - Pc  : Opencv expect a fixed image size for recording 
			// - Usb : Sending another codec will disable recording
			if(!HasStatus(STATE_RECORDING))
			{
				// If the video codec has change, use the new one
				// (Takes 2 or 3 seconds, that's why it's called only if needed...)
				if(CConfig::GetSingleton()->GetVideoCodec() != SavedCodec)
				{
					m_Drone.SetVideoCodec(CConfig::GetSingleton()->GetVideoCodec());
				}
			}
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// Open the dialog to configure keyboard layout
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnKeyboard(wxCommandEvent& WXUNUSED(event))
{
	CKeyboardDialog KeyboardDialog(this, &m_Input);
	if(0 == KeyboardDialog.ShowModal())
	{
		if(!m_Input.SaveToFile(FILE_KEYBOARD))
		{
			DoLog("Failed to save the configuration for keyboard !", MSG_ERROR);
			DoMessage(GetText("FailedSaveKey"), MSG_ERROR);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// Open the dialog to configure joystick
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnJoystick(wxCommandEvent& WXUNUSED(event))
{
	if(m_Input.HasJoystick())
	{
		CJoystickDialog JoystickDialog(this, &m_Input);
		if(0 == JoystickDialog.ShowModal())
		{
			if(!m_Input.SaveToFile(FILE_JOYSTICK))
			{
				DoLog("Failed to save the configuration for joystick !", MSG_ERROR);
				DoMessage(GetText("FailedSaveJoy"), MSG_ERROR);
			}
		}
	}
	else
	{
		DoMessage(GetText("NoJoystick"), MSG_WARNING);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Open the dialog to set another Ip address
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnIpAddress(wxCommandEvent& WXUNUSED(event))
{
	CConfig* pConfig = CConfig::GetSingleton();

	wxString strIp = wxGetTextFromUser(GetText("SetIpMsg"), "IP", pConfig->GetIpAddress(), this);

	// The ip address did not change, or is empty (user have clicked on cancel)
	if( (pConfig->GetIpAddress() == strIp) || (strIp.IsEmpty()) )
	{
		return;
	}

	// Be optimistic, lets say it's a valid address :-)
	bool bIsValidIp = true;

	// Check if it is a valid ipv4 ip address
	if(!strIp.StartsWith('.') && !strIp.EndsWith('.') && 3 == strIp.Freq('.') )
	{
		wxStringTokenizer Tokenizer(strIp, '.');
		wxString strToken = "";
		long lTokValue = 0;

		while (Tokenizer.HasMoreTokens())
		{
			strToken = Tokenizer.GetNextToken();

			// Every token must be between 0 and 255
			if(!strToken.IsEmpty() && strToken.IsNumber())
			{
				lTokValue = ToLong(strToken);
				if( (0 > lTokValue) || (255 < lTokValue) )
				{
					bIsValidIp = false;
					break;
				}
			}
			else
			{
				bIsValidIp = false;
				break;
			}
		}
	}
	else
	{
		bIsValidIp = false;
	}	

	if(bIsValidIp)
	{
		pConfig->SetIpAddress(strIp);
	}
	else
	{
		pConfig->SetIpAddress(ARDRONE_DEFAULT_ADDR);
		DoMessage(GetText("IpInvalid"), MSG_ERROR);
	}

	DoLog(wxString::Format("Ip address has been changed to %s", pConfig->GetIpAddress()));

	// Save the ip address to the config file
	pConfig->SaveConfig();
}


/////////////////////////////////////////////////////////////////////////////
// Open the dialog to configure joystick
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnAbout(wxCommandEvent& WXUNUSED(event))
{
	CAboutDialog AboutDialog(this);
	AboutDialog.ShowModal();
}


/////////////////////////////////////////////////////////////////////////////
// Open the dialog to configure joystick
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnEmergency(wxCommandEvent& WXUNUSED(event))
{
	m_Input.SetFlag(KEY_EMERGENCY);
}


/////////////////////////////////////////////////////////////////////////////
// Open the dialog to configure joystick
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnTrim(wxCommandEvent& WXUNUSED(event))
{
	// Trim key does not exist, but we use the same mechanism
	m_Input.SetFlag(KEY_TRIM);
}


/////////////////////////////////////////////////////////////////////////////
// Open the dialog to configure joystick
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnLandTakeOff(wxCommandEvent& WXUNUSED(event))
{
	m_Input.SetFlag(KEY_TAKEOFF);
}


/////////////////////////////////////////////////////////////////////////////
// Start or stop recording
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnRecord(wxCommandEvent& WXUNUSED(event))
{
	m_Input.SetFlag(KEY_RECORD);
}


/////////////////////////////////////////////////////////////////////////////
// Take a screenshot
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnScreenshot(wxCommandEvent& WXUNUSED(event))
{
	m_Input.SetFlag(KEY_SCREENSHOT);
}


/////////////////////////////////////////////////////////////////////////////
// Switch between fullscreen and windowed mode
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnFullscreen(wxCommandEvent& WXUNUSED(event))
{
	m_Input.SetFlag(KEY_FULLSCREEN);
}


/////////////////////////////////////////////////////////////////////////////
// Switch between the different onboard cameras
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnCamera(wxCommandEvent& WXUNUSED(event))
{
	m_Input.SetFlag(KEY_CAMERA);
}


/////////////////////////////////////////////////////////////////////////////
// Enable/Disable return to home function
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnHome(wxCommandEvent& WXUNUSED(event))
{
	m_Input.SetFlag(KEY_RETURNHOME);
}


/////////////////////////////////////////////////////////////////////////////
// Reset home position to current position
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnHomeReset(wxCommandEvent& WXUNUSED(event))
{
	double	dLat		= 0.0f;
	double	dLon		= 0.0f;
	bool	bHasGps		= m_Drone.HasGps();

	if(bHasGps)
	{
		m_Drone.GetGpsPosition(dLat, dLon);
	}

	m_AutoPilot.ResetAutoPilot(m_Watch.TimeInMicro(), bHasGps, dLat, dLon);
}


#ifdef wxHAS_POWER_EVENTS

/////////////////////////////////////////////////////////////////////////////
// Catched a sleep/hibernate event
/////////////////////////////////////////////////////////////////////////////
void CDroneController::OnSleep(wxPowerEvent& event)
{
	DoLog("Canceling power saving mode", MSG_INFO);
	event.Veto();	
}

#endif

/////////////////////////////////////////////////////////////////////////////
// Set limits from config to the helper object
/////////////////////////////////////////////////////////////////////////////
void CDroneController::SetLimits()
{
	// Set speed limits to the input
	double dSpeed = ((double)CConfig::GetSingleton()->GetSpeedLimit() / 100.0f);
	double dAcceleration = ((double)CConfig::GetSingleton()->GetAccelerationLimit() / 100.0f);
	m_Input.SetLimits(dSpeed, dAcceleration);

	// Set altitude limit to the helper object
	m_dMaxAltitude = (double)CConfig::GetSingleton()->GetAltitudeLimit();
}


/////////////////////////////////////////////////////////////////////////////
// Compute new positions of the graphical items after a resize event
/////////////////////////////////////////////////////////////////////////////
void CDroneController::ComputePositions(int iPanelWidth, int iPanelHeight)
{
	wxCriticalSectionLocker Lock(m_CSDrawing);	

	m_iPanelWidth = iPanelWidth;
	m_iPanelHeight = iPanelHeight;

	// Screen middle
	m_iMiddleX = m_iPanelWidth / 2;
	m_iMiddleY = m_iPanelHeight / 2;

	// Radius to use for the central circle
	if(m_iPanelWidth >= m_iPanelHeight)
	{
		m_iRadius = m_iPanelHeight/3;
	}
	else
	{
		m_iRadius = m_iPanelWidth/3;
	}

	// Part of radius
	m_dRadiusOver2	= ((double)m_iRadius)/2.0f;
	m_dRadiusOver20	= ((double)m_iRadius)/20.0f;

	// Position of the info text
	m_SpeedInfoRect			= wxRect(m_iMiddleX - m_iRadius - 80, m_iMiddleY, 70, 25);
	m_SpeedInfoTextRect		= wxRect(m_iMiddleX - m_iRadius - 80, m_iMiddleY - 20, 70, 25);
	m_AltInfoRect			= wxRect(m_iMiddleX + m_iRadius + 10, m_iMiddleY, 70, 25);
	m_AltInfoTextRect		= wxRect(m_iMiddleX + m_iRadius + 10, m_iMiddleY - 20, 70, 25);
	m_DirInfoRect			= wxRect(m_iMiddleX - 30, m_iMiddleY - m_iRadius - 40, 70, 25);
	m_ErrorRect				= wxRect(m_iMiddleX - m_iRadius, m_iMiddleY-m_iRadius, m_iRadius*2, 50);
	m_DebugRect				= wxRect(m_iMiddleX - m_iRadius, m_iPanelHeight-20, m_iRadius*2, 20);
}


/////////////////////////////////////////////////////////////////////////////
// The code executed by the thread (main loop)
/////////////////////////////////////////////////////////////////////////////
wxThread::ExitCode CDroneController::Entry()
{
	while(!m_thread->TestDestroy())
	{
		// Update the key status
		m_Input.Update(m_Watch.Time());

		// Only if drone is connected
		if(HasStatus(STATE_CONNECTEDTODRONE))
		{
			// Check if an update is needed
			if(m_Input.IsUpdateNeeded())
			{
				// Emergency has been pressed
				if(m_Input.HasFlag(KEY_EMERGENCY))
				{
					m_Drone.emergency();
					m_Input.ResetFlag(KEY_EMERGENCY);
					DoLog("Emergency mode enabled !");
				}

				// Toggle cameras
				if(m_Input.HasFlag(KEY_CAMERA))
				{
					m_Drone.setCamera(++m_iCameraMode%4);
					m_Input.ResetFlag(KEY_CAMERA);
				}

				// Enable/disable return to home
				if(m_Input.HasFlag(KEY_RETURNHOME))
				{
					if(HasStatus(STATE_RETURNHOMEACTIVE))
					{
						ResetStatus(STATE_RETURNHOMEACTIVE);
						DoLog("Return to home disabled");
					}
					else
					{
						SetStatus(STATE_RETURNHOMEACTIVE);
						DoLog("Return to home enabled");
					}
					m_Input.ResetFlag(KEY_RETURNHOME);
				}

				// Land and take off
				if(m_Input.HasFlag(KEY_TAKEOFF))
				{
					if(m_Drone.onGround())
					{
						DoLog("The drone will take off");

						double	dLat		= 0.0f;
						double	dLon		= 0.0f;
						bool	bHasGps		= m_Drone.HasGps();

						if(bHasGps)
						{
							m_Drone.GetGpsPosition(dLat, dLon);								
						}

						// This is our new home, sweet home..
						m_AutoPilot.ResetAutoPilot(m_Watch.TimeInMicro(), bHasGps, dLat, dLon);

						m_Drone.takeoff();

						// Start counting flying time
						m_WatchFlyingTime.Resume();
						m_bFlyingTimeWachActive = true;
					}
					else
					{
						DoLog("The drone will land");

						// Stop counting flying time
						m_WatchFlyingTime.Pause();
						m_bFlyingTimeWachActive = false;

						m_Drone.landing();
					}
					m_Input.ResetFlag(KEY_TAKEOFF);
				}				
					
				if(m_Input.HasFlag(KEY_TRIM))
				{
					if(m_Drone.onGround())
					{
						DoLog("Trim will be started");
						// If drone is on ground, do a trim
						m_Drone.Trim();
					}
					else
					{
						DoLog("Calibration will be started");
						// If drone is flying, start calibration
						m_Drone.Calibrate();
					}
					m_Input.ResetFlag(KEY_TRIM);
				}

				// View has been toggled between windowed and fullscreen
				if(m_Input.HasFlag(KEY_FULLSCREEN))
				{
					// Switch status
					if(HasStatus(STATE_FULLSCREEN))
					{
						/*if(CConfig::GetSingleton()->UseLowRes())
						{
							m_ScreenManager.ResetResolution();
						}*/
						ResetStatus(STATE_FULLSCREEN);
					}
					else
					{
						/*if(CConfig::GetSingleton()->UseLowRes())
						{
							m_ScreenManager.SetLowResolution();
						}*/
						SetStatus(STATE_FULLSCREEN);
					}
					
					// Apply changes and reset key flag
					this->ShowFullScreen(HasStatus(STATE_FULLSCREEN));
					m_Input.ResetFlag(KEY_FULLSCREEN);
				}

				// Toggle recording
				if(m_Input.HasFlag(KEY_RECORD))
				{
					// Note: Recording flags are already managed in the called functions
					if(HasStatus(STATE_RECORDING))
					{
						StopRecord();
					}
					else
					{
						StartRecord();
					}
					m_Input.ResetFlag(KEY_RECORD);
				}
			}
			
			if( m_Drone.onGround() )
			{
				// Drone on ground, check if FlyingTime watch still running (unexpected "landing"...)
				if(m_bFlyingTimeWachActive)
				{
					DoLog("Flying timer active while drone on ground, timer will be stopped", MSG_WARNING);
					m_WatchFlyingTime.Pause();
					m_bFlyingTimeWachActive = false;
				}
			}
			else // Move the drone if he is flying
			{				
				bool bIsAutopilotOn = HasStatus(STATE_RETURNHOMEACTIVE);
				
				double dVelX		= 0.0f;
				double dVelY		= 0.0f;
				double dLat			= 0.0f;
				double dLon			= 0.0f;
				double dAlt			= m_Drone.getAltitude();
				
				m_Drone.GetHorizontalVelocity(dVelX, dVelY);

				// Update the gps informations if available
				if(m_AutoPilot.HasGps())
				{
					m_Drone.GetGpsPosition(dLat, dLon);

					m_AutoPilot.UpdateGps(dLat, dLon);
				}

				// Update the autopilot with the new drone info
				m_AutoPilot.Update(m_Watch.TimeInMicro(), m_Drone.getYawDeg(), dVelX, dVelY, dAlt, bIsAutopilotOn);

				if(bIsAutopilotOn)
				{
					double dRotation	= 0.0f;
					double dSpeed		= 0.0f;
					double dAltitude	= 0.0f;
					
					// Return to home is active, compute the way to home
					m_AutoPilot.ComputeWayToHome(dRotation, dSpeed, dAltitude);

					// Transmit values to drone
					m_Drone.CustomMove(0.0f, (float)dSpeed, (float)dAltitude, (float)dRotation);
				}
				else
				{
					double dAltVector = 0.0f;

					// Check altitude limit
					if(dAlt < m_dMaxAltitude)
					{
						 dAltVector = m_Input.GetDirectionValue(IDX_ALTITUDE);
					}
					else
					{
						// Altitude has reach, or is over max altitude, bring the drone down
						double dAltDiff = dAlt - m_dMaxAltitude;
						if(dAltDiff > 10.0f)
						{
							dAltVector = -0.75f;	// Drone flying away ??
						}
						else if(dAltDiff > 5.0f)
						{
							dAltVector = -0.5f;
						}
						else
						{
							dAltVector = -0.20f;
						}
					}

					// Move the drone
					m_Drone.CustomMove(m_Input.GetDirectionValue(IDX_SIDE), m_Input.GetDirectionValue(IDX_FORWARD), dAltVector, m_Input.GetDirectionValue(IDX_TURN));
				}
			}
		}

		// Draw the new image
		DoRender();

		// Let the system responsive
		msleep(1);
	}

	DoLog("Main frame thread stopped");

	return NULL;
}


/////////////////////////////////////////////////////////////////////////////
// Draw the main panel with camera capture
/////////////////////////////////////////////////////////////////////////////
bool CDroneController::DoRender()
{	
	// Client device context -> Draw on the panel
	wxClientDC DC(m_pPanel);
	// Create a buffered DC, to avoid flickering on drawing
	wxBufferedDC BufferedDC(&DC, m_BufferBitmap);

	if(!BufferedDC.IsOk())
	{
		DoLog("Device context is not valid !", MSG_ERROR);
		return false;
	}

/*
	BufferedDC.Clear();
	BufferedDC.SetFont(FontBold12);
	BufferedDC.SetPen(m_HudPen);
	BufferedDC.SetBrush(m_HudBrush);
	DrawStatus(BufferedDC);
	DrawHUD2(BufferedDC);
	DrawDebugInfo(BufferedDC);

	BufferedDC.SetFont(FontBold18);
	BufferedDC.SetTextForeground(ColorRed);
	BufferedDC.DrawLabel(m_Drone.GetErrorText(), m_ErrorRect, wxALIGN_CENTRE);
	return true;
*/

	if(HasStatus(STATE_CONNECTEDTODRONE))
	{
		// If we have no image, display only navdata info
		bool bHasImage = true;

		// Get an image
		IplImage* pImg = m_Drone.getImage();
		if( (NULL == pImg) || (0 >= pImg->width) || (0 >= pImg->height) )
		{
			// Could not access to an image
			bHasImage = false;
		}

		if(bHasImage)
		{
			// Only needed for recording on pc
			if(HasStatus(STATE_RECORDINGONPC))
			{
				// We want to record a frame every 40 ms, so check if this time is elapsed
				wxLongLong llCurrentTime = m_Watch.TimeInMicro();

				if( llCurrentTime >  m_llNextFrameTime)
				{
					// Target frame time reached, compute next frame time
					wxLongLong llDiffTime = llCurrentTime-m_llNextFrameTime;

					// In case of big difference don't calculate the target time, as it would be a time in the passt
					if(llDiffTime > 39000)
					{
						// Add 20 ms to current time
						m_llNextFrameTime = llCurrentTime + 20000;
					}
					else
					{
						// Next frame time = current time + expected 40 ms - time over expected time (difference time)
						m_llNextFrameTime = llCurrentTime + 40000 - llDiffTime;
					}

					m_CSVideo.Enter();

					// Check if the writer still exit, as it may have been destroyed while waiting for critical section
					if(NULL != m_pVideoWriter)
					{
						cvWriteFrame(m_pVideoWriter, pImg);
					}
			
					m_CSVideo.Leave();
				}
			}

			// Convert colors from BGR (opencv) to RGB (wxwidgets)
			cvCvtColor((CvArr*)pImg, (CvArr*)pImg, CV_BGR2RGB);	

			// Set new data to the wxImage
			m_Image.SetData((unsigned char*)pImg->imageData, pImg->width, pImg->height, true);

			if(!m_Image.IsOk())
			{
				DoLog("Could not transform drone image to wxImage !", MSG_ERROR);
				return false;
			}

			// Screenshot requested, save current image to file as .png
			if(m_Input.HasFlag(KEY_SCREENSHOT))
			{
				m_Input.ResetFlag(KEY_SCREENSHOT);

                time_t rawtime;
                struct tm * timeinfo;
				time ( &rawtime );
                timeinfo = localtime ( &rawtime );
				if(!m_Image.SaveFile(wxString::Format("Media/Pictures/Pic_%s.png", asctime (timeinfo)), wxBITMAP_TYPE_PNG))
				{
					DoLog("Failed to take screenshot", MSG_ERROR);
				}
			}

			// Set the bitmap to the panel
			if( (m_iPanelWidth != pImg->width) || (m_iPanelHeight != pImg->height) )
			{
				BufferedDC.DrawBitmap(wxBitmap(m_Image.Scale(m_iPanelWidth, m_iPanelHeight)), 0, 0);
			}
			else
			{
				BufferedDC.DrawBitmap(wxBitmap(m_Image), 0, 0);
			}
		}
		else
		{
			// Clear the DC before status and other informations are displayed
			BufferedDC.Clear();
		}

		// In all cases, draw status informations
		DrawStatus(BufferedDC);

		if(CConfig::GetSingleton()->GetHudStyle() == 1)
		{
			DrawHUD1(BufferedDC);
		}
		else
		{
			DrawHUD2(BufferedDC);
		}
		DrawDebugInfo(BufferedDC);

		// Check if the drone has an error status
		if(m_Drone.HasError())
		{
			// Set big font, red color and display the message
			BufferedDC.SetFont(FontBold18);
			BufferedDC.SetTextForeground(ColorRed);
			BufferedDC.DrawLabel(m_Drone.GetErrorText(), m_ErrorRect, wxALIGN_CENTRE);
		}
	}
	else
	{
		BufferedDC.Clear();
		BufferedDC.SetFont(FontBold12);
		BufferedDC.SetTextForeground(ColorBlack);
		
		BufferedDC.DrawLabel(m_strNoConnection, wxRect(0, 0, m_iPanelWidth, m_iPanelHeight), wxALIGN_CENTRE);

		// Draw the debug informations
		DrawDebugInfo(BufferedDC);
	}	

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Draw status informations (battery, wifi...)
/////////////////////////////////////////////////////////////////////////////
void CDroneController::DrawStatus(wxDC& dc)
{
	wxCriticalSectionLocker Lock(m_CSDrawing);

	// Init font, pen and brush
	dc.SetFont(FontBold12);
	dc.SetPen(m_HudPen);
	dc.SetBrush(m_HudBrush);

	// Battery status (sounds handled in timer)
	int iBat = m_Drone.getBatteryPercentage();
	wxBitmap* pBat = NULL;

	if(iBat > iBatWarning)
	{
		dc.SetTextForeground(ColorGreen);
		pBat = &m_BatteryGreenBitmap;
	}
	else if(iBat < iBatCritical)
	{
		dc.SetTextForeground(ColorRed);
		pBat = &m_BatteryRedBitmap;
	}
	else if(iBat < iBatDanger)
	{
		dc.SetTextForeground(ColorOrange);
		pBat = &m_BatteryOrangeBitmap;
	}
	else
	{
		dc.SetTextForeground(ColorYellow);
		pBat = &m_BatteryYellowBitmap;
	}
	dc.DrawLabel(wxString::Format("%d %%", iBat), *pBat, m_BatteryRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);

    // TODO: wifi signal
	// Wifi signal quality (sounds handled in Timer update)
	/*
	long lSignal = m_Wifi.GetSignalIntensity();
	wxBitmap* pWifi = NULL;
	if(lSignal > lWifiWarning)
	{
		dc.SetTextForeground(ColorGreen);
		pWifi = &m_WifiGreenBitmap;
	}
	else if(lSignal < lWifiCritical)
	{
		dc.SetTextForeground(ColorRed);
		pWifi = &m_WifiRedBitmap;
	}
	else if(lSignal < lWifiDanger)
	{
		dc.SetTextForeground(ColorOrange);
		pWifi = &m_WifiOrangeBitmap;
	}
	else
	{
		dc.SetTextForeground(ColorYellow);
		pWifi = &m_WifiYellowBitmap;
	}	
	dc.DrawLabel(wxString::Format("%d %%", lSignal), *pWifi, m_WifiRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
	*/
	// If recording draw the Record info text
	if(HasStatus(STATE_RECORDING))
	{
		// Check if recording to pc or to the usb stick into the drone
		dc.SetTextForeground(ColorRed);
		if(HasStatus(STATE_RECORDINGONPC) && HasStatus(STATE_RECORDINGONUSB))
		{
			dc.DrawLabel("USB+PC", m_RecordBitmap, m_RecordRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
		else if(HasStatus(STATE_RECORDINGONPC))
		{
			dc.DrawLabel("PC", m_RecordBitmap, m_RecordRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
		else
		{
			dc.DrawLabel("USB", m_RecordBitmap, m_RecordRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
	}

	if(HasStatus(STATE_RETURNHOMEACTIVE))
	{						
		if(m_AutoPilot.HasGps())
		{
			dc.SetTextForeground(ColorGreen);
			dc.DrawLabel("ON [GPS])", m_HomeBitmap, m_HomeRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
		else
		{
			dc.SetTextForeground(ColorOrange);
			dc.DrawLabel("ON", m_HomeBitmap, m_HomeRect, wxALIGN_LEFT | wxALIGN_CENTER_VERTICAL);
		}
	}
}


/////////////////////////////////////////////////////////////////////////////
// Draw the basic version of the HUD, and all screen informations
/////////////////////////////////////////////////////////////////////////////
void CDroneController::DrawHUD1(wxDC& dc)
{
	// Get altitude in meters, and speed in meters/second
	double dAltitude = m_Drone.GetHorizontalVelocity();
	double dSpeed = m_Drone.getAltitude();

	// If needed, convert to feet
	if(CConfig::GetSingleton()->HasUSUnit())
	{
		dAltitude = dAltitude * dMetertoFeet;
		dSpeed = dSpeed * dMetertoFeet;
	}
	
	wxCriticalSectionLocker Lock(m_CSDrawing);

	// Set color for other elements
	dc.SetTextForeground(m_HudColor);

	// Draw central panel
	dc.DrawCircle(m_iMiddleX, m_iMiddleY, m_iRadius);

	// Draw speed info
	dc.DrawLabel(wxString::Format("Spd."), m_SpeedInfoTextRect, wxALIGN_CENTRE);
	dc.DrawRectangle(m_SpeedInfoRect);
	dc.DrawLabel(wxString::Format(m_strSpeed, dSpeed), m_SpeedInfoRect, wxALIGN_CENTRE);

	// Draw altitude info
	dc.DrawLabel(wxString::Format("Alt."), m_AltInfoTextRect, wxALIGN_CENTRE);
	dc.DrawRectangle(m_AltInfoRect);
	dc.DrawLabel(wxString::Format(m_strAltitude, dAltitude), m_AltInfoRect, wxALIGN_CENTRE);

	// Draw direction info
	dc.DrawRectangle(m_DirInfoRect);
	dc.DrawLabel(wxString::Format("%03.0f", m_Drone.getYawDeg()), m_DirInfoRect, wxALIGN_CENTRE);

	// Draw the artificial horizon
	double	dPitch	= m_Drone.getPitchDeg();
	double	dRoll	= m_Drone.getRollDeg();

	// Calculate vectors
	int iVectX = (int)(cos(-dRoll*dPIover180) * ((double)(m_iRadius/2)));
	int iVectY = (int)(sin(-dRoll*dPIover180) * ((double)(m_iRadius/2)));

	// Starting point after pitch has been added
	int iStartY = m_iMiddleY + (int)dPitch;

	// Draw the horizon
	dc.DrawLine(m_iMiddleX - iVectX, iStartY - iVectY, m_iMiddleX + iVectX, iStartY + iVectY);
}


/////////////////////////////////////////////////////////////////////////////
// Draw the advanced version of the HUD, and all screen informations
/////////////////////////////////////////////////////////////////////////////
void CDroneController::DrawHUD2(wxDC& dc)
{
	// Get altitude in meters, and speed in meters/second
	double dSpeed = m_Drone.GetHorizontalVelocity();
	double dAltitude = m_Drone.getAltitude();

	// If needed, convert to feet
	if(CConfig::GetSingleton()->HasUSUnit())
	{
		dAltitude = dAltitude * dMetertoFeet;
		dSpeed = dSpeed * dMetertoFeet;
	}
	
	wxCriticalSectionLocker Lock(m_CSDrawing);

	// Set color for other elements
	dc.SetTextForeground(m_HudColor);

	// Draw central cross
	dc.DrawLine(m_iMiddleX-30, m_iMiddleY, m_iMiddleX+30, m_iMiddleY);
	dc.DrawLine(m_iMiddleX, m_iMiddleY-10, m_iMiddleX, m_iMiddleY+10);

	// Draw vertical line on left and right
	dc.DrawLine(m_iMiddleX-m_iRadius, m_iMiddleY-m_iRadius, m_iMiddleX-m_iRadius, m_iMiddleY+m_iRadius);
	dc.DrawLine(m_iMiddleX+m_iRadius, m_iMiddleY-m_iRadius, m_iMiddleX+m_iRadius, m_iMiddleY+m_iRadius);

	// Draw speed info
	dc.DrawRectangle(m_SpeedInfoRect);
	dc.DrawLabel(wxString::Format(m_strSpeed, dSpeed), m_SpeedInfoRect, wxALIGN_CENTRE);

	// Draw altitude info
	dc.DrawRectangle(m_AltInfoRect);
	dc.DrawLabel(wxString::Format(m_strAltitude, dAltitude), m_AltInfoRect, wxALIGN_CENTRE);

	// Draw direction info
	dc.DrawRectangle(m_DirInfoRect);
	dc.DrawLabel(wxString::Format("%03.0f", m_Drone.getYawDeg()), m_DirInfoRect, wxALIGN_CENTRE);
		
	int		iPitch	= (int)m_Drone.getPitchDeg();
	double	dRoll	= m_Drone.getRollDeg();

	// Calculate vectors for angle
	int iVectX = (int)(cos(-dRoll*dPIover180)*(m_dRadiusOver2));
	int iVectY = (int)(sin(-dRoll*dPIover180)*(m_dRadiusOver2));

	// Find the difference to the next multiple of 10
	int iPitchDiff = (iPitch%10);

	// Get the vertical start position
	int iStartY = m_iMiddleY + (iPitchDiff*(m_dRadiusOver20));
	
	int iX = 0;
	int iY = 0;

	// Draw 5 lines with level info
	for(int i=-2; i<3; i++)
	{
		iX = m_iMiddleX-(iVectY*i);
		iY = iStartY+(iVectX*i);

		dc.DrawLine( iX - iVectX, iY - iVectY, iX + iVectX, iY + iVectY );
		dc.DrawRotatedText(wxString::Format("%d", iPitch-iPitchDiff-(i*10)), iX - iVectX, iY - iVectY, dRoll);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Draw debug informations
/////////////////////////////////////////////////////////////////////////////
void CDroneController::DrawDebugInfo(wxDC& dc)
{
	wxCriticalSectionLocker Lock(m_CSDrawing);

	dc.SetFont(FontNormal10);

	// Get the estimated distance between the drone and starting point
	double Distance = m_AutoPilot.GetDistance();

	if(CConfig::GetSingleton()->HasUSUnit())
	{
		Distance = Distance * dMetertoFeet;
	}

	// Get total flying time for the current battery
	long lFlightTimeInSeconds = m_WatchFlyingTime.Time()/1000;
	long lSec = lFlightTimeInSeconds%60;
	long lMin = (lFlightTimeInSeconds-lSec)/60;

	// Debug information : Distance, minutes flying, seconds flying, forward vector, side vector, turning vector, altitude vector
	wxString str = wxString::Format(m_strDebug, Distance, lMin, lSec, m_Input.GetDirectionValue(IDX_FORWARD), m_Input.GetDirectionValue(IDX_SIDE), m_Input.GetDirectionValue(IDX_TURN), m_Input.GetDirectionValue(IDX_ALTITUDE));
	dc.DrawLabel(str, m_DebugRect, wxALIGN_CENTRE);

	// If global debug flag is active display all informations
	if(g_bDebug)
	{
		double dMoveRotation	= 0.0f;
		double dMoveSpeed		= 0.0f;
		double dMoveAltitude	= 0.0f;

		m_AutoPilot.ComputeWayToHome(dMoveRotation, dMoveSpeed, dMoveAltitude);
	
		int PosX = m_iPanelWidth - 160;
		int PosY = m_iPanelHeight - 250;

		dc.DrawText(wxString::Format("DroneAng  %.2f", m_Drone.getYawDeg()), PosX, PosY); PosY+=20;
		dc.DrawText(wxString::Format("GpsAngle  %.2f", m_Drone.GetGpsAngle()), PosX, PosY); PosY+=20;
		dc.DrawText(wxString::Format("Ang2Home  %.2f", m_AutoPilot.GetProperty(DBG_HOMEANGLE)), PosX, PosY); PosY+=20;
		dc.DrawText(wxString::Format("Distance  %.2f", m_AutoPilot.GetDistance()), PosX, PosY); PosY+=20;
		dc.DrawText(wxString::Format("MaxAlt    %.2f", m_AutoPilot.GetProperty(DBG_ALTITUDEMAX)), PosX, PosY); PosY+=20;
		dc.DrawText(wxString::Format("PositionX %.2f", m_AutoPilot.GetProperty(DBG_POSITIONX)), PosX, PosY); PosY+=20;
		dc.DrawText(wxString::Format("PositionY %.2f", m_AutoPilot.GetProperty(DBG_POSITIONY)), PosX, PosY); PosY+=20;
		dc.DrawText(wxString::Format("Latitude  %.2f", m_AutoPilot.GetProperty(DBG_CURRENTLATITUDE)), PosX, PosY); PosY+=20;
		dc.DrawText(wxString::Format("Longitude %.2f", m_AutoPilot.GetProperty(DBG_CURRENTLONGITUDE)), PosX, PosY); PosY+=20;
	}
	
	dc.DrawText(wxString::Format("FPS: %d", m_Input.GetFPS()), m_iPanelWidth-70, 10);
}


/////////////////////////////////////////////////////////////////////////////
// Set speed an altitude text depending on current units (meters or feets)
/////////////////////////////////////////////////////////////////////////////
void CDroneController::SetUnitText()
{
	if(CConfig::GetSingleton()->HasUSUnit())
	{
		m_strAltitude = "%.1f ft";
		m_strSpeed = "%.1f ft/s";
		m_strDebug = "Distance: %.0f ft - Flying time: %dm %ds - Pitch: %.2f - Roll: %.2f - Yaw: %.2f - Altitude: %.2f";
	}
	else
	{
		m_strAltitude = "%.1f m";
		m_strSpeed = "%.1f m/s";
		m_strDebug = "Distance: %.0f m - Flying time: %dm %ds - Pitch: %.2f - Roll: %.2f - Yaw: %.2f - Altitude: %.2f";
	}
}


/////////////////////////////////////////////////////////////////////////////
// Read a status
/////////////////////////////////////////////////////////////////////////////
bool CDroneController::HasStatus(eAppState AppStatus)
{
	return ((m_usAppState & AppStatus) == AppStatus);
}


/////////////////////////////////////////////////////////////////////////////
// Set a status
/////////////////////////////////////////////////////////////////////////////
void CDroneController::SetStatus(eAppState AppStatus)
{
	m_usAppState |= AppStatus;
}


/////////////////////////////////////////////////////////////////////////////
// Reset a status
/////////////////////////////////////////////////////////////////////////////
void CDroneController::ResetStatus(eAppState AppStatus)
{
	m_usAppState &= ~AppStatus;
}


/////////////////////////////////////////////////////////////////////////////
// Refresh the text after a language change
/////////////////////////////////////////////////////////////////////////////
bool CDroneController::RefreshGUIText()
{
	// Load the new language
	if(!CConfig::GetSingleton()->LoadLanguage())
	{
		return false;
	}

	// Refresh input (only usefull for French)
	m_Input.RefreshLayout();

	// Refresh menu title
	m_pMenu->SetMenuLabel(0, GetText("Main"));
	m_pMenu->SetMenuLabel(1, GetText("Config"));	

	// Refresh menu text
	m_pMenu->SetLabel(MENU_CFG, GetText("MainGetCfg"));
	m_pMenu->SetLabel(MENU_QUIT, GetText("MainExit"));
	m_pMenu->SetLabel(MENU_CONFIG, GetText("ConfigApp"));
	m_pMenu->SetLabel(MENU_KEYBOARD, GetText("ConfigKey"));
	m_pMenu->SetLabel(MENU_JOYSTICK, GetText("ConfigJoy"));
	m_pMenu->SetLabel(MENU_IPADDRESS, GetText("ConfigIp"));
	m_pMenu->SetLabel(MENU_ABOUT, GetText("About"));
	
	// Refresh toolbar text
	wxToolBarToolBase* pTool = NULL;
	
	pTool = m_pToolbar->FindById(MENU_CONNECT);
	if(NULL != pTool) pTool->SetShortHelp(GetText("Connect"));

	pTool = m_pToolbar->FindById(MENU_DISCONNECT);
	if(NULL != pTool) pTool->SetShortHelp(GetText("Disconnect"));

	pTool = m_pToolbar->FindById(MENU_EMERGENCY);
	if(NULL != pTool) pTool->SetShortHelp(GetText("Emergency"));

	pTool = m_pToolbar->FindById(MENU_TRIM);
	if(NULL != pTool) pTool->SetShortHelp(GetText("Trim"));

	pTool = m_pToolbar->FindById(MENU_LANDTAKEOFF);
	if(NULL != pTool) pTool->SetShortHelp(GetText("LandTakeOff"));

	pTool = m_pToolbar->FindById(MENU_FULLSCREEN);
	if(NULL != pTool) pTool->SetShortHelp(GetText("Fullscreen"));

	pTool = m_pToolbar->FindById(MENU_CAMERA);
	if(NULL != pTool) pTool->SetShortHelp(GetText("Camera"));

	pTool = m_pToolbar->FindById(MENU_RECORD);
	if(NULL != pTool) pTool->SetShortHelp(GetText("Record"));

	pTool = m_pToolbar->FindById(MENU_SCREENSHOT);
	if(NULL != pTool) pTool->SetShortHelp(GetText("Screenshot"));

	pTool = m_pToolbar->FindById(MENU_HOME);
	if(NULL != pTool) pTool->SetShortHelp(GetText("Home"));

	pTool = m_pToolbar->FindById(MENU_HOMERESET);
	if(NULL != pTool) pTool->SetShortHelp(GetText("HomeReset"));
	
	// Refresh status bar if the drone is connected
	if(HasStatus(STATE_CONNECTEDTODRONE))
	{
		//m_pStatusBar->SetStatusText(GetText("ConnectedTo") + wxString(m_Wifi.GetNetworkName()), 1);
		m_pStatusBar->SetStatusText(GetText("DroneVersion") + wxString::Format("%d", m_Drone.getVersion()), 2);
	}

	// Text to display as long there is no connection to the drone
	m_strNoConnection	= GetText("NotConnected");

	return true;
}


/////////////////////////////////////////////////////////////////////////////
// Start recording on PC
/////////////////////////////////////////////////////////////////////////////
void CDroneController::StartPCRecording()
{
	DoLog("Start recording on pc");

	// Start recording on local pc
	char filename[256];

	time_t rawtime;
    struct tm * timeinfo;
    time ( &rawtime );
    timeinfo = localtime ( &rawtime );

	sprintf(filename, "Media/Movies/Mov_%s.avi", asctime (timeinfo));

	// Init the next frame target time (current time + 40 ms)
	m_llNextFrameTime = m_Watch.TimeInMicro() + 40000;

	// Get the image size send by the drone
	IplImage* pImg = m_Drone.getImage();
	if(NULL != pImg)
	{
		// Record at 25 fps (an image every 40 ms)
		double dFps = 25.0f;

		// Try different codecs for video writer
		DoLog("Try to load H264 codec");				
		m_pVideoWriter = cvCreateVideoWriter(filename, CV_FOURCC('H','2','6','4'), dFps, cvGetSize(pImg));				
		if(NULL == m_pVideoWriter)
		{
			DoLog("Failed to load H264 codec, try to load Xvid codec");
			m_pVideoWriter = cvCreateVideoWriter(filename, CV_FOURCC('X','V','I','D'), dFps, cvGetSize(pImg));		
		}
		if(NULL == m_pVideoWriter)
		{					
			DoLog("Failed to load Xvid codec, try to load MP4.2 codec");
			m_pVideoWriter = cvCreateVideoWriter(filename, CV_FOURCC('M','P','4','2'), dFps, cvGetSize(pImg));	
		}				
		if(NULL == m_pVideoWriter)
		{
			DoLog("Failed to load MP4.2 codec, try to load divx 5 codec");
			m_pVideoWriter = cvCreateVideoWriter(filename, CV_FOURCC('D','X','5','0'), dFps, cvGetSize(pImg));	
		}
		if(NULL == m_pVideoWriter)
		{
			DoLog("Failed to loadDivx 5 codec, try to load DivX codec");					
			m_pVideoWriter = cvCreateVideoWriter(filename, CV_FOURCC('D','I','V','X'), dFps, cvGetSize(pImg));
		}
		if(NULL == m_pVideoWriter)
		{
			DoLog("Failed to load DivX codec, try to load H263 codec");
			m_pVideoWriter = cvCreateVideoWriter(filename, CV_FOURCC('U','2','6','3'), dFps, cvGetSize(pImg));	
		}
		if(NULL == m_pVideoWriter)
		{
			DoLog("Failed to load H263 codec, try to load RGB avi file mode (Hard disk consuming)");
			m_pVideoWriter = cvCreateVideoWriter(filename, CV_FOURCC('D','I','B',' '), dFps, cvGetSize(pImg));	
		}

		if(NULL == m_pVideoWriter)
		{
			DoLog("Could not enable codec, recording on pc aborted !", MSG_ERROR);
		}
		else
		{
			SetStatus(STATE_RECORDINGONPC);
		}
	}
	else
	{
		DoLog("Failed to get image from drone, recording on pc aborted !", MSG_ERROR);
	}
}


/////////////////////////////////////////////////////////////////////////////
// Stop recording on PC
/////////////////////////////////////////////////////////////////////////////
void CDroneController::StopPCRecording()
{
	DoLog("Stop recording on pc");

	// Save the video
	m_CSVideo.Enter();
	cvReleaseVideoWriter(&m_pVideoWriter);
	m_pVideoWriter = NULL;
	m_CSVideo.Leave();

	ResetStatus(STATE_RECORDINGONPC);
}


/////////////////////////////////////////////////////////////////////////////
// Start recording on USB
/////////////////////////////////////////////////////////////////////////////
void CDroneController::StartUSBRecording()
{
	DoLog("Start recording on usb");

	m_Drone.setVideoRecord(true);
	
	SetStatus(STATE_RECORDINGONUSB);
}


/////////////////////////////////////////////////////////////////////////////
// Stop recording on USB
/////////////////////////////////////////////////////////////////////////////
void CDroneController::StopUSBRecording()
{
	DoLog("Stop recording on usb");

	m_Drone.setVideoRecord(false);

	ResetStatus(STATE_RECORDINGONUSB);
}
