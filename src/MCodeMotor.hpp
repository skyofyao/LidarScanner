#pragma once

#include <string>

#include "Socket.hpp"

using namespace std;

class MCodeMotor
{
private:
	static const unsigned int DEFAULT_ACCELERATION;
	static const unsigned int DEFAULT_DECELERATION;
	static const unsigned int DEFAULT_INITIAL_VELOCITY;
	static const unsigned int DEFAULT_MAXIMUM_VELOCITY;
	static const unsigned int DEFAULT_RUN_CURRENT;
	static const unsigned int DEFAULT_HOLD_CURRENT;

public:
	MCodeMotor(const string& ipAddress, const unsigned int port);
	bool connect();
	string sendCommand(const string& command);
	string getResponse();
	bool getResponseBool(bool defaultValue);
	void initializeSettings(
		const unsigned int acceleration = DEFAULT_ACCELERATION,
		const unsigned int deceleration = DEFAULT_DECELERATION,
		const unsigned int initialVelocity = DEFAULT_INITIAL_VELOCITY,
		const unsigned int maximumVelocity = DEFAULT_MAXIMUM_VELOCITY,
		const unsigned int runCurrent = DEFAULT_RUN_CURRENT,
		const unsigned int holdCurrent = DEFAULT_HOLD_CURRENT);
	bool homeToIndex();
	void blockWhileMoving();
	bool isMoving(bool defaultValue);

private:
	const string ipAddress;
	const unsigned int port;
	Socket socket;
	string response;

	static const unsigned int BLOCKING_REFRESH_RATE_MILLISECONDS;
	static const unsigned int HOME_RETRYS;
	static const unsigned int HOME_RETRY_DELAY_MILLISECONDS;
};

