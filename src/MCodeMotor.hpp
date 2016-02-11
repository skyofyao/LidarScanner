#pragma once

#include <string>

#include "Socket.hpp"

using namespace std;

/// Class for controlling and communcatating with Schneider Electric Mdrive motors.
/// See http://motion.schneider-electric.com/index.html
class MCodeMotor
{
private:
	/// Default acceleration rate.
	static const unsigned int DEFAULT_ACCELERATION;

	/// Default deceleration rate.
	static const unsigned int DEFAULT_DECELERATION;

	/// Default initial velocity.
	static const unsigned int DEFAULT_INITIAL_VELOCITY;

	/// Default maximum velocity.
	static const unsigned int DEFAULT_MAXIMUM_VELOCITY;

	/// Default run current.
	static const unsigned int DEFAULT_RUN_CURRENT;

	/// Default hold current.
	static const unsigned int DEFAULT_HOLD_CURRENT;

	/// Time in milliseconds to poll the motor when blocking execution until the motor stops.
	static const unsigned int BLOCKING_REFRESH_RATE_MILLISECONDS;

	/// Maximum time to block execution when waiting for the motor to stop.
	static const unsigned int BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS;

	/// Number of times to retry homing sequence before giving up.
	static const unsigned int HOME_RETRYS;

	/// Time in millisecond to between each try to home the motor.
	static const unsigned int HOME_RETRY_DELAY_MILLISECONDS;

	/// Amount of time in milliseconds to wait for a response from the motor after sending a command.
	static const unsigned int MOTOR_RESPONSE_TIMEOUT_MILLISECONDS;

	/// Amount of time between each check for a response from the motor.
	static const unsigned int MOTOR_RESPONSE_SLEEP_TIME_MILLISECONDS;

	/// Distance from home in encoder counts of the default zero position.
	static const int DEFAULT_POSITION;

	// Number of encoder counts in a single rotation.
	static const unsigned int ENCODER_COUNTS_PER_ROTATION;

public:
	/// Constructs a new McodeMotor.
	/// \param ipAddress IP address of the motor.
	/// \param port IP port number to connect to the motor on.
	MCodeMotor(const string& ipAddress, const unsigned int port);

	/// Connects to the motor.
	/// \returns True if connected to the motor successfully.
	bool connect();

	/// Sends a command to the motor.
	/// \returns The response from the motor.
	string& sendCommand(const string& command);

	/// Returns the response from the motor for the last command sent.
	/// \returns The response from the motor.
	string& getResponse();

	/// Returns the response from the motor for the last command sent as a bool.
	/// \returns The response from the motor.
	bool getResponseBool(bool defaultValue);

	/// Sends all of the settings to the motor. If paramters are not specified, the default settings will be sent.
	void initializeSettings(
		const unsigned int acceleration = DEFAULT_ACCELERATION,
		const unsigned int deceleration = DEFAULT_DECELERATION,
		const unsigned int initialVelocity = DEFAULT_INITIAL_VELOCITY,
		const unsigned int maximumVelocity = DEFAULT_MAXIMUM_VELOCITY,
		const unsigned int runCurrent = DEFAULT_RUN_CURRENT,
		const unsigned int holdCurrent = DEFAULT_HOLD_CURRENT);

	/// Sends the givien acceleration to the motor. If no parameter is specified the default acceleration will be sent.
	void setAcceleration(const unsigned int acceleration = DEFAULT_ACCELERATION);

	/// Sends the givien deceleration to the motor. If no parameter is specified the default deceleration will be sent.
	void setDeceleration(const unsigned int deceleration = DEFAULT_DECELERATION);

	/// Sends the givien velocity to the motor. If no parameter is specified the default velocity will be sent.
	void setInitialVelocity(const unsigned int initialVelocity = DEFAULT_INITIAL_VELOCITY);

