///////////////////////////////////////////////////////////
// Manages wifi connections
///////////////////////////////////////////////////////////

#ifndef __HEADER_WIFIMANAGER__
#define __HEADER_WIFIMANAGER__

#ifdef _WIN32
#include <windows.h>
#include <wlanapi.h>
#endif

// Possible errors using the wifi api
enum eWifiErrors
{
	WIFI_SUCCESS = 0,			// Operation successfull
	WIFI_ERR_NO_HANDLE,			// Error : No handle to api
	WIFI_ERR_INTERFACES,		// Error : Failed to list interfaces
	WIFI_ERR_NETWORKS,			// Error : Failed to list networks
	WIFI_ERR_NO_CONNECTION,		// Error : No connection, nothing to do
	WIFI_WRN_MULTIINTERFACES,	// Warning : More than one interface
	WIFI_WRN_MULTINETWORKS		// Warning : More than one connected network
};


class CWifiManager
{
public:
	// Constructor and destructor
	CWifiManager();
	~CWifiManager();

	// Load api and init
	eWifiErrors Init();
	// Set ressources free
	bool Clean();

	// Update the signal strength for connected access point
	bool Update();
	// Get the last received signal intensity
	long GetSignalIntensity();

	// Get the name of the network
	UCHAR const* GetNetworkName();

private:
	// Cached value of wifi signal intensity
	long							m_lSignalIntensity;

	//	Values / pointers used to store informations
	GUID*							m_pNetworkInterface;
	PDOT11_SSID						m_pConnectedNetwork;
	PDOT11_BSS_TYPE					m_pConnectedNetworkType;
	BOOL							m_bHasSecurity;

	// Allocated memory, have to be set free
	HANDLE							m_hWifiApiHandle;
	PWLAN_INTERFACE_INFO_LIST		m_pInterfaceList;
	PWLAN_AVAILABLE_NETWORK_LIST	m_pNetworkList;
};

#endif