## trackr-polhemus-g4

## About
This repository contains complete code for working with the [Polhemus G4](http://polhemus.com/motion-tracking/all-trackers/g4) tracking system on Microsoft Windows **and** Linux.  It utilizes the proprietary SDK provided by Polhemus.

The general purpose of this libary is to provide really simple and easy access to tracking data from a variety of langauges, sheilding the programmer from as much of the complexity and confusion that comes with interacting with the Polhemus SDK as possible.  Where tradeoffs between flexibility and ease of use have arisen, ease of use has won.

## Usage
The G4 is accessed through a client-server model.  The server is written in C++ and is launched separately from the application you intend to use the tracking data with.  The server code connects to the G4 tracking system and continuously polls the tracker for position and orientation data.  At time of this writing, only one hub, and reports all three sensors on that hub.

The server listens on port 1986 for incoming connections over TCP.  At time of this writing, multiple simultaneous connections are *not* supported.  Source code for the server is in `/server`, and contains Windows and Linux implementations.  Note, to run these servers you must also install the Polhemus SKD.  The Linux SDK is provided within the `server/linux/linux_sdk` folder.  The Windows SDK is available [here](ftp://ftp.polhemus.com/pub/Standalone/SWD-PS08-08-4.3.0-PISDKSetup.exe)

### Client requests
Requests are simple a binary chunk of three booleans - indicating if sensor data for sensor 0, 1, and 2 are requested.  

In C++/C, you could use a struct like this:
```cpp
struct po_req {
	bool sensor_0;
	bool sensor_1;
	bool sensor_2;
};
```
You can write that struct directly to a socket connected to the trackr server.  In other languages, you just need to make sure you are sending the raw boolean triple.  For example, in node, you could do the folling:

```js

var request = Buffer([true, true, true]); // request all three sensor position/orientation
socket.write(request);
```

### Server responses
Server responses are stringsof ascii text, with fields delimited by | characters

```
Field 0:  Frame number (incrementing number from G4 hub)

Field 1:   Sensor number (1)
Field 2:   Sensor 1 X position (meters)
Field 3:   Sensor 1 Y position (meters)
Field 4:   Sensor 1 Z position (meters)
Field 5:   Sensor 1 X-rotation / pitch (degrees)
Field 6:   Sensor 1 Y-rotation / yaw (degrees)
Field 7:   Sensor 1 z-rotation / roll (degrees)

Field 8:   Sensor number (2)
Field 9:   Sensor 2 X position (meters)
Field 10:  Sensor 2 Y position (meters)
Field 11:  Sensor 2 Z position (meters)
Field 12:  Sensor 2 X-rotation / pitch (degrees)
Field 13:  Sensor 2 Y-rotation / yaw (degrees)
Field 14:  Sensor 2 z-rotation / roll (degrees)

Field 15:  Sensor number (3)
Field 16:  Sensor 3 X position (meters)
Field 17:  Sensor 3 Y position (meters)
Field 18:  Sensor 3 Z position (meters)
Field 19:  Sensor 3 X-rotation / pitch (degrees)
Field 20:  Sensor 3 Y-rotation / yaw (degrees)
Field 21:  Sensor 3 z-rotation / roll (degrees)
```
## Examples
Withing the `/client` there are several examples for you to build on

- cpp (windows and linux) - creates a simple socket connection and outputs the server responses
- C# - creates a simple socket connection and outputs the server responses
- js - Contains two folders - `g4` and `nodewebkit`.  The `g4` folder is the source code for the npm module (also available though `npm install g4` - see [details](https://www.npmjs.com/package/g4)) which connects to a local server and can be used to provide tracking data to a nodejs application.  The nodewebkit folder contains an [nw](http://nwjs.io/) application that uses the g4 module to display tracked cars using [Three.js](http://threejs.org/)
- unity - a set of scripts that can be added to a unity project to add tracking data
