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
		unsigned int rgb;
	};
	
	struct DataRaw // The raw data used to represent each data point 
	{
		double dis; // in mm
		double angle_scan; // lidar angle, in deg
		double angle_motor; // motor angle, from motor home position, in deg
		unsigned short intensity;
		bool direction; // True for positive direction of motor, false for reverse
	};

	Scanner(Lidar& lidar, MCodeMotor& motor);
	~Scanner() {}

	void contScan(float scan_size, int scan_lines, float line_size, float scan_center = SCAN_CENTER, int scan_times = 1); 	// Scanning mode one, continuous
	void stepScan(float scan_size, int scan_lines, float line_size, float scan_center = SCAN_CENTER, int scan_times = 1);	// Scanning mode two, stepping motor
	static vector<DataPoint> getLidarData(vector<Scanner::DataRaw>& dataRaw);
	vector<Scanner::DataRaw>* getLidarRawPtr();
private:
	static const int SCAN_CENTER;
	static const int SCAN_SIZE;
	static const unsigned int SCAN_VELOCITY;

	Lidar& lidar;
	MCodeMotor& motor;
	vector<DataPoint> lidarData;
	vector<DataRaw> lidarRawData;
	// status:
	bool _is_motor_ready = false;
	bool _is_lidar_ready = false;
};

