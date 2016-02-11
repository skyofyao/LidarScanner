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

	motor.sendCommand("MA -1000");
}
