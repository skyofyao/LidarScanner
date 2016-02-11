#include "Scanner.hpp"

const int Scanner::SCAN_CENTER = 0;
const int Scanner::SCAN_SIZE = 90;
const unsigned int Scanner::SCAN_VELOCITY = 100;

Scanner::Scanner(Lidar& lidar, MCodeMotor& motor)
	:	lidar(lidar)
	,	motor(motor)
	,	lidarData() {}

void Scanner::scan()
{
	motor.moveAngleAbsolute(SCAN_CENTER - SCAN_SIZE / 2);

	motor.setMaximumVelocity(SCAN_VELOCITY);

	motor.moveAngleRelative(SCAN_SIZE, 0);

	vector<Lidar::DataPoint> data = lidar.scan(10010);

	// TODO process data

	motor.blockWhileMoving(1000);

	// return maximum velocity back to default
	motor.setMaximumVelocity();

}

vector<Scanner::DataPoint> Scanner::getLidarData()
{
	return lidarData;
}

