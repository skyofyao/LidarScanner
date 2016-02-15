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
		long timestamp;
	};


	Lidar(const string& ipAddress, const unsigned int port);
	bool connect();
	vector<DataPoint> scan(const unsigned int milliseconds);

private:
	static const unsigned int RANGE;
	static const unsigned int SCANS_PER_SECOND;

	const string ipAddress;
	const unsigned int port;

	void processScan(vector<long>& data, vector<unsigned short>& intensity, long timeStamp, vector<DataPoint>& dataPoints);

	qrk::Urg_driver urg;
};

