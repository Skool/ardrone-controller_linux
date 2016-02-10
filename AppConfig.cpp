//////////////////////////////////////////////////////////////////////////////
// Implementation of CConfig
//////////////////////////////////////////////////////////////////////////////

#include "AppConfig.h"
#include "Utils.h"

// Initialize the config singleton to NULL
CConfig* CConfig::msp_ConfigSingleton = NULL;


//////////////////////////////////////////////////////////////////////////////
// Default constructor of CConfig
////////////////////////////////////////////////////////////////////////////// 
CConfig::CConfig()
{
	// Set defaults
	m_strLanguage			= "English";
	m_bPlaySound			= true;
	m_VideoCodec			= Codec_H264_360P;
	m_lRecordOption			= 1;
	m_iHudStyle				= 1;
	m_HudColor				= ColorGreenLite;
	m_iHudLineSize			= 2;
	m_bUseUSUnit			= false;
	m_bUseLowRes			= false;
	m_lSpeedLimit			= 50;
	m_lAccelerationLimit	= 50;
	m_lAltitudeLimit		= 10;
	m_strIpAddress			= "192.168.1.1";
}


//////////////////////////////////////////////////////////////////////////////
// Default destructor of CConfig
////////////////////////////////////////////////////////////////////////////// 
CConfig::~CConfig()
{
}


//////////////////////////////////////////////////////////////////////////////   
// Check the file structure
//////////////////////////////////////////////////////////////////////////////
// [RETURN] : True if the configuration is alright 
//////////////////////////////////////////////////////////////////////////////
bool CConfig::CheckConfig()
{
	// Data directory must exist
    if(!wxDirExists("Data"))
	{
        return false;
    }
    
    // The language directory must exist
    if(!wxDirExists("Data/Lang"))
    {
		return false;
	}

	// Directory or configuration files
    if(!wxDirExists("Data/Config"))
    {
        return false;
    }
    
    // Directory for Log files
    if(!wxDirExists("Data/Log"))
    {
        if(!wxMkdir("Data/Log"))
        {
            return false;
        }
    }

	// Directories for media files (photos-videos)
	if(!wxDirExists("Media"))
    {
        if(!wxMkdir("Media"))
        {
            return false;
        }
    }

	if(!wxDirExists("Media/Pictures"))
    {
        if(!wxMkdir("Media/Pictures"))
        {
            return false;
        }
    }

	if(!wxDirExists("Media/Movies"))
    {
        if(!wxMkdir("Media/Movies"))
        {
            return false;
        }
    }

	return true;
}


//////////////////////////////////////////////////////////////////////////////
// Get the singleton object
//////////////////////////////////////////////////////////////////////////////
// [RETURN] : Pointer to the singleton
//////////////////////////////////////////////////////////////////////////////
/*static*/ CConfig* CConfig::GetSingleton()
{
	// If the object has not been created yet create it
	if (NULL == msp_ConfigSingleton)
	{
		msp_ConfigSingleton =  new CConfig();
	}
  
  	// Return the pointer
    return msp_ConfigSingleton;
}


//////////////////////////////////////////////////////////////////////////////
// Destroy the singleton object
//////////////////////////////////////////////////////////////////////////////
/*static*/ void CConfig::KillSingleton()
{
	// If the object exist, delete it and set the pointer to NULL
	if(NULL != msp_ConfigSingleton)
	{
		delete msp_ConfigSingleton;
		msp_ConfigSingleton = NULL;	
	}
}


//////////////////////////////////////////////////////////////////////////////
// Set a new language to use
//////////////////////////////////////////////////////////////////////////////
// [IN] : Name of the language/file to use
////////////////////////////////////////////////////////////////////////////// 	
void CConfig::SetLanguage(const wxString &strLanguage)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_strLanguage = strLanguage;
}


//////////////////////////////////////////////////////////////////////////////
// Get the current language
//////////////////////////////////////////////////////////////////////////////
// [RETURN] : Name of the language/file currently in use
////////////////////////////////////////////////////////////////////////////// 	
const wxString& CConfig::GetLanguage()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_strLanguage;
}


//////////////////////////////////////////////////////////////////////////////
// Set sound flag
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetSoundFlag(bool bPlaySound)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_bPlaySound = bPlaySound;
}


//////////////////////////////////////////////////////////////////////////////
// Get current sound flag
//////////////////////////////////////////////////////////////////////////////
bool CConfig::HasSoundFlag()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_bPlaySound;
}


//////////////////////////////////////////////////////////////////////////////
// Set video codec
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetVideoCodec(eVideoCodec VideoCodec)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_VideoCodec = VideoCodec;
}


