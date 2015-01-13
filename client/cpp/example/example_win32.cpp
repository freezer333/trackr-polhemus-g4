#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>


// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")


#define G4_SOCKET_PORT "1986"
#define TRACKR_HOST "localhost"

struct po_req {
	bool sensor_0;
	bool sensor_1;
	bool sensor_2;
};

struct po_sample {
	int frame_number;
	int sensor_number;
	float pos[3];
	float ori[3];
} ;

std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
    std::stringstream ss(s);
    std::string item;
    while (std::getline(ss, item, delim)) {
		if ( !item.empty() ) {
			elems.push_back(item);
		}
    }
    return elems;
}
std::vector<po_sample> make_samples(const std::string &s) {
	std::vector<std::string> tokens;
    split(s, '|', tokens);
	std::vector<po_sample> samples;
	for ( int i = 0; i < tokens.size(); i+= 8) {
		struct po_sample sample;
		sample.frame_number = stoi(tokens[i]);
		sample.sensor_number = stoi(tokens[i+1]);
		sample.pos[0] = stof(tokens[i+2]);
		sample.pos[1] = stof(tokens[i+3]);
		sample.pos[2] = stof(tokens[i+4]);
		sample.ori[0] = stof(tokens[i+5]);
		sample.ori[1] = stof(tokens[i+6]);
		sample.ori[2] = stof(tokens[i+7]);
		samples.push_back(sample);
	}
    return samples;
}


int __cdecl main(int argc, char **argv) 
{
	char recv_buffer[1000];
	
	struct po_req request;
	request.sensor_0 = true;
	request.sensor_1 = true;
	request.sensor_2 = false;

    WSADATA wsaData;
    SOCKET ConnectSocket = INVALID_SOCKET;
    struct addrinfo *result = NULL,
                    *ptr = NULL,
                    hints;
    char *sendbuf = (char*)&request;
   
	
    int iResult;
    

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2,2), &wsaData);
    if (iResult != 0) {
        printf("WSAStartup failed with error: %d\n", iResult);
        return 1;
    }

    ZeroMemory( &hints, sizeof(hints) );
    hints.ai_family = AF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    // Resolve the server address and port
    iResult = getaddrinfo(TRACKR_HOST, G4_SOCKET_PORT, &hints, &result);
    if ( iResult != 0 ) {
        printf("getaddrinfo failed with error: %d\n", iResult);
        WSACleanup();
        return 1;
    }

    // Attempt to connect to an address until one succeeds
    for(ptr=result; ptr != NULL ;ptr=ptr->ai_next) {
        // Create a SOCKET for connecting to server
        ConnectSocket = socket(ptr->ai_family, ptr->ai_socktype, 
            ptr->ai_protocol);
        if (ConnectSocket == INVALID_SOCKET) {
            printf("socket failed with error: %ld\n", WSAGetLastError());
            WSACleanup();
            return 1;
        }

        // Connect to server.
        iResult = connect( ConnectSocket, ptr->ai_addr, (int)ptr->ai_addrlen);
        if (iResult == SOCKET_ERROR) {
            closesocket(ConnectSocket);
            ConnectSocket = INVALID_SOCKET;
            continue;
        }
        break;
    }

    freeaddrinfo(result);

    if (ConnectSocket == INVALID_SOCKET) {
        printf("Unable to connect to server!\n");
        WSACleanup();
        return 1;
    }
	int sends = 0;
	while (1) {
		// Send an initial buffer
		iResult = send( ConnectSocket, sendbuf, sizeof(po_req), 0 );
		if (iResult == SOCKET_ERROR) {
			printf("send failed with error: %d\n", WSAGetLastError());
			closesocket(ConnectSocket);
			WSACleanup();
			return 1;
		}
		sends++;
		
		do {
			iResult = recv(ConnectSocket, recv_buffer, 1000, 0);
			if ( iResult > 0 ) {
				recv_buffer[iResult] = '\0';
				std::vector<po_sample> samples = make_samples(recv_buffer);
				printf("%3.3f %3.3f %3.3f\t%3.3f %3.3f %3.3f\n", 
					samples[0].pos[0],samples[0].pos[1], samples[0].pos[2],
					samples[1].pos[0],samples[1].pos[1], samples[1].pos[2]);
			}
			else if ( iResult == 0 )
				printf("All data recieved\n");
			else
				printf("recv failed with error: %d\n", WSAGetLastError());
		} while (iResult > 0 && iResult < sizeof(po_sample));
	}
   

     // shutdown the connection since no more data will be sent
    iResult = shutdown(ConnectSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ConnectSocket);
        WSACleanup();
        return 1;
    }


    // cleanup
    closesocket(ConnectSocket);
    WSACleanup();

    return 0;
}
