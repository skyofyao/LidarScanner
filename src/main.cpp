#include "MCodeMotor.hpp"		
#include "Lidar.hpp"
#include "Scanner.hpp"
#include "Camera.hpp"
#include "Socket.hpp"
#include "main.hpp"

#include <iostream>				// used for basic input output services
#include <sstream>
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

	/// Init sensors and motors

	Camera11 camera1;										// intializing object of class camera

	//camera1.cam_init();
	
	MCodeMotor motor(motorIpAddress, motorPort);			// initializing object of class MCodeMotor

	if (!motor.connect())									// checking for motor connection
	{
		std::cout<<"Motor not connected"<<std::endl;
		return 1;
	}

	Lidar lidar(lidarIpAddress, lidarPort);					// initializing object of class Lidar
	if (!lidar.connect())									// checking for lidar connection
	{
		std::cout<<"Lidar not connected""<<std::endl;
		return 1;
	}

	motor.initializeSettings();								// initialize motor settings 

	if (!motor.homeToIndex())								// check if motor is positioned at home location. Home is defined as the position parallel to the motor alignment
	{
		return 1;
	}

	Scanner scanner(lidar, motor);							// initialize object of class Scanner

	camera1.cam_init();										// take a picture using camera

	// Data acqusition loop
	while(1)
	{
		std::cout<<"Input parameters"<<endl;
		// Parameter format: 
		// "[char:scanType] [str:filename] [float:scan_size] [int:scan_lines] [float:lidarScanSize] [float:scan_center]"
		string input_str;
		getline(cin,input_str);
		stringstream str_stm = stringstream(input_str);
		// Parameters
		char scan_type; // 'O' for orginal, 'S' for step, 'C' for continuous, others for exit
		string filename_pfx;	// Filename prefix for the photos and the pcd file
		float scan_size;	// Motor movement size, in deg
		float scan_center;	// The middle angle of motor movement, in deg
		float lidar_scan_size; // The angle of lidar valid scan pixels, in deg
		int	 scan_lines; // How many lines to be scanned in the motor movement
		// Parse parameters
		str_stm >> scan_type;
		str_stm >> filename_pfx;
		string filename = "../photos/";
		switch(scan_type)
		{
			case 'O':
				std::cout<<"Origional scan"<<endl;
				
			break;
			case 'S':
				std::cout<<"step scan"<<endl;
				str_stm >> scan_size;
				str_stm >> scan_lines;
				str_stm >> lidar_scan_size;
				str_stm >> scan_center;
				std::cout<<"Parameters: "<<scan_size<<", " <<scan_lines<<", "<<lidar_scan_size<<", "<<scan_center<<endl;
				
				filename.append(filename_pfx);
				filename.append(".pcd");
				std::cout<<"file path"<<filename<<std::endl;
				continue;
			break;
			default:
				std::cout<<"Invalid scan_type: "<<scan_type<<endl;
				return 0;
			break;
		}
		
		// Wait for user input to select captureing mode
		scanner.contScan();											// start scanning using the Lidar at specified angle and speed

		motor.moveAngleAbsolute(0);								// after scanning bring back the motor to home location

		vector<Scanner::DataPoint> data = scanner.getLidarData(); 	// get the data by scanning from the Lidar
		
		if(data.size() == 0)
			std::cout<<"Error: no scanner data received"<<std::endl;

		string filename = "points.pcd";
		savePCD(data, filename);

		cout << "done!" << endl;	
		
	}	

	return 0;

}

