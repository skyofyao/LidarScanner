
#pragma once

#include <string>

#include "Urg_driver.h"

using namespace std;

class Lidar
{
public:
	struct DataPoint
	{
		unsigned int index;
		double x;
		double y;
		unsigned short intensity;
		long timestamp;	// From starting time
	};
	struct DataPointRaw
	{
		double dis;
		double radian;
		unsigned short intensity;
		long timestamp;// if possible
	}

	Lidar(const string& ipAddress, const unsigned int port);
	bool connect();
	vector<DataPoint> scan_time(const unsigned int milliseconds, float line_size = 0);
	vector<DataPointRaw> scan_once(float line_size = 0);
	bool is_connected = false;

private:
	static const unsigned int RANGE;
	static const unsigned int SCANS_PER_SECOND;
	
	static const unsigned int MAX_STEPS = 1080;
	static const unsigned int MAX_ANGLE = 270; // in deg
	static const float	ANGULAR_RES = 0.25;

	const string ipAddress;
	const unsigned int port;

	void processScan(vector<long>& data, vector<unsigned short>& intensity, long timeStamp, vector<DataPoint>& dataPoints, float line_size = 0);
	void processScanRaw(vector<long>& data, vector<unsigned short>& intensity, long timeStamp, vector<DataPointRaw>& rdataPoints, float line_size = 0);

	qrk::Urg_driver urg;
};

