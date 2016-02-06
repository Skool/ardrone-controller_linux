/////////////////////////////////////////////////////////////////////////////
// Some utils - Implementation
/////////////////////////////////////////////////////////////////////////////

#include <wx/mstream.h>

#include "Utils.h"
#include "AppConfig.h"

// A global critical section nedded for logging
wxCriticalSection CSLock;

/////////////////////////////////////////////////////////////////////////////
// Load a png file from ressources
/////////////////////////////////////////////////////////////////////////////
bool GetPngFromRessource(const wxString& strRessourceName, wxBitmap& Bitmap)
{	
	bool			bSuccess	= false;	// Result of operation
	char*			pData		= NULL;		// Image raw data
	unsigned long	ulDataSize	= 0;		// Image data size
	HGLOBAL			hHandle		= NULL;		// Handle to ressource
    HRSRC			pResource	= NULL;		// Pointer to ressource

	// Verify that the ressource exist
	pResource = FindResource(0, strRessourceName.wchar_str(), RT_RCDATA);  
	if(NULL != pResource)
	{
		// Load the ressource
		hHandle = LoadResource(NULL, pResource);
		if (NULL != hHandle)
		{
			// Get data and size
			pData = (char*)LockResource(hHandle);
			ulDataSize = SizeofResource(NULL, pResource);
			
			// Convert to wxImage
			wxMemoryInputStream iStream(pData, ulDataSize);
			Bitmap = wxBitmap(wxImage(iStream, wxBITMAP_TYPE_PNG, -1), -1);
			
			bSuccess = true;
		}
	}

	return bSuccess;
}


/////////////////////////////////////////////////////////////////////////////
// Display a message
// Note: No lock needed, only called from GUI thread
/////////////////////////////////////////////////////////////////////////////
void DoMessage(wxString strMessage, eMsgLevel MsgLevel)
{
	wxString strTitle;
	long lStyle = wxOK;
	
	if(MSG_WARNING == MsgLevel)
	{
		strTitle = "Warning";
		lStyle |= wxICON_WARNING;
	}
	else if(MSG_ERROR == MsgLevel)
	{
		strTitle = "Error";
		lStyle |= wxICON_ERROR;
	}
	else
	{
		strTitle = "Info";
		lStyle |= wxICON_INFORMATION;
	}

	wxMessageBox(strMessage, strTitle, lStyle);
}


/////////////////////////////////////////////////////////////////////////////
// Log to file
// Note: Lock is needed, as the function may be called from multiple threads
/////////////////////////////////////////////////////////////////////////////
void DoLog(wxString strMessage, eMsgLevel MsgLevel)
{
	wxCriticalSectionLocker Lock(CSLock);	

	// Type of message (info, warning, error)
	wxString strType;
	
	// Text file containing the device config
	wxTextFile LogFile("Data\\Log\\Errors.log");
	
    // Verify if the file exist
	if(LogFile.Exists())
	{
		// If the file exist, open it
		if(!LogFile.Open())
		{	
			return;
		}
	}
	else
	{
		// Create the file
		if(!LogFile.Create())
		{
			return;	
		}
	}
	
	SYSTEMTIME st;
	GetLocalTime(&st);

	switch(MsgLevel)
	{
		case MSG_INFO		: strType = wxString::Format("[NFO] [%02d:%02d:%02d,%03d] ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); break;
		case MSG_WARNING	: strType = wxString::Format("[WRN] [%02d:%02d:%02d,%03d] ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); break;
		default				: strType = wxString::Format("[ERR] [%02d:%02d:%02d,%03d] ", st.wHour, st.wMinute, st.wSecond, st.wMilliseconds); break;
	}

	LogFile.AddLine(strType + strMessage);

	// Save the changes
	LogFile.Write();
    // Close the file
    LogFile.Close();
}


/////////////////////////////////////////////////////////////////////////////
// Get a text in current language
/////////////////////////////////////////////////////////////////////////////
wxString GetText(wxString strKey)
{
	return CConfig::GetSingleton()->GetText(strKey);
}


/////////////////////////////////////////////////////////////////////////////
// Convert string to long
/////////////////////////////////////////////////////////////////////////////
long ToLong(wxString strValue)
{
	long lTmpVal = 0;
	
	strValue.ToLong(&lTmpVal);
	
	return lTmpVal;
}


/////////////////////////////////////////////////////////////////////////////
// Convert long to string
/////////////////////////////////////////////////////////////////////////////
wxString ToString(long lValue)
{
	return wxString::Format("%d", lValue);
}