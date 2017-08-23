#include "MCodeMotor.hpp"		
#include "Lidar.hpp"
#include "Scanner.hpp"
#include "Camera.hpp"
#include "Socket.hpp"
#include "main.hpp"

#include <iostream>				// used for basic input output services
#include <string>				
#include <thread>
#include <chrono>				// helps for measuring duration and clocks
#include <fstream>				// helps to read and write to files

using namespace std;

const unsigned int listeningPort = 500;		// listening port from Jetson TX1 as this is the server 

const string motorIpAddress = "192.168.33.1";			// motor ip address
const int motorPort = 503;								// motor port 
const string lidarIpAddress = "192.168.0.10";			// lidar ip address
const int lidarPort = 10940;							// lidar port

Socket serverSocket;

int main() 
{	//string commandReceived;
/*
	// get connection from Jetson
	cout << "Listening for connection" << endl;
	if  (serverSocket.hostServer(listeningPort))
	{
		cout << "Connected to Jetson" <<endl;		
	}
	else
	{
		cout << "Could not connect to Jetson" << endl;
		
	}
	
	if (serverSocket.acceptConnection())
	{
		cout << "accepted" << endl;
	}
	else
	{
		cout << "not accepted" << endl;
	}
	commandReceived = serverSocket.receiveString ();
	cout << commandReceived <<endl;
*/
	Camera11 camera1;										// intializing object of class camera

	//camera1.cam_init();
	
	MCodeMotor motor(motorIpAddress, motorPort);			// initializing object of class MCodeMotor

	if (!motor.connect())									// checking for motor connection
	{
		return 1;
	}

	Lidar lidar(lidarIpAddress, lidarPort);					// initializing object of class Lidar
	if (!lidar.connect())									// checking for lidar connection
	{
		return 1;
	}

	motor.initializeSettings();								// initialize motor settings 

	if (!motor.homeToIndex())								// check if motor is positioned at home location. Home is defined as the position parallel to the motor alignment
	{
		return 1;
	}

	Scanner scanner(lidar, motor);							// initialize object of class Scanner

	camera1.cam_init();										// take a picture using camera

	while(1)
	
		// Wait for user input to select captureing mode
		scanner.contScan();											// start scanning using the Lidar at specified angle and speed

		motor.moveAngleAbsolute(0);								// after scanning bring back the motor to home location

		vector<Scanner::DataPoint> data = scanner.getLidarData(); 	// get the data by scanning from the Lidar

		string filename = "points.pcd";
		savePCD(data, filename);

		cout << "done!" << endl;	
	}	

	return 0;

}