//////////////////////////////////////////////////////////////////////////////
// Get configured codec
//////////////////////////////////////////////////////////////////////////////
eVideoCodec CConfig::GetVideoCodec()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_VideoCodec;
}


//////////////////////////////////////////////////////////////////////////////
// Set the record option
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetRecordOption(long lRecordOption)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_lRecordOption = lRecordOption;
}


//////////////////////////////////////////////////////////////////////////////
// Get the current record option
//////////////////////////////////////////////////////////////////////////////
long CConfig::GetRecordOption()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_lRecordOption;
}


//////////////////////////////////////////////////////////////////////////////
// Set HUD style to use
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetHudStyle(int iHudStyle)
{
	if(iHudStyle == 1 || iHudStyle == 2)
	{
		wxCriticalSectionLocker LockConfig(m_CSConfig);
		m_iHudStyle = iHudStyle;
	}
}


//////////////////////////////////////////////////////////////////////////////
// Get current HUD style
//////////////////////////////////////////////////////////////////////////////
int CConfig::GetHudStyle()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_iHudStyle;
}


//////////////////////////////////////////////////////////////////////////////
// Set color of the HUD
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetHudColor(wxColour Color)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_HudColor = Color;
}


//////////////////////////////////////////////////////////////////////////////
// Get current color of the HUD
//////////////////////////////////////////////////////////////////////////////
wxColour CConfig::GetHudColor()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_HudColor;
}


//////////////////////////////////////////////////////////////////////////////
// Set the line size of the HUD
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetHudLineSize(int iSize)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_iHudLineSize = iSize;
}


//////////////////////////////////////////////////////////////////////////////
// Get current line size of the hud
//////////////////////////////////////////////////////////////////////////////
int CConfig::GetHudLineSize()
{
	return m_iHudLineSize;
}


//////////////////////////////////////////////////////////////////////////////
// Set the flag for US units
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetUsUnitFlag(bool bUseUSUnits)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_bUseUSUnit = bUseUSUnits;
}


//////////////////////////////////////////////////////////////////////////////
// Get the flag for US units
//////////////////////////////////////////////////////////////////////////////
bool CConfig::HasUSUnit()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_bUseUSUnit;
}


//////////////////////////////////////////////////////////////////////////////
// Set the flag for low res
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetLowResFlag(bool bUseLowRes)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_bUseLowRes = bUseLowRes;
}


//////////////////////////////////////////////////////////////////////////////
// Get the flag for low res
//////////////////////////////////////////////////////////////////////////////
bool CConfig::UseLowRes()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_bUseLowRes;
}


//////////////////////////////////////////////////////////////////////////////
// Set speed limit
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetSpeedLimit(long lSpeedLimit)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_lSpeedLimit = lSpeedLimit;
}


//////////////////////////////////////////////////////////////////////////////
// Get current speed limit
//////////////////////////////////////////////////////////////////////////////
long CConfig::GetSpeedLimit()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_lSpeedLimit;
}


//////////////////////////////////////////////////////////////////////////////
// Set acceleration limit
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetAccelerationLimit(long lAccelerationLimit)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_lAccelerationLimit = lAccelerationLimit;
}


//////////////////////////////////////////////////////////////////////////////
// Get acceleration limit
//////////////////////////////////////////////////////////////////////////////
long CConfig::GetAccelerationLimit()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_lAccelerationLimit;
}


//////////////////////////////////////////////////////////////////////////////
// Set altitude limit
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetAltitudeLimit(long lAltitudeLimit)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_lAltitudeLimit = lAltitudeLimit;
}


//////////////////////////////////////////////////////////////////////////////
// Get altitude limit
//////////////////////////////////////////////////////////////////////////////
long CConfig::GetAltitudeLimit()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_lAltitudeLimit;
}


//////////////////////////////////////////////////////////////////////////////
// Set the Ip Address to use
//////////////////////////////////////////////////////////////////////////////
void CConfig::SetIpAddress(const wxString &strIpAddress)
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	m_strIpAddress = strIpAddress;
}


//////////////////////////////////////////////////////////////////////////////
// Get the current Ip Address
//////////////////////////////////////////////////////////////////////////////
const wxString& CConfig::GetIpAddress()
{
	wxCriticalSectionLocker LockConfig(m_CSConfig);

	return m_strIpAddress;
}


