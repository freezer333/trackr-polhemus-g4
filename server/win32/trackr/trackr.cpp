// FTconsole.cpp : Defines the entry point for the console application.
//

#pragma once

#include <SDKDDKVer.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <windows.h>
#include <wincon.h>


#include <string>
#include <iostream>
#include "PDI.h"

#include "tracker_props.h"

using namespace std;
typedef basic_string<TCHAR> tstring;
#if defined(UNICODE) || defined(_UNICODE)
#define tcout std::wcout
#else
#define tcout std::cout
#endif

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
CPDIg4		g_pdiDev;
DWORD		g_dwFrameSize;
BOOL		g_bCnxReady;
DWORD		g_dwStationMap;
DWORD		g_dwLastHostFrameCount;
ePDIoriUnits g_ePNOOriUnits;



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

typedef enum
{
	CHOICE_CONT
	, CHOICE_SINGLE
	, CHOICE_QUIT
	, CHOICE_ENTER
	, CHOICE_HUBMAP
	, CHOICE_OPTIONS

	, CHOICE_NONE = -1
} eMenuChoice;

#define ESC	0x1b
#define ENTER 0x0d
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void		Initialize			(  );
BOOL		Connect				( VOID );
VOID		Disconnect			( VOID );
BOOL		SetupDevice			( VOID );
VOID		UpdateStationMap	( VOID );
VOID		AddMsg				( tstring & );
VOID		AddResultMsg		( LPCTSTR );
VOID		ShowMenu			( VOID );	
eMenuChoice Prompt				( VOID );
BOOL		StartCont			( VOID );
BOOL		StopCont			( VOID );	
VOID		DisplayCont			( VOID );
VOID		DisplaySingle		( VOID );
VOID		ParseG4NativeFrame	( PBYTE, DWORD );
eMenuChoice		CnxPrompt			( VOID );
VOID SetOriUnits( ePDIoriUnits eO );
VOID SetPosUnits( ePDIposUnits eP );
VOID ShowOptionsMenu( VOID );
VOID OptionsPrompt( VOID );

