#include "MCodeMotor.hpp"

#include <thread>
#include <chrono>
#include <iostream>
#include <cmath>

using namespace std;

const unsigned int MCodeMotor::DEFAULT_ACCELERATION = 10000;
const unsigned int MCodeMotor::DEFAULT_DECELERATION = 10000;
const unsigned int MCodeMotor::DEFAULT_INITIAL_VELOCITY = 0;
const unsigned int MCodeMotor::DEFAULT_MAXIMUM_VELOCITY = 1000;
const unsigned int MCodeMotor::DEFAULT_RUN_CURRENT = 80;
const unsigned int MCodeMotor::DEFAULT_HOLD_CURRENT = 80;
const unsigned int MCodeMotor::BLOCKING_REFRESH_RATE_MILLISECONDS = 50;
const unsigned int MCodeMotor::BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS = 10000;
const unsigned int MCodeMotor::HOME_RETRYS = 5;
const unsigned int MCodeMotor::HOME_RETRY_DELAY_MILLISECONDS = 1000;
const unsigned int MCodeMotor::MOTOR_RESPONSE_TIMEOUT_MILLISECONDS = 500;
const unsigned int MCodeMotor::MOTOR_RESPONSE_SLEEP_TIME_MILLISECONDS = 5;
const int MCodeMotor::DEFAULT_POSITION = -1322;
const unsigned int MCodeMotor::ENCODER_COUNTS_PER_ROTATION = 4000;

MCodeMotor::MCodeMotor(const string& ipAddress, const unsigned int port)
	:	ipAddress(ipAddress)
	,	port(port)
	,	socket() {}

bool MCodeMotor::connect()
{
	return socket.connectToServer(ipAddress, port);
}

string& MCodeMotor::sendCommand(const string& command)
{
	socket.sendString(command + "\r\n");
	response = socket.receiveString();

	// Each response from the motor ends with a '?' or '>'.
	// Check to see that a '?' was recieved before continuing.
	chrono::steady_clock::time_point recieveStartTime = chrono::steady_clock::now();
	while ((getResponse().find('?') == string::npos && getResponse().find('>') == string::npos) &&
		(chrono::duration_cast<std::chrono::milliseconds>(
			chrono::steady_clock::now() - recieveStartTime).count() <=
			MOTOR_RESPONSE_TIMEOUT_MILLISECONDS))
	{
		this_thread::sleep_for(chrono::milliseconds(MOTOR_RESPONSE_SLEEP_TIME_MILLISECONDS));
		response = response + socket.receiveString();
	}


	if (getResponse().find('?') == string::npos && getResponse().find('>') == string::npos)
	{
		cout << response << endl;
		// The response does not contain a '?' or '>'.
		// The response timedout.
		cerr << "[Error] Timeout: No response from motor at " << ipAddress << ":" 
			<< port << "!" << endl;
		response = "";
	}

	// Get rid of the extra characters on the response.
	if (command.size() + 2 < response.size())
	{
		response = response.substr(command.size() + 2, response.size() - command.size() - 4);
	}
	return getResponse();
}

string& MCodeMotor::getResponse()
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
	sendCommand("ST 0"); // Reset Stall Flag
	sendCommand("EE 1"); // Enable Encoder
	setAcceleration(acceleration);
	setDeceleration(deceleration);
	setInitialVelocity(initialVelocity);
	setMaximumVelocity(maximumVelocity);
	setRunCurrent(runCurrent);
	setHoldCurrent(holdCurrent);
}

void MCodeMotor::setAcceleration(const unsigned int acceleration)
{
	sendCommand("A " + to_string(acceleration)); // Set Acceleration
	this->acceleration = acceleration;
}

void MCodeMotor::setDeceleration(const unsigned int deceleration)
{
	sendCommand("D " + to_string(deceleration)); // Set Decleration
	this->deceleration = deceleration;
}

