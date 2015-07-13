#include "trackr.h"

#include "G4TrackIncl.h"
#include <thread>
#include <iostream>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstdlib>
#include <cstdio>
#include <string>
#include <cstring>
#include <signal.h>
#include <sys/poll.h>

using namespace std;

bool polling_active = true;
bool serving_active = true;
bool trackr_online = false;
struct po_sample sensor_p_o_records[G4_MAX_SENSORS_PER_HUB];

void error(const char *msg)
{
	perror(msg);
    polling_active = false;
}


void append_po(po_sample sample, int sensor, char buffer[], int bufferSize, bool last) {
	char tmp [512];
	sprintf(tmp, "%i|%i|%f|%f|%f|%f|%f|%f",
				sample.frame_number, sensor, 
				sample.pos[0], sample.pos[1], sample.pos[2], 
				sample.ori[0], sample.ori[1], sample.ori[2]);
	strncat(buffer, tmp, bufferSize);
	if (!last ) {
		strncat(buffer, "|", bufferSize);
	}
}
void sample_to_string(po_sample sample[], po_req request, char buffer[], int bufferSize) {
	memset(buffer, 0, bufferSize);
	if ( request.sensor_0 ) append_po(sample[0], 1, buffer, bufferSize, !request.sensor_1 && !request.sensor_2);
	if ( request.sensor_1 ) append_po(sample[1], 2, buffer, bufferSize, !request.sensor_2);
	if ( request.sensor_2 ) append_po(sample[2], 3, buffer, bufferSize, true);
}


void server() {
	
	struct po_sample sample[3];
	struct po_req request;
	int sockfd, newsockfd, portno;
    socklen_t clilen;
    char buffer[1024];
    struct sockaddr_in serv_addr, cli_addr;
    int n;
    struct pollfd fd;
    memset(&fd, 0 , sizeof(fd));

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
       error("ERROR opening socket");
       return;
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(G4_SOCKET_PORT);
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    	error("ERROR on binding");
    	return;
    }
             
    listen(sockfd,5);
    clilen = sizeof(cli_addr);

    

    fd.fd = sockfd;
 	fd.events = POLLIN;

    // Now just wait for connections and serve the connection.  We are not supporting multiple clients
    // simultaneously just yet...
   	int timeouts = 0;
    while (serving_active) {
     	
     	if ( timeouts == 0 ) cout << "Accepting connections on port " << G4_SOCKET_PORT << endl;
   
   		int rc = poll(&fd, 1, 1000);
    	
	    /***********************************************************/
	    /* Check to see if the poll call failed.                   */
	    /***********************************************************/
	    if (rc < 0)
	    {
	      error("poll() failed");
	      return;
	    }

	    /***********************************************************/
	    /* Check to see if the 3 minute time out expired.          */
	    /***********************************************************/
	    else if (rc > 0) {
	    	timeouts =0;
	    	cout << "\tClient connected, serving requests..." << endl;
	    	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		    if (newsockfd < 0)  {
		      	error("ERROR on accept");
		    }
		    else {
		    	bool active = true;
		    	do {
					int n = read(newsockfd, (char *)&request, sizeof(request));
					if (n > 0) {
						//update_mutex.lock();
						memset(&sample, 0, sizeof(po_sample)*3);
						//memcpy(&sample, sensor_p_o_records, sizeof(po_sample)*3);
						//update_mutex.unlock();
						sample_to_string(sample, request, buffer, 1024);
						n = write( newsockfd, buffer, strnlen(buffer, 1024));
						if (n != strnlen(buffer, 1024)) {
							printf("send failed with error\n");
							active = false;
						}
					}
					else  {
						printf("\tSocket (read) timedout\n");
						active = false;
					}
				} while (active);
		    	close(newsockfd);
		    }

		    cout << "\tClient disconnected." << endl;
		    
		}
		else {
			timeouts++;
		}
    }
    
    close(sockfd);
    // shutdown the tracker
	polling_active = false;
	serving_active = false;
    
    return; 
}

int get_hubs(int sysId) {
	G4_CMD_STRUCT cs;
	
	cs.cmd=G4_CMD_GET_ACTIVE_HUBS;
  	cs.cds.id=G4_CREATE_ID(sysId,0,0);
  	cs.cds.action=G4_ACTION_GET;
  	cs.cds.pParam=NULL;

  	int attempts = 0;

  	// After g4_init_sys, the tracker won't actually respond with hubs for few moments.
  	// Unfortunately the G4 SDK hoses interrupts, so we can't just sleep. See we'll
  	// keep asking the G4 how many hubs are connected till we get the answer we want :)
  	while (!trackr_online ) {
  		int res = g4_set_query(&cs);
	  	if ( res != G4_ERROR_NONE ) {
	  		cerr << "Error querying for hubs" << endl;
	  		return -1;
	  	}
	  	attempts++;
	  	if (cs.cds.iParam < 1 ) {
	  		if ( attempts % 100000 == 0) {
	  			cout << "Waiting for hubs to come online... " << endl;
	  		}
	  	}
	  	else {
	  		trackr_online = true;
	  	}
  	}
 	return cs.cds.iParam;
}

void run_tracker() {
	const char* src_file=G4C_PATH;
	int sysId;
	int res;

	res=g4_init_sys(&sysId,src_file,NULL);
	if (res!=G4_ERROR_NONE) {
		cerr << "Connection to g4 failed - " << res << endl;
		return;
	}
	cout << "Connected Successfully to G4 - system id = " << sysId << endl;
	int hubs = get_hubs(sysId);
	cout << "Found " << hubs << " hubs." << endl;

	while (polling_active){
		// poll and update the common variable
		;
	}
	g4_close_tracker();
	cout << "Trackr stopped" << endl;
}


void stop_polling(int s){
    printf("Stopping trackr on signal %d\n",s);
    polling_active = false;
    serving_active = false;
}

int main() {
	struct sigaction sigIntHandler;

	sigIntHandler.sa_handler = stop_polling;
	sigemptyset(&sigIntHandler.sa_mask);
	sigIntHandler.sa_flags = 0;

	sigaction(SIGINT, &sigIntHandler, NULL);
	thread server_thread(server);
	run_tracker();
	server_thread.join();
}	

