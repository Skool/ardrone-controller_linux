///////////////////////////////////////////////////////////
// Manages wifi connections
///////////////////////////////////////////////////////////


#pragma once

#include "WifiManager.h"

#include <wlanapi.h>
#include <objbase.h>
#include <wtypes.h>

#pragma comment(lib, "wlanapi.lib")
#pragma comment(lib, "ole32.lib")

///////////////////////////////////////////////////////////
// Constructor
///////////////////////////////////////////////////////////
CWifiManager::CWifiManager()
{
	m_lSignalIntensity			= 0;

	m_pInterfaceList			= NULL;
	m_pNetworkList				= NULL;

	m_hWifiApiHandle			= NULL;
	m_pNetworkInterface			= NULL;
	m_pConnectedNetwork			= NULL;
	m_pConnectedNetworkType		= NULL;
	m_bHasSecurity				= FALSE;
}


///////////////////////////////////////////////////////////
// Destructor
///////////////////////////////////////////////////////////
CWifiManager::~CWifiManager()
{
	Clean();
}


///////////////////////////////////////////////////////////
// Load api and init
///////////////////////////////////////////////////////////
eWifiErrors CWifiManager::Init()
{
	// Return value
	eWifiErrors WifiResult = WIFI_SUCCESS;

	DWORD					dwCurVersion	= 0;
    PWLAN_INTERFACE_INFO	pInterface		= NULL;
	PWLAN_AVAILABLE_NETWORK pNetwork		= NULL;

	// Open a handle to the Wlan (wifi) api 
	if(ERROR_SUCCESS != WlanOpenHandle(2, NULL, &dwCurVersion, &m_hWifiApiHandle))
	{		
        return WIFI_ERR_NO_HANDLE;
    }

	// Enum all available interfaces
	if(ERROR_SUCCESS != WlanEnumInterfaces(m_hWifiApiHandle, NULL, &m_pInterfaceList))
	{
        return WIFI_ERR_INTERFACES;
    }

	// Get the connected interface
	for (int i = 0; i < (int) m_pInterfaceList->dwNumberOfItems; i++) 
	{
		if(NULL != (pInterface = (WLAN_INTERFACE_INFO *) &m_pInterfaceList->InterfaceInfo[i]))
		{
			if(wlan_interface_state_connected == pInterface->isState)
			{
				if(NULL != m_pNetworkInterface)
				{
					// More than one connected interface found, take the first one only !
					WifiResult = WIFI_WRN_MULTIINTERFACES;
				}
				else
				{
					// Get now a list of acces points
					if(ERROR_SUCCESS != WlanGetAvailableNetworkList(m_hWifiApiHandle, &pInterface->InterfaceGuid, 0, NULL, &m_pNetworkList))
					{
						return WIFI_ERR_NETWORKS;
					}

					for(int j = 0; j < (int) m_pNetworkList->dwNumberOfItems; j++)
					{
						if(NULL != (pNetwork = (WLAN_AVAILABLE_NETWORK *) & m_pNetworkList->Network[j]))
						{
							if(pNetwork->dwFlags & WLAN_AVAILABLE_NETWORK_CONNECTED)
							{
								// Save the connected network informations
								if(NULL == m_pConnectedNetwork)
								{
									m_pConnectedNetwork = &(pNetwork->dot11Ssid);
									m_pConnectedNetworkType = &(pNetwork->dot11BssType);
									m_bHasSecurity = pNetwork->bSecurityEnabled;

									// Save also the network interface
									m_pNetworkInterface = &(pInterface->InterfaceGuid);
								}
								else
								{
									// Connected to more than one network
									WifiResult = WIFI_WRN_MULTINETWORKS;
								}
							}
						}
					}
				}
			}			
		}
	}

	// Verify that we have an interface and a connected network
	if( (NULL == m_pNetworkInterface) || (NULL == m_pConnectedNetwork) || (NULL == m_pConnectedNetworkType) )
	{
		WifiResult = WIFI_ERR_NO_CONNECTION;
		Clean();
	}
	else
	{
		// Get the signal value
		Update();
	}

	return WifiResult;
}


///////////////////////////////////////////////////////////
// Set ressources free
///////////////////////////////////////////////////////////
bool CWifiManager::Clean()
{
	m_pNetworkInterface			= NULL;
	m_pConnectedNetwork			= NULL;
	m_pConnectedNetworkType		= NULL;

	// Clean the interface list
    if (m_pInterfaceList != NULL) 
	{
        WlanFreeMemory(m_pInterfaceList);
        m_pInterfaceList = NULL;
    }

	// Clean the network list
    if (m_pNetworkList != NULL) 
	{
        WlanFreeMemory(m_pNetworkList);
        m_pNetworkList = NULL;
    }

	// Close the handle
	if(m_hWifiApiHandle != NULL)
	{
		WlanCloseHandle(m_hWifiApiHandle, NULL);
		m_hWifiApiHandle = NULL;
	}

	return true;
}


///////////////////////////////////////////////////////////
// Update the signal strength for connected access point
///////////////////////////////////////////////////////////
bool CWifiManager::Update()
{
	bool bSuccess = false;
	m_lSignalIntensity = 0;

	if(m_hWifiApiHandle != NULL)
	{
		// List should only contain one entry, as we are looking for one network, on a specific interface 
		PWLAN_BSS_LIST pBssList = NULL;
		
		if(ERROR_SUCCESS == WlanGetNetworkBssList(m_hWifiApiHandle, m_pNetworkInterface, m_pConnectedNetwork, *m_pConnectedNetworkType, m_bHasSecurity, NULL, &pBssList))
		{					
			if(pBssList->dwNumberOfItems > 0)
			{
				// Cast from ulong to long is Ok, as the value can only be between 0 and 100
				m_lSignalIntensity = (long)pBssList->wlanBssEntries[0].uLinkQuality;
				bSuccess = true;
			}
		}

		// Free the memory
		WlanFreeMemory(pBssList);
	}

	return bSuccess;
}


///////////////////////////////////////////////////////////
// Get the last received signal intensity
///////////////////////////////////////////////////////////
long CWifiManager::GetSignalIntensity()
{
	return m_lSignalIntensity;
}


///////////////////////////////////////////////////////////
// Get the name of the network
///////////////////////////////////////////////////////////
UCHAR const* CWifiManager::GetNetworkName()
{
	if(NULL == m_pConnectedNetwork)
	{
		return NULL;
	}
	return m_pConnectedNetwork->ucSSID;
}
