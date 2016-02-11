#include "MCodeMotor.hpp"

#include <iostream>
#include <string>
#include <thread>
#include <chrono>

using namespace std;

const string ipAddress = "192.168.33.1";
const int port = 503;

int main()
{
	MCodeMotor motor(ipAddress, port);
	if (!motor.connect())
	{
		return 1;
	}

	motor.initializeSettings();

	if (!motor.homeToIndex())
	{
		return 1;
	}

	motor.moveAbsolute(-800);

	this_thread::sleep_for(chrono::milliseconds(5000));

	motor.moveRelative(200);
	
	motor.moveRelative(-1000);

	motor.moveRelative(50);

	motor.moveAbsolute(-800);
}