	/// Sends the givien maximum velocity to the motor. If no parameter is specified the default maximum will be sent.
	void setMaximumVelocity(const unsigned int maximumVelocity = DEFAULT_MAXIMUM_VELOCITY);

	/// Sends the givien run current to the motor. If no parameter is specified the default run current will be sent.
	void setRunCurrent(const unsigned int runCurrent = DEFAULT_RUN_CURRENT);

	/// Sends the givien hold current to the motor. If no parameter is specified the default hold current will be sent.
	void setHoldCurrent(const unsigned int holdCurrent = DEFAULT_HOLD_CURRENT);

	/// Returns the current acceleration.
	unsigned int getAcceleration();

	/// Returns the current deceleration.
	unsigned int getDeceleration();

	/// Returns the current initial velocity.
	unsigned int getInitialVelocity();

	/// Returns the current maximum velocity.
	unsigned int getMaximumVelocity();

	/// Returns the current run current.
	unsigned int getRunCurrent();

	/// Returns the current hold current.
	unsigned int getHoldCurrent();

	/// Calculates the time in milliseconds for the motor to move the given angle.
	/// \param angle The angle in degrees for the motor to move.
	/// \returns The calculated number of milliseconds for the motor to move the given angle.
	unsigned int getMoveRelativeTime(const double angle);

	/// Calculates the angle at the given time if the motor is moving the given angle.
	/// \param angle The total angle in degrees for the motor to move.
	/// \param milliseconds The time since the motor started moving in milliseconds.
	/// \returns The calculated angle in degrees at the given time. 
	double getMoveRelativeAngleAtTime(const double angle, const unsigned int milliseconds);

	/// Tells the motor to find home and zeros the encoder position based DEFAULT_POSITION.
	/// \returns True if successful.
	bool homeToIndex();

	/// Moves the motor by the given angle. This blocks execution until the move is complete.
	/// \param angle The angle in degrees for the motor to move.
	/// \param timeoutMilliseconds If the the motor has not completed the move within this time, this will stop blocking.
	/// 	If timeoutMilliseconds is 0, this will not block execution.
	/// \returns True if successful.
	bool moveAngleRelative(const float angle, const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);

	/// Moves the motor to the given angle. This blocks execution until the move is complete.
	/// \param angle The angle in degrees for the motor to move to.
	/// \param timeoutMilliseconds If the the motor has not completed the move within this time, this will stop blocking.
	/// 	If timeoutMilliseconds is 0, this will not block execution.
	/// \returns True if successful.
	bool moveAngleAbsolute(const float angle, const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);

	/// Moves the motor by the given angle. This blocks execution until the move is complete.
	/// \param angle The angle in encoder counts for the motor to move.
	/// \param timeoutMilliseconds If the the motor has not completed the move within this time, this will stop blocking.
	/// 	If timeoutMilliseconds is 0, this will not block execution.
	/// \returns True if successful.
	bool moveRelative(const int motorSteps, const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);

	/// Moves the motor to the given angle. This blocks execution until the move is complete.
	/// \param angle The angle in encoder counts for the motor to move to.
	/// \param timeoutMilliseconds If the the motor has not completed the move within this time, this will stop blocking.
	/// 	If timeoutMilliseconds is 0, this will not block execution.
	/// \returns True if successful.
	bool moveAbsolute(const int motorSteps, const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);

	/// Blocks execution until the motor is no longer moving
	/// \param timeoutMilliseconds If the the motor has not completed the move within this time, this will stop blocking.
	/// \returns Ture if the motor did not timeout.
	bool blockWhileMoving(const unsigned int timeoutMilliseconds = BLOCKING_DEFAULT_TIMEOUT_MILLISECONDS);

	/// Checks if the motor is moving
	/// \returns True if the motor is moving.
	bool isMoving(bool defaultValue);

	/// Checks to see if the stall flag has been triggered. Resets the stall flag.
	/// \returns True if a stall was detected.
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

