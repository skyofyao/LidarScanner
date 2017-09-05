#include "Lidar.hpp"

#include <iostream>
#include <chrono>
#include <cmath>

const unsigned int Lidar::RANGE = 135;
const unsigned int Lidar::SCANS_PER_SECOND = 40;
static const float	LIDAR_ANGULAR_RES = 0.25;

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
	is_connected = true;
	return true;
}

void Lidar::scan_time_start()
{

}

// Perform continues scan for certain time
/* Param: line_size: for each scan, define the size in which data is valid, in deg
*/
vector<Lidar::DataPointRaw> Lidar::scan_time(const unsigned int milliseconds, float line_size)
{
	vector<DataPointRaw> dataPoints;
	chrono::steady_clock::time_point startTime = chrono::steady_clock::now();
	urg.start_measurement(qrk::Urg_driver::Distance_intensity, qrk::Urg_driver::Infinity_times, 0);
	while (chrono::duration_cast<std::chrono::milliseconds>(
			chrono::steady_clock::now() - startTime).count() <=
			milliseconds)
	{
		vector<long> data;
		vector<unsigned short> intensity;
		long timestamp = 0;

		if (!urg.get_distance_intensity(data, intensity, &timestamp))
		{
			cout << "Urg_driver::get_distance(): " << urg.what() << endl;
			//return 1; TODO handle this properly
		}
		//std::cout << "Urg_get_dis_inten() received points: " << data.size() << std::endl;
		processScanRaw(data, intensity, timestamp, dataPoints, line_size);
	}
	urg.stop_measurement();
	std::cout << "Urg_get_dis_inten() received points: " << dataPoints.size() << std::endl;

	return dataPoints;
}

// Perform only one scan
/* Param: line_size: for each scan, define the size in which data is valid, in deg
*/
vector<Lidar::DataPointRaw> Lidar::scan_once(float line_size)
{
	vector<DataPointRaw> dataPoints;
	urg.start_measurement(qrk::Urg_driver::Distance_intensity, qrk::Urg_driver::Infinity_times, 0);
	
	vector<long> data;
	vector<unsigned short> intensity;
	long timestamp = 0;
	if (!urg.get_distance_intensity(data, intensity, &timestamp))
	{
		cout << "Urg_driver::get_distance(): " << urg.what() << endl;
		//return 1; TODO handle this properly
	}
	processScanRaw(data, intensity, timestamp, dataPoints, line_size);
	//std::cout<<"One scan, size"<<dataPoints.size()<<"/"<<data.size()<<endl;
	return dataPoints;
}
//
//void Lidar::processScan(vector<long>& data, vector<unsigned short>& intensity, long timestamp, vector<DataPoint>& dataPoints, float line_size)
//{
//	long minDistance = urg.min_distance();
//	long maxDistance = urg.max_distance();
//	size_t data_n = data.size();
//
//	double timePerIndex = 1000.0 * abs(urg.index2deg(1) - urg.index2deg(0)) / SCANS_PER_SECOND / 360.0;
//	for (size_t i = 0; i < data_n; ++i)
//	{
//		long l = data[i];
//		if ((l <= minDistance) || (l >= maxDistance))
//		{
//			continue;
//		}
//
//		double radian = urg.index2rad(i);
//		double x = l * cos(radian);
//		double y = l * sin(radian);
//
//		// calculate actual timestamp
//		DataPoint point = {i, x, y, intensity[i], timestamp + (long)(timePerIndex * i)};
//		dataPoints.push_back(point);
//	}
//}

// Store the scaned data into a vector, storing: dis, angle, intensity, and time
/*	@param data - Sensor output distance array
	@param intensity - Sensor output intensity array
	@param timestamp - Sensor output time stemp, (from scanning start)
	@param dataPoints - Input & Output, the vector sotring values, append values at the end
*/
void Lidar::processScanRaw(vector<long>& data, vector<unsigned short>& intensity, long timestamp, vector<DataPointRaw>& dataPoints, float line_size)
{
	long minDistance = urg.min_distance();
	long maxDistance = urg.max_distance();
	size_t data_n = data.size();

	int idx_mid = MAX_STEPS / 2;
	int idx_min = idx_mid - line_size / LIDAR_ANGULAR_RES / 2;
	int idx_max = idx_mid + line_size / LIDAR_ANGULAR_RES / 2;
	
	// TODO get this to calculate things right
	double timePerIndex = 1000.0 * abs(urg.index2deg(1) - urg.index2deg(0)) / SCANS_PER_SECOND / 360.0; // ms
	int n_valid = 0;
	vector<DataPointRaw> data_current_scan(idx_max - idx_min);
	for (size_t i = idx_min; i < idx_max; ++i)
	{
		long l = data[i];
		if ((l >= minDistance) && (l <= maxDistance))
		{
			DataPointRaw point = {l, urg.index2rad(i), intensity[i], timestamp + (long)(timePerIndex * i)};
			data_current_scan[n_valid] = point;
			n_valid++;
			
		}
	}
	data_current_scan.resize(n_valid);
	//std::cout<<"In scan, valid:"<<n_valid<<"/"<<data.size()<<endl;
	// Append to the end of the output vector
	dataPoints.insert(dataPoints.end(), data_current_scan.begin(), data_current_scan.end());
}