void MCodeMotor::setInitialVelocity(const unsigned int initialVelocity)
{
	sendCommand("VI " + to_string(initialVelocity)); // Set Initial Velocity
	this->initialVelocity = initialVelocity;
}

void MCodeMotor::setMaximumVelocity(const unsigned int maximumVelocity)
{
	sendCommand("VM " + to_string(maximumVelocity)); // Set Maximum Velocity
	this->maximumVelocity = maximumVelocity;
}

void MCodeMotor::setRunCurrent(const unsigned int runCurrent)
{
	sendCommand("RC " + to_string(runCurrent)); // Set Run Current
	this->runCurrent = runCurrent;
}

void MCodeMotor::setHoldCurrent(const unsigned int holdCurrent)
{
	sendCommand("HC " + to_string(holdCurrent)); // Set Hold Current
	this->holdCurrent = holdCurrent;
}

unsigned int MCodeMotor::getAcceleration()
{
	return acceleration;
}

unsigned int MCodeMotor::getDeceleration()
{
	return deceleration;
}

unsigned int MCodeMotor::getInitialVelocity()
{
	return initialVelocity;
}

unsigned int MCodeMotor::getMaximumVelocity()
{
	return maximumVelocity;
}

unsigned int MCodeMotor::getRunCurrent()
{
	return runCurrent;
}

unsigned int MCodeMotor::getHoldCurrent()
{
	return holdCurrent;
}

unsigned int MCodeMotor::getMoveRelativeTime(const float angle)
{
	if (getInitialVelocity() != 0)
	{
		// TODO support non-zero initial velocity
		cerr << "[Error] Move time calculation with non-zero initial velocity "
			<< "not supported." << endl;
	}

	unsigned int encoderCounts = abs(ceil((angle * ENCODER_COUNTS_PER_ROTATION) / 360));

	// this uses doubles to prevent overflow and rounding
	unsigned int rampUpCounts = 0.5 * getMaximumVelocity() * getMaximumVelocity() /
		getAcceleration();
	unsigned int rampDownCounts = 0.5 * getMaximumVelocity() * getMaximumVelocity() /
		getDeceleration();
	unsigned int rampCounts = rampUpCounts + rampDownCounts;

	bool reachFullSpeed = encoderCounts >= rampCounts;

	if (reachFullSpeed)
	{
		return 1000.0 * encoderCounts / getMaximumVelocity() +
			1000.0 * getMaximumVelocity() * (getAcceleration() + getDeceleration()) /
			(2 * (double)getAcceleration() * getDeceleration());
	}
	else
	{
		return sqrt(2.0 * 1000 * 1000 *  encoderCounts * (getAcceleration() + getDeceleration()) /
			(getAcceleration() * getDeceleration()));
	}
}

float MCodeMotor::getMoveRelativeAngleAtTime(const float angle, const unsigned int milliseconds)
{
	if (getInitialVelocity() != 0)
	{
		// TODO support non-zero initial velocity
		cerr << "[Error] Move time calculation with non-zero initial velocity "
			<< "not supported." << endl;
	}

	unsigned int encoderCounts = abs(ceil((angle * ENCODER_COUNTS_PER_ROTATION) / 360));

	// this uses doubles to prevent overflow and rounding
	unsigned int rampUpCounts = 0.5 * getMaximumVelocity() * getMaximumVelocity() /
		getAcceleration();
	unsigned int rampDownCounts = 0.5 * getMaximumVelocity() * getMaximumVelocity() /
		getDeceleration();
	unsigned int moveTime = getMoveRelativeTime(angle)

	bool reachFullSpeed = encoderCounts >= rampCounts;

	if (!reachFullSpeed)
	{
		// TODO support angle calcuation when motor does not reach maximum velocity
		cerr << "[Error] Angle calculation not supported when motor does not reach maximum velocity" << endl;
	}

	// TODO
}

