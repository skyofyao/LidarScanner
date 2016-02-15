#include "Lidar.hpp"

#include <iostream>
#include <chrono>
#include <cmath>

const unsigned int Lidar::RANGE = 135;
const unsigned int Lidar::SCANS_PER_SECOND = 30;

Lidar::Lidar(const string& ipAddress, const unsigned int port)
	:	ipAddress(ipAddress)
	,	port(port)
	,	urg() {}

bool Lidar::connect()
{
	if (!urg.open(ipAddress.c_str(),
				  port,
				  qrk::Urg_driver::Ethernet)) {
		cerr << "[Error] Error connecting to lidar on " << ipAddress << ":" 
			<< port << ": " << urg.what() << endl;
		return false;
	}
	cout << "[Info] Connected to lidar on " << ipAddress << ":" << port << endl;

	urg.set_scanning_parameter(urg.deg2step(-RANGE), urg.deg2step(+RANGE), 0);
	return true;
}

vector<Lidar::DataPoint> Lidar::scan(const unsigned int milliseconds)
{
	vector<DataPoint> dataPoints;
	chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
	urg.start_measurement(qrk::Urg_driver::Distance_intensity, qrk::Urg_driver::Infinity_times, 0);
	while (chrono::duration_cast<std::chrono::milliseconds>(
			chrono::steady_clock::now() - startTime).count() <=
			milliseconds)
	{
		vector<long> data;
		vector<unsigned short> intensity;
		long time_stamp = 0;

		if (!urg.get_distance_intensity(data, intensity, &time_stamp))
		{
			cout << "Urg_driver::get_distance(): " << urg.what() << endl;
			//return 1; TODO handle this properly
		}

		processScan(data, intensity, time_stamp, dataPoints);		
	}
	urg.stop_measurement();

	return dataPoints;
}

void Lidar::processScan(vector<long>& data, vector<unsigned short>& intensity, long timeStamp, vector<DataPoint>& dataPoints)
{
	long min_distance = urg.min_distance();
	long max_distance = urg.max_distance();
	size_t data_n = data.size();

	// TODO get this to calculate things right
	double timePerIndex = 1000.0 / SCANS_PER_SECOND / 360.0 / abs(urg.index2deg(1) - urg.index2deg(0));
	for (size_t i = 0; i < data_n; ++i)
	{
		long l = data[i];
		if ((l <= min_distance) || (l >= max_distance))
		{
			continue;
		}

		double radian = urg.index2rad(i);
		double x = l * cos(radian);
		double y = l * sin(radian);

		// TODO calculate actual timestamp
		DataPoint point = {i, x, y, intensity[i], timeStamp + (long)(timePerIndex * i)};
		dataPoints.push_back(point);
	}
}