//////////////////////////////////////////////////////////////////////////////
// Load the saved config
//////////////////////////////////////////////////////////////////////////////
// [RETURN] : true if config could successfully be loaded from file
////////////////////////////////////////////////////////////////////////////// 
bool CConfig::LoadConfig()
{
	// Text file containing the config
	wxTextFile ConfigFile("Data/Config/Config.ini");
	
    // Verify if the file exist
	if(ConfigFile.Exists())
	{
		// If the file exist, open it
		if(ConfigFile.Open())
		{
			int iLine=0;

			// Read the language
			m_strLanguage = ConfigFile.GetLine(iLine++);

			// Get the sound flag
			if(1 == ToLong(ConfigFile.GetLine(iLine++)))
			{
				m_bPlaySound = true;
			}
			else
			{
				m_bPlaySound = false;
			}

			// Read codec
			m_VideoCodec = (eVideoCodec)ToLong(ConfigFile.GetLine(iLine++));

			// Record option
			m_lRecordOption = ToLong(ConfigFile.GetLine(iLine++));

			// Get the HUD style
			m_iHudStyle = ToLong(ConfigFile.GetLine(iLine++));

			// Read HUD color
			m_HudColor = wxColor(ConfigFile.GetLine(iLine++));

			// Get line size of the HUD
			m_iHudLineSize = ToLong(ConfigFile.GetLine(iLine++));

			// Get US unit flag
			if(1 == ToLong(ConfigFile.GetLine(iLine++)))
			{
				m_bUseUSUnit = true;
			}
			else
			{
				m_bUseUSUnit = false;
			}

			// Low resolution flag
			if(1 == ToLong(ConfigFile.GetLine(iLine++)))
			{
				m_bUseLowRes = true;
			}
			else
			{
				m_bUseLowRes = false;
			}

			// Get speed limit
			m_lSpeedLimit = ToLong(ConfigFile.GetLine(iLine++));

			// Get acceleration limit
			m_lAccelerationLimit = ToLong(ConfigFile.GetLine(iLine++));

			// Get altitude limit
			m_lAltitudeLimit = ToLong(ConfigFile.GetLine(iLine++));

			// Get Ip address
			m_strIpAddress = ConfigFile.GetLine(iLine++);

			// Close the file
			ConfigFile.Close();
			
			// Leave function
			return true;
		}
		else
		{
			// File exist, but cannot be loaded !
			return false;
		}
	}	
	
	return true;
}


//////////////////////////////////////////////////////////////////////////////
// Save the current config
//////////////////////////////////////////////////////////////////////////////
// [RETURN] : true if config could successfully be saved to file
////////////////////////////////////////////////////////////////////////////// 
bool CConfig::SaveConfig()
{
	// Text file containing the config
	wxTextFile ConfigFile("Data/Config/Config.ini");
	
    // Verify if the file exist
	if(ConfigFile.Exists())
	{
		// If the file exist, open it
		if(!ConfigFile.Open())
		{	
			return false;
		}
	}
	else
	{
		// Create the file
		if(!ConfigFile.Create())
		{
			return false;	
		}
	}
	
	// Clear the file
	ConfigFile.Clear();

	m_CSConfig.Enter();
	
	// Add the language information to the config file
	ConfigFile.AddLine(m_strLanguage);	
	// Add sound config
	ConfigFile.AddLine(m_bPlaySound ? "1":"0");
	// Add Codec
	ConfigFile.AddLine(ToString(m_VideoCodec));
	// Add record option
	ConfigFile.AddLine(ToString(m_lRecordOption));
	// Add HUD style
	ConfigFile.AddLine(ToString(m_iHudStyle));
	// Add HUD color
	ConfigFile.AddLine(m_HudColor.GetAsString());
	// Add HUD line size
	ConfigFile.AddLine(ToString(m_iHudLineSize));
	// Add US unit flag
	ConfigFile.AddLine(m_bUseUSUnit ? "1":"0");
	// Add low res flag
	ConfigFile.AddLine(m_bUseLowRes ? "1":"0");
	// Add speed limit
	ConfigFile.AddLine(ToString(m_lSpeedLimit));
	// Add acceleration limit
	ConfigFile.AddLine(ToString(m_lAccelerationLimit));
	// Add altitude limit
	ConfigFile.AddLine(ToString(m_lAltitudeLimit));
	// Add Ip Adress
	ConfigFile.AddLine(m_strIpAddress);

	m_CSConfig.Leave();

	// Save the changes
	ConfigFile.Write();
    // Close the file
    ConfigFile.Close();
	
	return true;
}


