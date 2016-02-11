#include "MCodeMotor.hpp"

#include <thread>
#include <chrono>
#include <iostream>

using namespace std;

const unsigned int MCodeMotor::DEFAULT_ACCELERATION = 10000;
const unsigned int MCodeMotor::DEFAULT_DECELERATION = 50000;
const unsigned int MCodeMotor::DEFAULT_INITIAL_VELOCITY = 100;
const unsigned int MCodeMotor::DEFAULT_MAXIMUM_VELOCITY = 1000;
const unsigned int MCodeMotor::DEFAULT_RUN_CURRENT = 80;
const unsigned int MCodeMotor::DEFAULT_HOLD_CURRENT = 80;
const unsigned int MCodeMotor::BLOCKING_REFRESH_RATE_MILLISECONDS = 50;
const unsigned int MCodeMotor::HOME_RETRYS = 5;
const unsigned int MCodeMotor::HOME_RETRY_DELAY_MILLISECONDS = 1000;

MCodeMotor::MCodeMotor(const string& ipAddress, const unsigned int port)
	:	ipAddress(ipAddress)
	,	port(port)
	,	socket() {}

bool MCodeMotor::connect()
{
	return socket.connectToServer(ipAddress, port);
}

string MCodeMotor::sendCommand(const string& command)
{
	socket.sendString(command + "\r\n");
	response = socket.receiveString();
	
	// TODO below is for debugging purposes, remove
	cout << "cmd [" << command << "]" << endl;

	cout << "response [" << response << "]" << endl;

	if (response.size() == 2)
	{
		cout << "<<<\n<<<\n<<<\n<<<\n<<<\n<<<\n<<<\n<<<\n<<<\n<<<\n<<<\n";
		response = socket.receiveString();
		cout << "response [" << response << "]" << endl;
	}
	// remove command and extra characters from the response
	response = response.substr(command.size() + 2, response.size() - command.size() - 4);
	return getResponse();
}

string MCodeMotor::getResponse()
{
	return response;
}

bool MCodeMotor::getResponseBool(bool defaultValue)
{
	if (defaultValue)
	{
		// The returns true (1) if the return string does not contain 0
		return getResponse().find('0') == string::npos;
	}
	else
	{
		// The returns false (0) if the return string does not contain 1
		return getResponse().find('1') != string::npos;
	}
}

void MCodeMotor::initializeSettings(const unsigned int acceleration,
	const unsigned int deceleration, const unsigned int initialVelocity,
	const unsigned int maximumVelocity, const unsigned int runCurrent,
	const unsigned int holdCurrent)
{
	sendCommand("EE 1"); // Enable Encoder
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
		if (i != 0)
		{
			cerr << "[Error] Motor at " << ipAddress << ":" << port 
				<< " was blocked homing to index. Retrying in " 
				<< HOME_RETRY_DELAY_MILLISECONDS << " milliseconds..." << endl;
			this_thread::sleep_for(chrono::milliseconds(HOME_RETRY_DELAY_MILLISECONDS));
		}
		sendCommand("MR -20"); // Move left 20
		blockWhileMoving();
		sendCommand("HI 3"); // Home to Index Mark
		blockWhileMoving();
		sendCommand("PR I6"); // Read Encoder at Index
		success = getResponseBool(false);
	}

	if (success)
	{
		sendCommand("C2 0"); // Reset Encoder Count
	}
	else
	{
		cerr << "[Error] Motor at " << ipAddress << ":" << port 
			<< " was unable to home to index after " << HOME_RETRYS
			<< " retrys." << endl;
		
	}
	return success;
}

void MCodeMotor::blockWhileMoving()
{
	// TODO add timeout
	while (isMoving(true))
	{
		this_thread::sleep_for(chrono::milliseconds(BLOCKING_REFRESH_RATE_MILLISECONDS));

	}
}

bool MCodeMotor::isMoving(bool defaultValue)
{
	sendCommand("PR MV"); // Check if Moving

	return getResponseBool(defaultValue);
}

