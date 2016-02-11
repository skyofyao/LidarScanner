#include "Scanner.hpp"

#include <iostream>
#include <cmath>

using namespace std;

const double PI = 3.141592653589793238;

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

	vector<Lidar::DataPoint> data = lidar.scan(motor.getMoveRelativeTime(SCAN_SIZE));

	motor.blockWhileMoving(1000);

	long startTime = data.at(0).timestamp;
	// process data
	for (unsigned int i = 0; i < data.size(); i++)
	{
		Lidar::DataPoint point = data.at(i);
		double angle = (motor.getMoveRelativeAngleAtTime(SCAN_SIZE, point.timestamp - startTime) -
			(SCAN_SIZE / 2.0)) * PI / 180;

		// convert from cylindrical to orthagonal coordinates
		DataPoint newPoint;
		newPoint.x = point.y * cos(angle);
		newPoint.y = point.y * sin(angle);
		newPoint.z = point.x;
		newPoint.intensity = point.intensity;
		lidarData.push_back(newPoint);
	}

	// return maximum velocity back to default
	motor.setMaximumVelocity();

}

vector<Scanner::DataPoint> Scanner::getLidarData()
{
	return lidarData;
}