#define BUFFER_SIZE 0x1FA400   // 30 seconds of xyzaer+fc 8 sensors at 240 hz 
//BYTE	g_pMotionBuf[0x0800];  // 2K worth of data.  == 73 frames of XYZAER
BYTE	g_pMotionBuf[BUFFER_SIZE]; 
TCHAR	g_G4CFilePath[_MAX_PATH+1];

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int _tmain(int argc, TCHAR* argv[])
{

	int connection_tries = 0;
	bool failed;
	do {
		failed = false;
		Initialize();
		if (!failed &&  !Connect() ) {
			std::cerr << "Error connecting to g4 tracker" << endl;
			failed = true;
		}
		else if (!failed &&  !SetupDevice()) {
			std::cerr << "Error setting up g4 tracker" << endl;
			failed = true;
		}
		else if (!failed &&  !StartCont() ) {
			std::cerr << "Error initiating continous polling of g4 tracker" << endl;
			failed = true;
		}
		if ( failed ) {
			std::cerr << "... waiting 5 seconds to retry..." << endl;
			Sleep(5000);  
		}
	} while ( connection_tries < 20 && failed );

	DisplayCont();
	StopCont();
	Disconnect();
	return 0;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void Initialize(  )
{
	g_pdiDev.Trace(TRUE, 7);
	_tcsncpy_s(g_G4CFilePath, _countof(g_G4CFilePath), G4C_PATH, _tcslen(G4C_PATH));
	g_bCnxReady = FALSE;
	g_dwStationMap = 0;
	g_ePNOOriUnits = E_PDI_ORI_QUATERNION;
}


VOID SetOriUnits( ePDIoriUnits eO )
{
	g_pdiDev.SetPNOOriUnits( eO );
	AddResultMsg(_T("SetPNOOriUnits") );

	g_ePNOOriUnits = eO;
}

VOID SetPosUnits( ePDIposUnits eP )
{
	g_pdiDev.SetPNOPosUnits( eP );
	AddResultMsg(_T("SetPNOPosUnits") );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BOOL Connect( VOID )
{
    tstring msg;
    if (!(g_pdiDev.CnxReady()))
    {
 		g_pdiDev.ConnectG4( g_G4CFilePath );

		msg = _T("ConnectG4") + tstring( g_pdiDev.GetLastResultStr() ) + _T("\r\n");

        g_bCnxReady = g_pdiDev.CnxReady();
		AddMsg( msg );		
	}
    else
    {
        msg = _T("Already connected\r\n");
        g_bCnxReady = TRUE;
	    AddMsg( msg );
    }

	return g_bCnxReady;
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
VOID Disconnect()
{
    tstring msg;
    if (!(g_pdiDev.CnxReady()))
    {
        //msg = _T("Already disconnected\r\n");
    }
    else
    {
        g_pdiDev.Disconnect();
        msg = _T("Disconnect result: ") + tstring(g_pdiDev.GetLastResultStr()) + _T("\r\n");
		AddMsg( msg );
    }

}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
BOOL SetupDevice( VOID )
{
	g_pdiDev.SetPnoBuffer( g_pMotionBuf, BUFFER_SIZE );
	AddResultMsg(_T("SetPnoBuffer") );

	g_pdiDev.StartPipeExport();
	AddResultMsg(_T("StartPipeExport") );

	UpdateStationMap();

	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
VOID UpdateStationMap( VOID )
{
	g_pdiDev.GetStationMap( g_dwStationMap );
	AddResultMsg(_T("GetStationMap") );

	TCHAR szMsg[100];
	_stprintf_s(szMsg, _countof(szMsg), _T("ActiveStationMap: %#x\r\n"), g_dwStationMap );

	AddMsg( tstring(szMsg) );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
VOID AddMsg( tstring & csMsg )
{
	tcout << csMsg ;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
VOID AddResultMsg( LPCTSTR szCmd )
{
	tstring msg;

	//msg.Format("%s result: %s\r\n", szCmd, m_pdiDev.GetLastResultStr() );
	msg = tstring(szCmd) + _T(" result: ") + tstring( g_pdiDev.GetLastResultStr() ) + _T("\r\n");
	AddMsg( msg );
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
VOID ShowMenu( VOID )
{

	tcout << _T("\n\nPlease enter select a data command option: \n\n");
	tcout << _T("C or c:  Continuous Motion Data Display\n");
	tcout << _T("P or p:  Single Motion Data Frame Display\n");
	tcout << _T("H or h:  Update Hub/Station Map\n");
	tcout << _T("O or o:  Options Menu\n");
	tcout << endl;
	tcout << _T("ESC:     Quit\n");
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
eMenuChoice Prompt( VOID )
{
	eMenuChoice eRet = CHOICE_NONE;
	INT			ch;

	do
	{
		tcout << _T("\nCc/Pp/Hh/Oo/ESC>> ");
		ch = _getche();
		ch = toupper( ch );

		switch (ch)
		{
		case 'C':
			eRet = CHOICE_CONT;
			break;
		case 'P':
			eRet = CHOICE_SINGLE;
			break;
		case 'H':
			eRet = CHOICE_HUBMAP;
			break;
		case 'O':
			eRet = CHOICE_OPTIONS;
			break;
		case ESC: // ESC
			eRet = CHOICE_QUIT;
			break;
		default:
			break;
		}
	} while (eRet == CHOICE_NONE);

	return eRet;
}



/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
BOOL StartCont( VOID )
{
	BOOL bRet = FALSE;


	if (g_dwStationMap==0)
	{
		UpdateStationMap();
	}

	g_pdiDev.ResetHostFrameCount();
	g_dwLastHostFrameCount = 0;

	if (!(g_pdiDev.StartContPnoG4(0)))
	{
	}
	else
	{
		bRet = TRUE;
		Sleep(1000);  
	}
	AddResultMsg(_T("\nStartContPnoG4") );

	return bRet;
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
BOOL StopCont( VOID )
{
	BOOL bRet = FALSE;

	if (!(g_pdiDev.StopContPno()))
	{
	}
	else
	{
		bRet = TRUE;
		Sleep(1000);
	}
	AddResultMsg(_T("StopContPno") );

	return bRet;
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
VOID DisplayCont( VOID )
{
	BOOL bExit = FALSE;

	//tcout << _T("\nPress any key to start continuous display\r\n");
	//tcout << _T("\nPress ESC to stop.\r\n");
	//tcout << _T("\nReady? ");
	//_getche();
	//tcout << endl;

	INT  count = 10;
	PBYTE pBuf;
	DWORD dwSize;
	DWORD dwFC;

	do
	{
		if (!(g_pdiDev.LastHostFrameCount( dwFC )))
		{
			AddResultMsg(_T("LastHostFrameCount"));
			bExit = TRUE;
		}
		else if (dwFC == g_dwLastHostFrameCount)
		{
			// no new frames since last peek
		}
		else if (!(g_pdiDev.LastPnoPtr( pBuf, dwSize )))
		{
			AddResultMsg(_T("LastPnoPtr"));
			bExit = TRUE;
		}
		else if ((pBuf == 0) || (dwSize == 0))
		{
		}
		else 
		{
			g_dwLastHostFrameCount = dwFC;
			ParseG4NativeFrame( pBuf, dwSize );
		}

		if ( _kbhit() && ( _getch() == ESC ) ) 
		{
			bExit = TRUE;
		}

		if (count == 0)
			bExit = TRUE;

	} while (!bExit);

}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
VOID DisplaySingle( VOID )
{
	PBYTE pBuf;
	DWORD dwSize;

	cout << endl;

	if (g_dwStationMap==0)
	{
		UpdateStationMap();
	}

	if (!(g_pdiDev.ReadSinglePnoBufG4(pBuf, dwSize)))
	{
		AddResultMsg(_T("ReadSinglePno") );
	}
	else if ((pBuf == 0) || (dwSize == 0))
	{
	}
	else 
	{
		ParseG4NativeFrame( pBuf, dwSize );
	}
}
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
//typedef struct {				// per sensor P&O data
//	UINT32 nSnsID;				//4
//	float pos[3];				//12
//	float ori[4];				//16
//}*LPG4_SENSORDATA,G4_SENSORDATA;//32 bytes
//
//typedef struct {				// per hub P&O data
//	UINT32 nHubID;								//4
//	UINT32 nFrameCount;							//4
//	UINT32 dwSensorMap;							//4
//	UINT32 dwDigIO;								//4
//	G4_SENSORDATA sd[G4_MAX_SENSORS_PER_HUB]; //96
//}*LPG4_HUBDATA,G4_HUBDATA;	//	112 bytes

void ParseG4NativeFrame( PBYTE pBuf, DWORD dwSize )
{
	if ((!pBuf) || (!dwSize))
	{}
	else
	{
		DWORD i= 0;
		LPG4_HUBDATA	pHubFrame;

		while (i < dwSize )
		{
			pHubFrame = (LPG4_HUBDATA)(&pBuf[i]);

			i += sizeof(G4_HUBDATA);

			UINT	nHubID = pHubFrame->nHubID;
			UINT	nFrameNum =  pHubFrame->nFrameCount;
			UINT	nSensorMap = pHubFrame->dwSensorMap;
			UINT	nDigIO = pHubFrame->dwDigIO;

			UINT	nSensMask = 1;

			TCHAR	szFrame[800];

			for (int j=0; j<G4_MAX_SENSORS_PER_HUB; j++)
			{
				if (((nSensMask << j) & nSensorMap) != 0)
				{
					G4_SENSORDATA * pSD = &(pHubFrame->sd[j]);
					if (g_ePNOOriUnits == E_PDI_ORI_QUATERNION)
					{
						_stprintf_s( szFrame, _countof(szFrame), _T("%3d %3d|  %05d|  0x%04x|  % 7.3f % 7.3f % 7.3f| % 8.4f % 8.4f % 8.4f % 8.4f\r\n"),
								pHubFrame->nHubID, pSD->nSnsID,
								pHubFrame->nFrameCount, pHubFrame->dwDigIO,
								pSD->pos[0], pSD->pos[1], pSD->pos[2],
								pSD->ori[0], pSD->ori[1], pSD->ori[2], pSD->ori[3] );
					}
					else
					{
						_stprintf_s( szFrame, _countof(szFrame), _T("%3d %3d|  %05d|  0x%04x|  % 7.3f % 7.3f % 7.3f| % 8.3f % 8.3f % 8.3f\r\n"),
								pHubFrame->nHubID, pSD->nSnsID,
								pHubFrame->nFrameCount, pHubFrame->dwDigIO,
								pSD->pos[0], pSD->pos[1], pSD->pos[2],
								pSD->ori[0], pSD->ori[1], pSD->ori[2] );
					}
					AddMsg( tstring(szFrame) );
				}
			}

		} // end while dwsize
	}
}

VOID DisplayFrame( PBYTE pBuf, DWORD dwSize )
{
	TCHAR	szFrame[200];
	DWORD	i = 0;

    while ( i<dwSize)
    {
		FT_BINHDR *pHdr = (FT_BINHDR*)(&pBuf[i]);
	    CHAR cSensor = pHdr->station;
		SHORT shSize = 6*(sizeof(FLOAT));

        // skip rest of header
        i += sizeof(FT_BINHDR);

		PFLOAT pPno = (PFLOAT)(&pBuf[i]);

		_stprintf_s( szFrame, _countof(szFrame), _T("%2c   %+011.6f %+011.6f %+011.6f   %+011.6f %+011.6f %+011.6f\r\n"), 
			     cSensor, pPno[0], pPno[1], pPno[2], pPno[3], pPno[4], pPno[5] );

		AddMsg( tstring( szFrame ) );

		i += shSize;
	}
}

