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

int main(int argc, char** argv) 
{	
	std::cout<<"START"<<endl;
	//string commandReceived;
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

	CameraPair camera_pair;									// intializing object of class camera

	camera_pair.camPair_connect();
	camera_pair.camPair_init();

	
	//camera_pair.camPair_capture("t1");						// take a picture using cameras
	
	MCodeMotor motor(motorIpAddress, motorPort);			// initializing object of class MCodeMotor

	if (!motor.connect())									// checking for motor connection
	{
		std::cout<<"Motor not connected"<<std::endl;
		return 1;
	}

	Lidar lidar(lidarIpAddress, lidarPort);					// initializing object of class Lidar
	if (!lidar.connect())									// checking for lidar connection
	{
		std::cout<<"Lidar not connected"<<std::endl;
		return 1;
	}

	motor.initializeSettings();								// initialize motor settings 

	if (!motor.homeToIndex())								// check if motor is positioned at home location. Home is defined as the position parallel to the motor alignment
	{
		return 1;
	}

	motor.initializeSettings(500, 500);								// initialize motor settings 
	
	Scanner scanner(lidar, motor);							// initialize object of class Scanner

	//camera1.cam_init();										// take a picture using camera

	// Data acqusition loop
	while(1)
	{
		std::cout<<"Input parameters"<<endl;
		// Parameter format: 
		// "[char:scanType] [str:filename] [float:scan_size] [int:scan_lines] [float:lidarScanSize] [float:scan_center] [int: scan_times]"
		string input_str;
		getline(cin,input_str);
		stringstream str_stm = stringstream(input_str);
		// Parameters
		char scan_type = 0; // 'O' for orginal, 'S' for step, 'C' for continuous, others for exit
		string filename_pfx;	// Filename prefix for the photos and the pcd file
		float scan_size = 0;	// Motor movement size, in deg
		float scan_center = 0;	// The middle angle of motor movement, in deg
		float lidar_scan_size = 0; // The angle of lidar valid scan pixels, in deg
		int	 scan_lines = 0; // How many lines to be scanned in the motor movement
		int scan_times = 0;	// How many times to scan 
		// Parse parameters
		str_stm >> scan_type;
		if(scan_type <31 || scan_type > 127 )
		{
			getline(cin,input_str);
			str_stm = stringstream(input_str);
			str_stm >> scan_type;
		}
		str_stm >> filename_pfx;
		
		string filename = "/home/odroid/pheno3v2/photos/";
		
		vector<Scanner::DataRaw>* rdata;
		vector<Scanner::DataPoint> data;

		
		switch(scan_type)
		{
			//case 'O':
			//	std::cout<<"Origional scan"<<endl;
			//	scanner.contScan();											// start scanning using the Lidar at specified angle and speed

			//	motor.moveAngleAbsolute(0);								// after scanning bring back the motor to home location

			//	//data = scanner.getLidarData(); 	// get the data by scanning from the Lidar
			//	
			//	if(data.size() == 0)
			//		std::cout<<"Error: no scanner data received"<<std::endl;

			//	savePCD(data, filename);

			//	cout << "done!" << endl;	
			//break;
			case 'S':
				std::cout<<"Step scan"<<endl;
				str_stm >> scan_size>> scan_lines>> lidar_scan_size>> scan_center>>scan_times;
				str_stm.ignore();
				std::cout<<"Parameters: "<<scan_size<<", " <<scan_lines<<", "<<lidar_scan_size<<", "<<scan_center<<", "<<scan_times<<endl;
		
				filename.append(filename_pfx);
				camera_pair.camPair_capture(filename_pfx);										// take a picture using cameras
	
				filename.append(".pcd");
				std::cout<<"file path"<<filename<<std::endl;
				
				scanner.stepScan(scan_size, scan_lines, lidar_scan_size, scan_center, scan_times);
				rdata = scanner.getLidarRawPtr();
				if(rdata->size() == 0)
					std::cout<<"Error: no scanner data received"<<std::endl;
				saveRaw(*rdata, filename);
				cout << "Done!" << endl;
				motor.moveAngleAbsolute(0);
			break;
			case 'C':
				std::cout << "Cont scan" << endl;
				str_stm >> scan_size >> scan_lines >> lidar_scan_size >> scan_center >> scan_times;
				str_stm.ignore();
				std::cout << "Parameters: " << scan_size << ", " << scan_lines << ", " << lidar_scan_size << ", " << scan_center << ", " << scan_times << endl;

				filename.append(filename_pfx);
				camera_pair.camPair_capture(filename_pfx);										// take a picture using cameras

				filename.append(".pcd");
				std::cout << "file path" << filename << std::endl;

				scanner.contScan(scan_size, scan_lines, lidar_scan_size, scan_center, scan_times);
				rdata = scanner.getLidarRawPtr();
				data = scanner.getLidarData(*rdata);
				if (data.size() == 0)
					std::cout << "Error: no scanner data received" << std::endl;
				saveColoredPCD(data, filename);
				cout << "Done!" << endl;
				motor.moveAngleAbsolute(0);
				break;
			case 'L':	// For camera calibration
				std::cout << "Calibration mode" << endl;
				str_stm.ignore();
				filename.append(filename_pfx);
				std::cout << "File prefix:" << filename << endl;

				while (1)
				{
					std::cout << "Ready to take images, input file number to start" << endl;
					getline(cin, input_str);
					str_stm = stringstream(input_str);
					string file_num;
					str_stm >> file_num;
					string filename_img = filename;
					filename_img.append(file_num);
					
					camera_pair.camPair_capture(filename_img);


				}



				break;
			default:
				std::cout<<"Invalid scan_type: "<<scan_type<<endl;
				return 0;
			break;
		}
		
		
	}	
	camera_pair.camPair_disconnect();
	return 0;

}

