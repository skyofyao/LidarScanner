/***********************************************************************/
// This is a header managing methods of controlling motor AND lidar as a /
// Scanner. 
// Scanner has two operation modes:
// 1. Continuous scanning mode (contScan()), in which lidar keeps scanning, while motor /
// moves at constant speed, without stops
// 2. Step scanning mode(stepScan()), in which scan only once after the motor was moved /
// in a small angle. 
/*************************************************************************/
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
		double z;
		unsigned short intensity;
	};
	
	struct DataRaw // The raw data used to represent each data point 
	{
		double dis; // in mm
		double angle_scan; // lidar angle, 
		double angle_motor; // motor angle, from motor home position
		unsigned short intensity;
	}

	Scanner(Lidar& lidar, MCodeMotor& motor);
	void contScan(); 	// Scanning mode one, continuous
	void stepScan();	// Scanning mode two, stepping motor
	vector<DataPoint> getLidarData();

private:
	static const int SCAN_CENTER;
	static const int SCAN_SIZE;
	static const unsigned int SCAN_VELOCITY;

	Lidar& lidar;
	MCodeMotor& motor;
	vector<DataPoint> lidarData;
};

