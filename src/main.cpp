#include "MCodeMotor.hpp"
#include "Lidar.hpp"	

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
	
	motor.moveAngleAbsolute(-45);

	//motor.setMaximumVelocity(500);
	motor.setMaximumVelocity(100);
	motor.moveAngleRelative(90, 0);

	//vector<Lidar::DataPoint> data = lidar.scan(2050);
	vector<Lidar::DataPoint> data = lidar.scan(10010);

	int count = 0;
	for(unsigned int i = 0; i < data.size(); i++)
	{
		if (i > 0 && data[i].index < data[i - 1].index)
		{
			cout << endl;
			count = 0;
		}

		while (count < data[i].index)
		{
			cout << 0 << " ";
			count++;
		}
		cout << data[i].intensity << " ";
		count++;
	}

	cout << endl << "::==::" << endl;

	count = 0;
	for(unsigned int i = 0; i < data.size(); i++)
	{
		if (i > 0 && data[i].index < data[i - 1].index)
		{
			cout << endl;
			count = 0;
		}

		while (count < data[i].index)
		{
			cout << 0 << " ";
			count++;
		}
		cout << (int)(data[i].x + 500) << " ";
		count++;
	}

	motor.blockWhileMoving(1000);
	motor.setMaximumVelocity();

	motor.moveAngleAbsolute(0);
}

