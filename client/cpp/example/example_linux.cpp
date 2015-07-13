
#include <stdlib.h>
#include <stdio.h>
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 


#define G4_SOCKET_PORT 1986
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

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char **argv) 
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
	char recv_buffer[1000];
	
	struct po_req request;
	request.sensor_0 = true;
	request.sensor_1 = true;
	request.sensor_2 = false;
    char *sendbuf = (char*)&request;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(TRACKR_HOST);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(G4_SOCKET_PORT);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
    


	int sends = 0;
	while (1) {
		// Send an initial buffer
		int n = write( sockfd, sendbuf, sizeof(po_req));
		if (n < 0 ) {
			printf("send failed with error\n");
			return 1;
		}
		sends++;
		
		do {
			n = read(sockfd, recv_buffer, 1000);
			if ( n > 0 ) {
				recv_buffer[n] = '\0';
				std::vector<po_sample> samples = make_samples(recv_buffer);
				printf("%3.3f %3.3f %3.3f\t%3.3f %3.3f %3.3f\n", 
					samples[0].pos[0],samples[0].pos[1], samples[0].pos[2],
					samples[1].pos[0],samples[1].pos[1], samples[1].pos[2]);
			}
			else if ( n == 0 )
				printf("All data recieved\n");
			else
				printf("recv failed with error:\n");
		} while (n > 0 && n < sizeof(po_sample));
	}
   

    // shutdown the connection since no more data will be sent
    close(sockfd);
    
    return 0;
}