bool MCodeMotor::homeToIndex()
{
	bool success = false;
	for (unsigned int i = 0; i < HOME_RETRYS && !success; i++)
	{
		if (i != 0)
		{
			cerr << "[Error] Motor at " << ipAddress << ":" << port 
				<< " was unable to home to index. Retrying in " 
				<< HOME_RETRY_DELAY_MILLISECONDS << " milliseconds..." << endl;
			this_thread::sleep_for(chrono::milliseconds(HOME_RETRY_DELAY_MILLISECONDS));
		}
		// Move to the left to make sure the motor is past the index mark
		moveRelative(-20, 2000);
		sendCommand("HI 3"); // Home to Index Mark
		blockWhileMoving(5000);
		detectStall();
		sendCommand("PR I6"); // Read Encoder at Index
		success = getResponseBool(false);
	}

	if (success)
	{
		sendCommand("C2 " + to_string(-1 * DEFAULT_POSITION)); // Reset Encoder Count
		moveAbsolute(0);
	}
	else
	{
		cerr << "[Error] Motor at " << ipAddress << ":" << port 
			<< " was unable to home to index after " << HOME_RETRYS
			<< " trys." << endl;
		
	}
	return success;
}

bool MCodeMotor::moveAngleRelative(const float angle, const unsigned int timeoutMilliseconds)
{
	int encoderCounts = ceil((angle * ENCODER_COUNTS_PER_ROTATION) / 360);
	return moveRelative(encoderCounts, timeoutMilliseconds);
}

bool MCodeMotor::moveAngleAbsolute(const float angle, const unsigned int timeoutMilliseconds)
{
	int encoderCounts = ceil((angle * ENCODER_COUNTS_PER_ROTATION) / 360);
	return moveAbsolute(encoderCounts, timeoutMilliseconds);
}

bool MCodeMotor::moveRelative(const int motorSteps, const unsigned int timeoutMilliseconds)
{
	if (timeoutMilliseconds != 0)
	{
		sendCommand("ST 0"); // Reset Stall Flag
	}

	sendCommand("MR " + to_string(motorSteps));

	if (timeoutMilliseconds == 0)
	{
		return true;
	}

	bool timeout = !blockWhileMoving(timeoutMilliseconds);
	bool stall = detectStall();

	return timeout || stall;
}

bool MCodeMotor::moveAbsolute(const int motorSteps, const unsigned int timeoutMilliseconds)
{
	if (timeoutMilliseconds != 0)
	{
		sendCommand("ST 0"); // Reset Stall Flag
	}

	sendCommand("MA " + to_string(motorSteps));

	if (timeoutMilliseconds == 0)
	{
		return true;
	}

	bool timeout = !blockWhileMoving(timeoutMilliseconds);
	bool stall = detectStall();

	return timeout || stall;
	
}

bool MCodeMotor::blockWhileMoving(const unsigned int timeoutMilliseconds)
{
	chrono::steady_clock::time_point blockStartTime = chrono::steady_clock::now();
	while (isMoving(true) &&
		(chrono::duration_cast<std::chrono::milliseconds>(
			chrono::steady_clock::now() - blockStartTime).count() <=
			timeoutMilliseconds))
	{
		this_thread::sleep_for(chrono::milliseconds(BLOCKING_REFRESH_RATE_MILLISECONDS));
	}

	if (isMoving(true))
	{
		cerr << "[Error] Timeout: Motor at " << ipAddress << ":" << port 
			<< " did not finish moving in " << timeoutMilliseconds << " milliseconds." << endl;
		return false;
	}
	return true;
}

bool MCodeMotor::isMoving(bool defaultValue)
{
	sendCommand("PR MV"); // Check if Moving

	return getResponseBool(defaultValue);
}

bool MCodeMotor::detectStall()
{
	sendCommand("PR ST"); // Read Stall Flag
	bool stall = getResponseBool(false);

	if (stall)
	{
		cerr << "[Error] Stall detected with motor at " << ipAddress << ":" << port << "." << endl;
	}

	sendCommand("ST 0"); // Reset Stall Flag

	return stall;
}

