#pragma once

#include <string>

#include "Socket.hpp"

using namespace std;

class MCodeMotor
{
private:
	static const unsigned int DEFAULT_ACCELERATION = 10000;
	static const unsigned int DEFAULT_DECELERATION = 50000;
	static const unsigned int DEFAULT_INITIAL_VELOCITY = 100;
	static const unsigned int DEFAULT_MAXIMUM_VELOCITY = 1000;
	static const unsigned int DEFAULT_RUN_CURRENT = 80;
	static const unsigned int DEFAULT_HOLD_CURRENT = 80;

public:
	MCodeMotor(const string& ipAddress, const unsigned int port);
	bool connect();
	void sendCommand(const string& command);
	string getResponse();
	void initializeSettings(
		const unsigned int acceleration = DEFAULT_ACCELERATION,
		const unsigned int deceleration = DEFAULT_DECELERATION,
		const unsigned int initialVelocity = DEFAULT_INITIAL_VELOCITY,
		const unsigned int maximumVelocity = DEFAULT_MAXIMUM_VELOCITY,
		const unsigned int runCurrent = DEFAULT_RUN_CURRENT,
		const unsigned int holdCurrent = DEFAULT_HOLD_CURRENT);
	bool homeToIndex();
	void blockWhileMoving();
	bool isMoving();

private:
	const string ipAddress;
	const unsigned int port;
	Socket socket;
	string response;

	static const unsigned int BLOCKING_REFRESH_RATE_MILLISECONDS = 50;
	static const unsigned int HOME_RETRYS = 5;
	static const unsigned int HOME_RETRY_DELAY_MILLISECONDS = 1000;
};

