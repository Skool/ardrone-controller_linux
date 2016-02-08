/////////////////////////////////////////////////////////////////////////////
// Some utils - Definition
/////////////////////////////////////////////////////////////////////////////

#ifndef __HEADER_DRONE_UTILS__
#define __HEADER_DRONE_UTILS__

#include <wx/wx.h>
#include <wx/thread.h>

// String containing the version
const wxString VersionString("ArDrone controller 1.6.0");

// Battery levels
const int iBatCritical		= 10L; // Drone will force landing soon
const int iBatDanger		= 15L; // We're really short coco !
const int iBatWarning		= 24L; // Time to go home

// Wifi quality levels
const long lWifiCritical	= 15L; // Signal may be lost at any time now
const long lWifiDanger		= 30L; // Video probably looks like a slideshow, or has already been lost
const long lWifiWarning		= 44L; // Video get some lags and may be lost

// Define some colors
const wxColour ColorRed(255,0,0);
const wxColour ColorOrange(255,165,0);
const wxColour ColorYellow(230,230,0);
const wxColour ColorGreen(0,180,0);
const wxColour ColorBlue(0,0,255);
const wxColour ColorWhite(255,255,255);
const wxColour ColorGrey(204,204,204);
const wxColour ColorBlack(0,0,0);

const wxColor ColorRedLite(180,0,0);		// Cancel buttons
const wxColor ColorGreenLite(0,180,0);		// Ok buttons
const wxColor ColorBlueLite(0,0,180);		// Text color in dialogs

// Define some fonts
const wxFont FontBold12(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
const wxFont FontBold18(18, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
const wxFont FontNormal10(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
const wxFont FontNormal8(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

// Calculate pi over 180 (used for trigonometry operations -> degree to radian)
const double dPIover180 = (3.14159265358979323846264338327/180.0);

// Calculate 180 over PI (used for trigonometry operations -> rad to degree)
const double d180overPI = (180.0/3.14159265358979323846264338327);

// Calculate meters to feet (1 meter = 3.2808399 feets)
const double dMetertoFeet = 3.2808399f;

// Possible message and log levels
enum eMsgLevel
{
	MSG_INFO = 1,
	MSG_WARNING,
	MSG_ERROR
};

// Load a png file from ressources
bool GetPngFromRessource(const wxString& strRessourceName, wxBitmap& Bitmap);

// Display a message
void DoMessage(wxString strMessage, eMsgLevel MsgLevel);

// Log to file
void DoLog(wxString strMessage, eMsgLevel MsgLevel = MSG_INFO);

// Get a text in current language
wxString GetText(wxString strKey);

// Convert string to long
long ToLong(wxString strValue);

// Convert long to string
wxString ToString(long lValue);

#endif