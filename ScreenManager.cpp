///////////////////////////////////////////////////////////
// Manages screens
///////////////////////////////////////////////////////////


#pragma once

#include "Utils.h"
#include "ScreenManager.h"


CScreenManager::CScreenManager()
{
	m_bInit = false;
	m_bIsLowRes = false;

	// Normally the user fly his drone outside, this means, on a laptop (one screen)
	int index=0;
    DISPLAY_DEVICE Device;
    Device.cb = sizeof(DISPLAY_DEVICE);
 
    while (EnumDisplayDevices(NULL, index++, &Device, 0))
    {
        if (Device.StateFlags & DISPLAY_DEVICE_PRIMARY_DEVICE)
		{
			m_MainDevice = Device;
			m_bInit = true;
			break;
		}
    }
}


CScreenManager::~CScreenManager()
{

}


bool CScreenManager::SetLowResolution()
{
	if(!m_bInit)
	{
		return false;
	}

	DEVMODE DevMod;
    DevMod.dmSize = sizeof(DEVMODE);
    if (!EnumDisplaySettings(m_MainDevice.DeviceName, ENUM_CURRENT_SETTINGS, &DevMod))
    {
		DoLog("Failed to enum display settings, low resolution canceled", MSG_ERROR);
        return false;
    }
 
	if( (DevMod.dmPelsWidth <= 1366) || (DevMod.dmPelsWidth <= 768) )
	{
		DoLog("Screen has already a low res, ignore resolution change", MSG_WARNING);
		return true;
	}

	// Flag for what we want to set
	DevMod.dmFields = (DM_PELSWIDTH | DM_PELSHEIGHT);

	// Try a 19/9 low res
    DevMod.dmPelsWidth = 1366;
    DevMod.dmPelsHeight = 768;   
    if (ChangeDisplaySettings(&DevMod, CDS_TEST) != DISP_CHANGE_SUCCESSFUL)
    {
		// Try a 4/3 low res
		DevMod.dmPelsWidth = 1024;
		DevMod.dmPelsHeight = 768;
		if (ChangeDisplaySettings(&DevMod, CDS_TEST) != DISP_CHANGE_SUCCESSFUL)
		{
			DoLog("Low resolution display settings not supported, canceled", MSG_ERROR);
			return false;
		 }
    }
 
    if(ChangeDisplaySettings(&DevMod, 0) != DISP_CHANGE_SUCCESSFUL)
	{
		DoLog("Failed to apply low resolution display settings", MSG_ERROR);
        return false;
	}

	m_bIsLowRes = true;
	
	return true;
}


void CScreenManager::ResetResolution()
{
	if(m_bIsLowRes)
	{
		// Takes the registry settings
		ChangeDisplaySettings(NULL, 0);
		m_bIsLowRes = false;
	}
}
