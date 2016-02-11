#pragma once

#include "Lidar.hpp"
#include "MCodeMotor.hpp"

class Scanner
{
public:
	struct DataPoint
	{
		double x;
		double y;
		unsigned short intensity;
		double angle;
	};

	Scanner(Lidar& lidar, MCodeMotor& motor);
	void scan();
	vector<DataPoint> getLidarData();

private:
	static const int SCAN_CENTER;
	static const int SCAN_SIZE;
	static const unsigned int SCAN_VELOCITY;

	Lidar& lidar;
	MCodeMotor& motor;
	vector<DataPoint> lidarData;
};

