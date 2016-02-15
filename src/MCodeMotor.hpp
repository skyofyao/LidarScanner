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

	static const unsigned int BLOCKING_REFRESH_RATE_MILLISECONDS;
	static const unsigned int BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS;
	static const unsigned int HOME_RETRYS;
	static const unsigned int HOME_RETRY_DELAY_MILLISECONDS;

	static const unsigned int MOTOR_RESPONSE_TIMEOUT_MILLISECONDS;
	static const unsigned int MOTOR_RESPONSE_SLEEP_TIME_MILLISECONDS;

	static const int DEFAULT_POSITION;
	static const unsigned int ENCODER_COUNTS_PER_ROTATION;

public:
	MCodeMotor(const string& ipAddress, const unsigned int port);
	bool connect();
	string& sendCommand(const string& command);
	string& getResponse();
	bool getResponseBool(bool defaultValue);
	void initializeSettings(
		const unsigned int acceleration = DEFAULT_ACCELERATION,
		const unsigned int deceleration = DEFAULT_DECELERATION,
		const unsigned int initialVelocity = DEFAULT_INITIAL_VELOCITY,
		const unsigned int maximumVelocity = DEFAULT_MAXIMUM_VELOCITY,
		const unsigned int runCurrent = DEFAULT_RUN_CURRENT,
		const unsigned int holdCurrent = DEFAULT_HOLD_CURRENT);
	void setAcceleration(const unsigned int acceleration = DEFAULT_ACCELERATION);
	void setDeceleration(const unsigned int deceleration = DEFAULT_DECELERATION);
	void setInitialVelocity(const unsigned int initialVelocity = DEFAULT_INITIAL_VELOCITY);
	void setMaximumVelocity(const unsigned int maximumVelocity = DEFAULT_MAXIMUM_VELOCITY);
	void setRunCurrent(const unsigned int runCurrent = DEFAULT_RUN_CURRENT);
	void setHoldCurrent(const unsigned int holdCurrent = DEFAULT_HOLD_CURRENT);
	unsigned int getAcceleration();
	unsigned int getDeceleration();
	unsigned int getInitialVelocity();
	unsigned int getMaximumVelocity();
	unsigned int getRunCurrent();
	unsigned int getHoldCurrent();
	unsigned int getMoveRelativeTime(const double angle);
	double getMoveRelativeAngleAtTime(const double angle, const unsigned int milliseconds);
	bool homeToIndex();
	bool moveAngleRelative(const float angle, const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);
	bool moveAngleAbsolute(const float angle, const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);
	bool moveRelative(const int motorSteps, const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);
	bool moveAbsolute(const int motorSteps, const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);
	bool blockWhileMoving(const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);
	bool isMoving(bool defaultValue);
	bool detectStall();

private:
	const string ipAddress;
	const unsigned int port;
	Socket socket;
	string response;
	unsigned int acceleration;
	unsigned int deceleration;
	unsigned int initialVelocity;
	unsigned int maximumVelocity;
	unsigned int runCurrent;
	unsigned int holdCurrent;
};

