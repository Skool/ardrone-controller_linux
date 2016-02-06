//////////////////////////////////////////////////////////////////////////////
// Watchdog Implementation - Background thread that checks the worker threads
// and restart them if needed
//////////////////////////////////////////////////////////////////////////////

#include "ardrone.h"

//////////////////////////////////////////////////////////////////////////////
// Starts the watchdog thread
//////////////////////////////////////////////////////////////////////////////
bool ARDrone::initWatchdog(void)
{
    DoLog("Initialize Watchdog thread");

    // Enable thread loop
	bflagWatchdog = true;

	// Init times of observed threads
	lCommandUpdateTime = m_pWatch->Time();
	lNavdataUpdateTime = m_pWatch->Time();
	lVideoUpdateTime = m_pWatch->Time();

    // Create a thread
	UINT id;
    threadWatchdog = (HANDLE)_beginthreadex(NULL, 0, runWatchdog, this, 0, &id);
    if (threadWatchdog == INVALID_HANDLE_VALUE)
	{
        CVDRONE_ERROR("_beginthreadex() was failed. (%s, %d)\n", __FILE__, __LINE__);
        return 0;
    }

	DoLog("Watchdog thread initialized");

    return 1;
}


//////////////////////////////////////////////////////////////////////////////
// The watchdog main loop
//////////////////////////////////////////////////////////////////////////////
UINT ARDrone::loopWatchdog(void)
{
	long lCurrentTime = 0;
	long lLocCommandTime = 0;
	long lLocNavdataTime = 0;
	long lLocVideoTime = 0;

	while (bflagWatchdog)
	{
		lCurrentTime = m_pWatch->Time();

		if(bflagCommand)
		{
			CSCommandWD.Enter();
			lLocCommandTime = lCommandUpdateTime;
			CSCommandWD.Leave();

			if ((lCurrentTime - lLocCommandTime) > 3000)
			{
				DoLog("Command thread is running but has not be updated for a long time, try to restart", MSG_ERROR);
				finalizeCommand();
				if(0 == initCommand())
				{
					DoLog("Failed to restart command thread, watchdog will try again", MSG_ERROR);
					bNeedCommandRestart = true;
				}
			}
		}
		else if (bNeedCommandRestart)
		{
			bNeedCommandRestart = false;
			DoLog("Command thread is not running, try to restart", MSG_ERROR);
			finalizeCommand();
			if(0 == initCommand())
			{
				DoLog("Failed to restart command thread, watchdog will try again", MSG_ERROR);
				bNeedCommandRestart = true;
			}
		}
		
		if(bflagNavdata)
		{
			CSNavdataWD.Enter();
			lLocNavdataTime = lNavdataUpdateTime;
			CSNavdataWD.Leave();

			if ((lCurrentTime - lLocNavdataTime) > 3000)
			{
				DoLog("Navdata thread is running but has not be updated for a long time, try to restart", MSG_ERROR);
				finalizeNavdata();
				if(0 == initNavdata())
				{
					DoLog("Failed to restart navdata thread, watchdog will try again", MSG_ERROR);
					bNeedNavdataRestart = true;
				}
			}
		}
		else if(bNeedNavdataRestart)
		{
			bNeedNavdataRestart = false;
			DoLog("Navdata thread is not running, try to restart", MSG_ERROR);
			finalizeNavdata();
			if(0 == initNavdata())
			{
				DoLog("Failed to restart navdata thread, watchdog will try again", MSG_ERROR);
				bNeedNavdataRestart = true;
			}
		}

		if(bflagVideo)
		{
			CSVideoWD.Enter();
			lLocVideoTime = lVideoUpdateTime;
			CSVideoWD.Leave();

			if ((lCurrentTime - lLocVideoTime) > 3000)
			{
				DoLog("Video thread is running but has not be updated for a long time, try to restart", MSG_ERROR);
				finalizeVideo();
				if(0 == initVideo())
				{
					DoLog("Failed to restart video thread, watchdog will try again", MSG_ERROR);
					bNeedVideoRestart = true;
				}
			}
		}
		else if(bNeedVideoRestart)
		{
			bNeedVideoRestart = false;
			DoLog("Video thread is not running, try to restart", MSG_ERROR);
			finalizeVideo();
			if(0 == initVideo())
			{
				DoLog("Failed to restart video thread, watchdog will try again", MSG_ERROR);
				bNeedVideoRestart = true;
			}
		}

        Sleep(100);
    }

    return 0;
}


//////////////////////////////////////////////////////////////////////////////
// End the watchdog thread
//////////////////////////////////////////////////////////////////////////////
void ARDrone::finalizeWatchdog(void)
{
    // Disable thread loop
    bflagWatchdog = false;

	// Wait a moment, so the main loop can finish
	Sleep(100);

	DoLog("Finalize Watchdog");

    // Destroy the thread
    if (threadWatchdog != INVALID_HANDLE_VALUE)
	{
		DWORD dwRes = WaitForSingleObject(threadWatchdog, 2500);
		switch(dwRes)
		{
			case WAIT_TIMEOUT:
				DoLog("Wait for Watchdog: thread timed out !", MSG_ERROR);
				TerminateThread(threadWatchdog, -1);
				break;

			case WAIT_FAILED:
				DoLog("Wait for Watchdog: thread failed !", MSG_ERROR);
				TerminateThread(threadWatchdog, -1);
				break;

			default:
				break;
		}

        if(FALSE == CloseHandle(threadWatchdog))
		{
			DoLog("Failed to close Watchdog thread handle", MSG_ERROR);
		}
        threadWatchdog = INVALID_HANDLE_VALUE;
    }    

	DoLog("Watchdog thread finalized");
}