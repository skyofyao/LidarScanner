#include "MCodeMotor.hpp"
#include "Lidar.hpp"
#include "Scanner.hpp"
#include "Camera.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

const string motorIpAddress = "192.168.33.1";
const int motorPort = 503;
const string lidarIpAddress = "192.168.0.10";
const int lidarPort = 10940;

int main() 
{
//	Camera11 camera1;	

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

//	camera1.cam_init();

	scanner.scan();

	motor.moveAngleAbsolute(0);

}

