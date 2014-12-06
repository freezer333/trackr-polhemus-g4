
#pragma once
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#include <SDKDDKVer.h>
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <tchar.h>
#include <conio.h>
#include <windows.h>
#include <wincon.h>


#include <string>
#include <mutex>
#include <thread>
#include <iostream>
#include "PDI.h"
#include "server.h"
#include "trackr.h"

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
DWORD		last_frame_number;
ePDIoriUnits g_ePNOOriUnits;



/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////


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
BOOL		StartCont			( VOID );
BOOL		StopCont			( VOID );	
void poll();
VOID		ParseG4NativeFrame	( PBYTE, DWORD );

#define BUFFER_SIZE 0x1FA400   // 30 seconds of xyzaer+fc 8 sensors at 240 hz 
//BYTE	g_pMotionBuf[0x0800];  // 2K worth of data.  == 73 frames of XYZAER
BYTE	g_pMotionBuf[BUFFER_SIZE]; 
TCHAR	g_G4CFilePath[_MAX_PATH+1];

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////

mutex update_mutex;
struct po_sample sensor_p_o_records[G4_MAX_SENSORS_PER_HUB];


void sample_to_string(po_sample sample, char buffer[], int bufferSize) {
	sprintf_s(buffer,bufferSize, "%i|%f|%f|%f|%f|%f|%f|%f",
		sample.frame_number, sample.pos[0], sample.pos[1], sample.pos[2], 
		sample.ori[0], sample.ori[1], sample.ori[2], sample.ori[3]);
}

void run_server() {
	struct po_sample sample;
	struct po_req request;
	SOCKET ClientSocket = INVALID_SOCKET;
	char buffer[100];
	int iSendResult ;
	while ( 1 ) {
		cout << "Listening on " << G4_SOCKET_PORT << endl;
		SOCKET ListenSocket = build_listen_socket();
		int iResult = listen(ListenSocket, SOMAXCONN);
		if (iResult == SOCKET_ERROR) {
			printf("listen failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
		   return;
		}

		while (1) {

    		ClientSocket = accept(ListenSocket, NULL, NULL);
			if (ClientSocket == INVALID_SOCKET) {
				printf("accept failed with error: %d\n", WSAGetLastError());
				closesocket(ListenSocket);
				WSACleanup();
				return;
			}

			bool active = true;

			// Receive until the peer shuts down the connection
			do {

				iResult = recv(ClientSocket, (char *)&request, sizeof(request), 0);
				if (iResult > 0) {
					update_mutex.lock();
					memcpy(&sample, &(sensor_p_o_records[0]), sizeof(po_sample));
					update_mutex.unlock();

					sample_to_string(sample, buffer, 100);
					iSendResult = send( ClientSocket, buffer, strnlen(buffer, 100), 0 );
					if (iSendResult == SOCKET_ERROR) {
						printf("send failed with error: %d\n", WSAGetLastError());
						closesocket(ClientSocket);
						active = false;
					}
				}
				else  {
					printf("recv failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					active = false;
				}

			} while (active);

		
			 // cleanup
			closesocket(ClientSocket);
		
			printf("client socket shutdown\n");
		}

		// No longer need server socket
		closesocket(ListenSocket);
		WSACleanup();
	}
}




/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
int main()
{
	Initialize();
	if (!Connect() ) {
		std::cerr << "Error connecting to g4 tracker" << endl;
		return CONNECT_FAILURE;
	}
	else if (!SetupDevice()) {
		std::cerr << "Error setting up g4 tracker" << endl;
		return SETUP_FAILURE;
	}
	else if (!StartCont() ) {
		std::cerr << "Error initiating continous polling of g4 tracker" << endl;
		return START_POLLING_FAILURE;
	}
	
	cout << "g4 server started, connect socket at port " << G4_SOCKET_PORT << endl;
	std::thread t1(run_server);
	poll();
	StopCont();
	Disconnect();
	cout << "g4 server shutdown" << endl;
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

	g_pdiDev.SetPNOOriUnits( g_ePNOOriUnits );

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
	last_frame_number = 0;

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
void poll()
{
	bool exit = false;

	PBYTE sample;
	DWORD sample_buffer_size;
	DWORD current_frame_count;

	do
	{
		if (!(g_pdiDev.LastHostFrameCount( current_frame_count )))
		{
			AddResultMsg(_T("LastHostFrameCount"));
			exit = true;
		}
		else if (current_frame_count == last_frame_number)
		{
			// no new frames since last peek
		}
		else if (!(g_pdiDev.LastPnoPtr( sample, sample_buffer_size )))
		{
			AddResultMsg(_T("LastPnoPtr"));
			exit = true;
		}
		else if ((sample == 0) || (sample_buffer_size == 0))
		{
			// no sample was obtained...
		}
		else 
		{
			last_frame_number = current_frame_count;
			ParseG4NativeFrame( sample, sample_buffer_size );
		}

		if ( _kbhit() && ( _getch() == ESC ) ) 
		{
			exit = true;
		}

	} while (!exit);

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
	
	if (!pBuf || !dwSize)
	{ 
		return;
	}
	DWORD i= 0;
	LPG4_HUBDATA	pHubFrame;
	while (i < dwSize ) {
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
					
				if ( update_mutex.try_lock() ) {
					if ( PRINT_RECORDS ) {
						_stprintf_s( szFrame, _countof(szFrame), _T("%3d %3d|  %05d|  0x%04x|  % 7.3f % 7.3f % 7.3f| % 8.3f % 8.3f % 8.3f\r\n"),
								pHubFrame->nHubID, pSD->nSnsID,
								pHubFrame->nFrameCount, pHubFrame->dwDigIO,
								pSD->pos[0], pSD->pos[1], pSD->pos[2],
								pSD->ori[0], pSD->ori[1], pSD->ori[2] );
						AddMsg( tstring(szFrame) );
					}

					sensor_p_o_records[pSD->nSnsID].frame_number = pHubFrame->nFrameCount;
					sensor_p_o_records[pSD->nSnsID].pos[0] = pSD->pos[0];
					sensor_p_o_records[pSD->nSnsID].pos[1] = pSD->pos[1];
					sensor_p_o_records[pSD->nSnsID].pos[2] = pSD->pos[2];
					sensor_p_o_records[pSD->nSnsID].ori[0] = pSD->ori[0];
					sensor_p_o_records[pSD->nSnsID].ori[1] = pSD->ori[1];
					sensor_p_o_records[pSD->nSnsID].ori[2] = pSD->ori[2];
					sensor_p_o_records[pSD->nSnsID].ori[3] = pSD->ori[3];
					update_mutex.unlock();
				}
			}
		}
	} // end while dwsize
}
