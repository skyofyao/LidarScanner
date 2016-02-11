#include "MCodeMotor.hpp"
#include "Lidar.hpp"
#include "Scanner.hpp"
#include "Camera.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>
#include <fstream>

using namespace std;

const unsigned int listeningPort = 500;

const string motorIpAddress = "192.168.33.1";
const int motorPort = 503;
const string lidarIpAddress = "192.168.0.10";
const int lidarPort = 10940;

int main() 
{
	//Camera11 camera1;	

	//camera1.cam_init();
	MCodeMotor motor(motorIpAddress, motorPort);

	if (!motor.connect())
	{
		return 1;
	}

	Lidar lidar(lidarIpAddress, lidarPort);
	if (!lidar.connect())
	{
		return 1;
	}

	motor.initializeSettings();

	if (!motor.homeToIndex())
	{
		return 1;
	}

	Scanner scanner(lidar, motor);

	//camera1.cam_init();

	scanner.scan();

	motor.moveAngleAbsolute(0);

	vector<Scanner::DataPoint> data = scanner.getLidarData();

	cout << "[Info] Writing to file...";
	// write file
	ofstream stlfile;
	stlfile.open("points.pcd");
	// header
	stlfile << "VERSION .7" << endl;
	stlfile << "FIELDS x y z"/* intensity"*/ << endl;
	stlfile << "SIZE 4 4 4"/* 4"*/ << endl;
	stlfile << "TYPE F F F"/* I"*/ << endl;
	stlfile << "COUNT 1 1 1"/* 1"*/ << endl;
	stlfile << "WIDTH " << data.size() << endl;
	stlfile << "HEIGHT 1" << endl;
	stlfile << "VIEWPOINT 0 0 0 1 0 0 0" << endl;
	stlfile << "POINTS " << data.size() << endl;
	stlfile << "DATA ascii" << endl;
	// data
	for (unsigned int i = 0; i < data.size(); i++)
	{
		Scanner::DataPoint point = data.at(i);
		stlfile << point.x << " " << point.y << " " << point.z << " " /*<< point.intensity */<< endl;
	}
	stlfile.close();

	cout << "done!" << endl;		

	return 0;
}

