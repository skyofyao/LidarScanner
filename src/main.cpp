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

	motor.sendCommand("EE 1"); // Encoder Enable
/*	while (true)
	{
		motor.sendCommand("PR I6"); // Read Encoder Count
		sleep(0.25);
		cout << socket.receiveString();
	}*/
	motor.initializeSettings();
	motor.sendCommand("PR I6"); // Read Encoder at Index
	motor.sendCommand("HI 3"); // Home to Index Mark
	while (true)
	{
		cout << (motor.isMoving() ? "true" : "false") << endl;
		this_thread::sleep_for(chrono::milliseconds(200));
	}
//	motor.sendCommand("MR 100"); // Move Relative
//	sleep(2);
//	motor.sendCommand("MR -100"); // Move Relative
}
