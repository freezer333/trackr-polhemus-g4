using UnityEngine;
using System.Collections;

public class G4Sensor : MonoBehaviour {

	public enum SensorNumber
	{
		OFF = -1,
		SENSOR1 = 0,
		SENSOR2 = 1,
		SENSOR3 = 2
	};

	public SensorNumber m_sensorNumber = SensorNumber.OFF;
	
	private G4 g4;
	
	
	
	void Start()
	{
		Component[] all_g4s = Object.FindObjectsOfType<G4>();

		// Make sure the scene contains only one instance of the G4
		if( all_g4s.Length != 1 )
		{
			Debug.LogError( "The scene <b>must</b> contain a single instance of the G4" +
			               " for the sensors to work." );
			Debug.Break();
			g4 = null;
		}
		else {
			// Grab the G4 Manager Script for use
			g4 = (G4)all_g4s[0];

			// Warn the user if the sensor is turned off (make a best attempt at quelling unneeded frustration)
			if( m_sensorNumber == SensorNumber.OFF )
			{
				Debug.LogWarning( "A G4Sensor's Sensor Number is set to \"OFF\". This means the script is not" +
					" processing any sensor data." );
			}
			
			Debug.Log ("Sensor # " + ( (int) m_sensorNumber) + " is being used");
		}
	}
		
	
	void Update()
	{
		try {
			if ( g4 != null ) {
				transform.position = g4.sensor_position[(int) m_sensorNumber];
				transform.rotation = g4.sensor_orientation[(int) m_sensorNumber];
			}
		}
		catch {
			Debug.LogError("Could not get sensor p&o data for sensor " + ( (int) m_sensorNumber));
		}
	}
}