//////////////////////////////////////////////////////////////////////////////
// List the languages files available in the language folder
//////////////////////////////////////////////////////////////////////////////
// [OUT]	: Pointer to string array containing the found languages
// [RETURN]	: true on success, false if an error occur
//////////////////////////////////////////////////////////////////////////////
bool CConfig::GetPossibleLanguages(wxArrayString *parLanguages)
{
	// Language directory
	wxDir MyDir;
	// Name of the language file
	wxString strFileName;
	// Flag for iteration control
	bool bLoop = true;
	
	// Clear the array
	parLanguages->Clear();
		
	// Open the dir to iterate over language files
	if(!MyDir.Open("Data/Lang"))
	{
		return false;
	}
	
	// Get the first file
	bLoop = MyDir.GetFirst(&strFileName, "*.lng", wxDIR_FILES|wxDIR_HIDDEN);
	
	// Iterate over the folder as long a language file can be found
	while(bLoop)
	{
		// Take only the first part of the filename (without file extension)
		strFileName = strFileName.BeforeLast('.');
		if(!strFileName.IsEmpty())
		{
			// Add it to the list of languages
			parLanguages->Add(strFileName);
		}
		
		// Get next file
		bLoop = MyDir.GetNext(&strFileName);
	}
	
	return true;
}


//////////////////////////////////////////////////////////////////////////////
// Load current language
//////////////////////////////////////////////////////////////////////////////
// [RETURN]	: true on success, false if an error occur
//////////////////////////////////////////////////////////////////////////////
bool CConfig::LoadLanguage()
{	
	return this->LoadLanguage(m_strLanguage);
}


//////////////////////////////////////////////////////////////////////////////
// Load a specific language
//////////////////////////////////////////////////////////////////////////////
// [IN]		: Language to use
// [RETURN]	: true on success, false if an error occur
//////////////////////////////////////////////////////////////////////////////
bool CConfig::LoadLanguage(const wxString& strLanguage)
{
	// Temporary value
	wxString strTmp;
	// Key name
	wxString strKey;
	// Value of the key in the selected language
	wxString strValue;
	// Number of lines in the file
	long lCount = 0;
	// Position in the file
	long lPos = 0;
	// Position of equality sign in the string
	int iTokenPos = 0;
	// Return value
	bool bRet = true;
	
	// Text file containing the language strings
	wxTextFile LangFile("Data/Lang/" + strLanguage + ".lng");
	
    // Verify if the file exist
	if(!LangFile.Exists())
	{
		return false;
	}
		
	// If the file exist, open it
	if(!LangFile.Open())
	{
		DoLog(wxString("Failed to open language file ") + strLanguage, MSG_ERROR);
		return false;
	}
		
	// Now copy all the strings to the map
	if( 0 >= (lCount = LangFile.GetLineCount()))
	{
		// The file is empty !
		bRet = false;	
	}
	else
	{
		// Clear the current map before we fill it with another language
		m_LanguageMap.empty();
		
		// Iterate over the file
		for(lPos = 0; lPos < lCount; lPos++)
		{
			strTmp = LangFile.GetLine(lPos);			
			
			// Ignore empty strings or commented strings
			if( strTmp.IsNull() || strTmp.IsEmpty() )
			{
				continue;
			}

			// Trim blanks from left
			strTmp.Trim(false);
			// Now check that the string does not begin with ";" or "//"
			if( (strTmp.Left(1) == ";") || (strTmp.Left(2) == "//") )
			{
				continue;
			}
			
			// A "=" sign must be found to be a valid string
			if(wxNOT_FOUND != (iTokenPos = strTmp.Find('=')))
			{
				// Trim only the white spaces from left
				strTmp = strTmp.Trim(false);
				
				// Get the left part of the string as key
				strKey = strTmp.BeforeFirst('=');
				// Get the right part of the string as key
				strValue = strTmp.AfterFirst('=');
				
				// Set the values into the map
				m_LanguageMap[strKey] =  strValue;
			}
		}
	}
	
	// Close the file
	LangFile.Close();
	
	return bRet;
}


//////////////////////////////////////////////////////////////////////////////
// Get a string in the current language
//////////////////////////////////////////////////////////////////////////////
// [IN] : Name of the string we are looking for
// [RETURN] : String in the current language
//////////////////////////////////////////////////////////////////////////////
wxString CConfig::GetText(const wxString& strTargetName)
{
	// Return string
	wxString strValue;
	// Iterator tor the map
	LanguageMap::iterator LangIt;
	
	// Look for the specified name
	LangIt = m_LanguageMap.find(strTargetName);
	
	if(m_LanguageMap.end() != LangIt)
	{
		// Get the string from the map
		strValue = LangIt->second;
	}
	else
	{
		// String could not be found in the map so return a default value
		strValue = L"ER__" + strTargetName;	
	}
	
	return strValue;
}
