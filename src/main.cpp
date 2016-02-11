#include "MCodeMotor.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

#include "get_distance.hpp"

using namespace std;

const string motorIpAddress = "192.168.33.1";
const int motorPort = 503;

int main() 
{
/*
	MCodeMotor motor(motorIpAddress, motorPort);
	if (!motor.connect())
	{
		return 1;
	}

	motor.initializeSettings();

	if (!motor.homeToIndex())
	{
		return 1;
	}

	motor.moveAngleAbsolute(-45);
	motor.moveAngleRelative(90);
	motor.moveAngleAbsolute(0);
*/
	get_distance();
}

