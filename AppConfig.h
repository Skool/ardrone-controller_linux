//////////////////////////////////////////////////////////////////////////////
// Description of CConfig
//////////////////////////////////////////////////////////////////////////////
// Utility class used to hold some configuration informations
//////////////////////////////////////////////////////////////////////////////

#ifndef __HEADER_APPCONFIG__
#define __HEADER_APPCONFIG__

// Includes
#include "wx/wx.h"
#include <wx/string.h>
#include <wx/dir.h>
#include <wx/arrstr.h>
#include <wx/textfile.h>
#include <wx/hashmap.h>



// An enum for video codecs
enum eVideoCodec
{
	Codec_MP4_360P				= 0x80,	// Not used
	Codec_H264_360P				= 0x81,
	Codec_MP4_360P_H264_720P	= 0x82,
	Codec_H264_720P				= 0x83,
	Codec_MP4_360P_H264_360P	= 0x88	// Not used
};

// Define a map
WX_DECLARE_STRING_HASH_MAP( wxString, LanguageMap );

// Describe the config class
class CConfig
{
private:
	// The one and only singleton object
	static CConfig *msp_ConfigSingleton;
	// Map containing the language strings
    LanguageMap		m_LanguageMap;

	// Constructor and destructor are private
	// -> Don't allow multiple instances
	CConfig();
    ~CConfig();
    

	///////////////////////////////////////
	// Configuration variables

    // Current Language
    wxString			m_strLanguage;
	// Play sound or not
	bool				m_bPlaySound;
	// Video codec to use
	eVideoCodec			m_VideoCodec;
	// Record options used when an usb key or the flight recorder is connected (1=usb, 2=usb+pc, 3=pc)
	long				m_lRecordOption;
	// Hud style to use
	int					m_iHudStyle;
	// Color of the hud
	wxColour			m_HudColor;
	// Line size of the hud
	int					m_iHudLineSize;
	// Use US units (feet) instead of EU (meters)
	bool				m_bUseUSUnit;
	// Low resolution flag for fullscreen mode
	bool				m_bUseLowRes;
	// Speed limitation (% of the max speed, minimum is 10%)
	long				m_lSpeedLimit;
	// Acceleration limit (% of the max acceleration)
	long				m_lAccelerationLimit;
	// Altitude limitation (in meters)
	long				m_lAltitudeLimit;
	// Ip Address to use
	wxString			m_strIpAddress;

	// Critical section to protect data
	wxCriticalSection	m_CSConfig;
		    
public:
	// Get the singleton object
    static CConfig* GetSingleton();
	// Destroy the singleton object
    static void		KillSingleton();
    
    // Check the file structure
    bool CheckConfig();
	
	// Load the saved config
	bool LoadConfig();
	// Save the current config
	bool SaveConfig();
	
	// List the languages files available in the language folder
	bool GetPossibleLanguages(wxArrayString *parLanguages);	
    // Load current language
	bool LoadLanguage();
	// Load a specific language
	bool LoadLanguage(const wxString& strLanguage);
	// Get a string in the current language
	wxString GetText(const wxString& strTargetName);

	///////////////////////////////////////
	// Access the configuration variables

	// Application language
	void SetLanguage(const wxString &strLanguage);
	const wxString& GetLanguage();	

	// Sound flag
	void SetSoundFlag(bool bPlaySound);
	bool HasSoundFlag();

	// Video codec
	void SetVideoCodec(eVideoCodec VideoCodec);
	eVideoCodec GetVideoCodec();

	// Record options
	void SetRecordOption(long lRecordOption);
	long GetRecordOption();

	// Hud style to use
	void SetHudStyle(int iHudStyle);
	int GetHudStyle();

	// Color of the HUD
	void SetHudColor(wxColour Color);
	wxColour GetHudColor();

	// Line size of the HUD
	void SetHudLineSize(int iSize);
	int GetHudLineSize();

	// Should the HUD display us units or not
	void SetUsUnitFlag(bool bUseUSUnits);
	bool HasUSUnit();

	// Should low res be used in fullscreen mode or not
	void SetLowResFlag(bool bUseLowRes);
	bool UseLowRes();

	// Speed limit
	void SetSpeedLimit(long lSpeedLimit);
	long GetSpeedLimit();

	// Acceleration limit
	void SetAccelerationLimit(long lAccelerationLimit);
	long GetAccelerationLimit();

	// Altitude limit
	void SetAltitudeLimit(long lAltitudeLimit);
	long GetAltitudeLimit();

	// Ip Address
	void SetIpAddress(const wxString &strIpAddress);
	const wxString& GetIpAddress();
};

#endif