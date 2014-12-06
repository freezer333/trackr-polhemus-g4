#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")

#define G4_SOCKET_PORT "1986"

struct po_req {
	bool sensor_0;
	bool sensor_1;
	bool sensor_2;
};

struct po_sample {
	int frame_number;
	float pos[3];
	float ori[3];
} ;

int __cdecl main(void) 
{
    WSADATA wsaData;
    int iResult;
	struct po_sample sample;
	struct po_req request;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo *result = NULL;
    struct addrinfo hints;

    int iSendResult;
   
    
    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo(NULL, G4_SOCKET_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    if (ListenSocket == INVALID_SOCKET) {
        printf("socket failed with error: %ld\n", WSAGetLastError());
        freeaddrinfo(result);
        WSACleanup();
        return 1;
    }

    // Setup the TCP listening socket
    iResult = bind( ListenSocket, result->ai_addr, (int)result->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        printf("bind failed with error: %d\n", WSAGetLastError());
        freeaddrinfo(result);
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);
    if (iResult == SOCKET_ERROR) {
        printf("listen failed with error: %d\n", WSAGetLastError());
        closesocket(ListenSocket);
        WSACleanup();
        return 1;
    }

	while (1) {
    	ClientSocket = accept(ListenSocket, NULL, NULL);
		if (ClientSocket == INVALID_SOCKET) {
			printf("accept failed with error: %d\n", WSAGetLastError());
			closesocket(ListenSocket);
			WSACleanup();
			return 1;
		}

		bool active = true;

		// Receive until the peer shuts down the connection
		do {

			iResult = recv(ClientSocket, (char *)&request, sizeof(request), 0);
			if (iResult > 0) {
				//printf("Bytes received: %d\n", iResult);
				iSendResult = send( ClientSocket, (char *)&sample, sizeof(sample), 0 );
				if (iSendResult == SOCKET_ERROR) {
					printf("send failed with error: %d\n", WSAGetLastError());
					closesocket(ClientSocket);
					active = false;
				}
				//printf("Bytes sent: %d\n", iSendResult);
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


    return 0;
}