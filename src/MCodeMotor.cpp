#include "MCodeMotor.hpp"

#include <thread>
#include <chrono>

MCodeMotor::MCodeMotor(const string& ipAddress, const unsigned int port)
	:	ipAddress(ipAddress)
	,	port(port)
	,	socket() {}

bool MCodeMotor::connect()
{
	return socket.connectToServer(ipAddress, port);
}

void MCodeMotor::sendCommand(const string& command)
{
	socket.sendString(command + "\r\n");
	response = socket.receiveString();

	// remove command and extra characters from the response
	response = response.substr(command.size() + 2, response.size() - command.size() - 4);
}

string MCodeMotor::getResponse()
{
	return response;
}

void MCodeMotor::initializeSettings(const unsigned int acceleration,
	const unsigned int deceleration, const unsigned int initialVelocity,
	const unsigned int maximumVelocity, const unsigned int runCurrent,
	const unsigned int holdCurrent)
{
	sendCommand("A " + to_string(acceleration)); // Set Acceleration
	sendCommand("D " + to_string(deceleration)); // Set Decleration
	sendCommand("VI " + to_string(initialVelocity)); // Set Initial Velocity
	sendCommand("VM " + to_string(maximumVelocity)); // Set Maximum Velocity
	sendCommand("RC " + to_string(runCurrent)); // Set Run Current to 80%
	sendCommand("HC " + to_string(holdCurrent)); // Set Hold Current to 80%
}

bool MCodeMotor::homeToIndex()
{
	// TODO add timeout
	bool success = false;
	for (unsigned int i = 0; i < HOME_RETRYS && !success; i++)
	{
		// TODO
	}
	
	if (success)
	{
		sendCommand("EE 1"); // Encoder Enable
	}
	return success;
}

void MCodeMotor::blockWhileMoving()
{
	// TODO add timeout
	while (isMoving())
	{
		// TODO
		//this_thread::sleep_for(chrono::milliseconds(BLOCKING_REFRESH_RATE_MILLISECONDS));
	}
}

bool MCodeMotor::isMoving()
{
	sendCommand("PR MV"); // Check if Moving

	// The return string will not contain 0 if the motor is moving
	return getResponse().find('0') == string::npos;
}

