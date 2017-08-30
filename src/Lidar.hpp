
#pragma once

#include <string>
#include <vector>

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
	};

	Lidar(const string& ipAddress, const unsigned int port);
	bool connect();
	vector<DataPointRaw> scan_time(const unsigned int milliseconds, float line_size = 0);
	vector<DataPointRaw> scan_once(float line_size = 0);
	bool is_connected = false;

	static const unsigned int RANGE;
	static const unsigned int SCANS_PER_SECOND;

private:

	
	static const unsigned int MAX_STEPS = 1080;
	static const unsigned int MAX_ANGLE = 270; // in deg


	const string ipAddress;
	const unsigned int port;

	//void processScan(vector<long>& data, vector<unsigned short>& intensity, long timeStamp, vector<DataPoint>& dataPoints, float line_size = 0);
	void processScanRaw(vector<long>& data, vector<unsigned short>& intensity, long timeStamp, vector<DataPointRaw>& rdataPoints, float line_size = 0);

	qrk::Urg_driver urg;
};

