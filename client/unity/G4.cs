using UnityEngine;
using System.Collections;
using System.Collections.Generic;
using System.IO;
using System.Net.Sockets;
using System.Text;
using System.Net;


public class G4 : MonoBehaviour {
	private Socket socket;
	public Vector3 [] sensor_position;
	public Quaternion [] sensor_orientation;
	private byte[] po_request;
	private byte[] bytes;
	
	void Start()
	{
		// Initialization
		po_request = new byte[3] { System.Convert.ToByte(true), System.Convert.ToByte(true), System.Convert.ToByte(true) };
		bytes = new byte[1024];
		sensor_position = new Vector3[3];
		sensor_orientation = new Quaternion[3];
		sensor_position[0] = new Vector3();
		sensor_position[1] = new Vector3();
		sensor_position[2] = new Vector3();
		
		// Connect to the trackr server (localhost at port 1986)
		IPEndPoint trackr = buildEndpoint();
		socket = new Socket(AddressFamily.InterNetwork, SocketType.Stream, ProtocolType.Tcp );
		socket.Connect(trackr);
	}
	
	

	void Update()
	{
		// poll the server
		int bytesSent = socket.Send(po_request);
		
		// Receive the response from the trackr server.
		int bytesRec = socket.Receive(bytes);
		string sensor_data_string = Encoding.ASCII.GetString( bytes, 0, bytesRec );
		
		// parse the response string to populate sensor data.
		process_trackr_response(sensor_data_string);
	}

	/*//////////////////////////////////////////////////////
	The Polhemus G4 is returning X/Y/Z positions/orientations
	in a Right-Handed coordinate system.  Unity works in a Left-Handed
	coordinate system.  We flip the sign of X to fix this for positions.
			
	For orientations, its a bit more complicated.  See documentation 
	in build_quaternion_from_g4_euler method.
	*///////////////////////////////////////////////////////
	void process_trackr_response(string response) {
		string [] tokens = response.Split(new System.Char [] {'|'});
		
		
		for ( int sensor_i = 0, i = 0; i < tokens.Length; i+= 8, sensor_i++ ) {
			// first two entries of sensor data are frame # and sensor.
			// since we've hardcoded the request for ALL 3 sensors, we'll
			// just ignore these fields.  In the future probably a good
			// idea to add support for this.
			sensor_position[sensor_i].x = - (float) System.Double.Parse (tokens[i + 2].Trim());
			sensor_position[sensor_i].y = (float) System.Double.Parse (tokens[i + 3].Trim());
			sensor_position[sensor_i].z = (float) System.Double.Parse (tokens[i + 4].Trim());
					
			sensor_orientation[sensor_i] = build_quaternion_from_g4_euler(
				(float) (System.Double.Parse (tokens[i + 5].Trim()) * 180 / System.Math.PI), 
				(float) (System.Double.Parse (tokens[i + 6].Trim()) * 180 / System.Math.PI), 
				(float) (System.Double.Parse (tokens[i + 7].Trim()) * 180 / System.Math.PI));	
		}
	}
	
	/*///////////////////////////////////////////////////////
	The Euler angles (radians) returns by G4 system is meant 
	to be performed in a ZYX order.  Unity uses ZXY order so
	we cannot just create the quaternion using their build in 
	Quaternion constructor. 
			
	First build the quaternion using the custom order.  At the 
	same time, we also need to account for the handedness. Again, 
	Polhemus is using right-handed, Unity uses left handed.  The
	switch is achieved by negating the z and y axis of rotation.
	*////////////////////////////////////////////////////////
	Quaternion build_quaternion_from_g4_euler(float xAngle, float yAngle, float zAngle) {
		Quaternion q =  
			Quaternion.AngleAxis(zAngle, Vector3.back) *
			Quaternion.AngleAxis(yAngle, Vector3.down) *
			Quaternion.AngleAxis(xAngle, Vector3.right);
		return q;	
	}
	
	IPEndPoint buildEndpoint() {
		IPHostEntry ipHostInfo = Dns.GetHostEntry(Dns.GetHostName());
		IPAddress ipAddress = null;
		foreach (IPAddress ip in ipHostInfo.AddressList) {
			if (ip.AddressFamily == AddressFamily.InterNetwork) ipAddress = ip;
		}
		return new IPEndPoint(ipAddress,1986);
	}

	void OnApplicationQuit()
	{
		if( gameObject.GetComponent<G4>().enabled && socket != null )
		{
			socket.Close();
		}
	}
}
