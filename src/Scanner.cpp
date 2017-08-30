
#include "Scanner.hpp"

#include <iostream>
#include <cmath>
#include <vector>

using namespace std;

const double PI = 3.141592653589793238;

const int Scanner::SCAN_CENTER = 0;
const int Scanner::SCAN_SIZE = 120;
const unsigned int Scanner::SCAN_VELOCITY = 200;

Scanner::Scanner(Lidar& lidar, MCodeMotor& motor)
	:	lidar(lidar)
	,	motor(motor)
	,	lidarData()
	,	_is_lidar_ready(lidar.is_connected)
	,	_is_motor_ready(motor.is_connected)
	{
		//TODO: handle cases if lidar or motor not connected
	}

// TODO: for scanning methods, pass parameters to the function instead of using classmember values


// Continuous scanning mode, motor non-stopping
/*	@param scan_size - The angle of scanning (motor movement), in deg
	@param scan_velocity - The motor speed whil moving, in encoder/second
	@param scan_center - The center of the scanning (motor swiping) view, relative to the defind home. in deg
*/
void Scanner::contScan(float scan_size, int scan_lines, float line_size, float scan_center)
{
	motor.moveAngleAbsolute(scan_center - scan_size / 2);

	int scan_velocity = scan_size / scan_lines * MCodeMotor::ENCODER_COUNTS_PER_ROTATION / 360.0 * Lidar::SCANS_PER_SECOND;

	motor.setMaximumVelocity(scan_velocity);

	motor.moveAngleRelative(scan_size, 0);

	std::vector<Lidar::DataPointRaw> data = lidar.scan_time(motor.getMoveRelativeTime(SCAN_SIZE));

	motor.blockWhileMoving(1000);

	long startTime = data.at(0).timestamp;
	// process data
	for (unsigned int i = 0; i < data.size(); i++)
	{
		Lidar::DataPointRaw point = data.at(i);
		double angle = (motor.getMoveRelativeAngleAtTime(scan_size, point.timestamp - startTime) -
			(scan_size / 2.0)) * PI / 180;

		// convert from cylindrical to orthagonal coordinates
		DataRaw newPoint;
		newPoint.dis = point.dis;
		newPoint.angle_motor = angle;
		newPoint.angle_scan = point.radian / PI * 180.0;
		newPoint.intensity = point.intensity;
		lidarRawData.push_back(newPoint);
	}

	// return maximum velocity back to default
	//motor.setMaximumVelocity();

}

// Stepping scanning mode, stop-scan-move-stop-scan-....
/*	@param scan_size - The angle of scanning (motor movement), in deg
	@param scan_lines - How many lines within the entire scanning
	@param lin_size - The angle for each scan line to save
	@param scan_center - The center of the scanning (motor swiping) view, relative to the defind home. in deg
*/
void Scanner::stepScan(float scan_size, int scan_lines, float line_size, float scan_center)
{
	lidarRawData.clear();
	float angle_start = scan_center - scan_size / 2;
	motor.moveAngleAbsolute(angle_start);

	float angle_per_scan = scan_size / (scan_lines-1);
	
	motor.setMaximumVelocity();

	for(int i = 0; i < scan_lines; i++)
	{
		motor.moveAngleAbsolute(angle_start + angle_per_scan * i, 0);

		motor.blockWhileMoving(500);
		int msecondsToSleep = 100;
#if defined(WIN32) || defined(WIN64)
		Sleep(msecondsToSleep);
#elif defined(LINUX)
		struct timespec nsDelay;
		nsDelay.tv_sec = 0;
		nsDelay.tv_nsec = (long)msecondsToSleep * 1000000L;
		nanosleep(&nsDelay, NULL);
#endif
		vector<Lidar::DataPointRaw> rdata = lidar.scan_once(line_size);
		// Append scanner data to the vector in class
		vector<DataRaw> tempvec(rdata.size());
		for(int it_vec = 0; it_vec < rdata.size(); it_vec++)
		{
			tempvec[it_vec].dis = rdata[it_vec].dis;
			tempvec[it_vec].angle_scan = rdata[it_vec].radian / PI * 180.0;
			tempvec[it_vec].angle_motor = angle_start + angle_per_scan * i;
			tempvec[it_vec].intensity = rdata[it_vec].intensity;
		}
		// Append
		lidarRawData.insert(lidarRawData.end(), tempvec.begin(), tempvec.end());
	}
}


// Process raw data to calculate xyz data
vector<Scanner::DataPoint> Scanner::getLidarData(vector<Scanner::DataRaw>& dataRaw)
{
	vector<Scanner::DataPoint> vec_xyzi;
	int cloudsize = dataRaw.size();
	int valid_n = 0;
	for (int i = 0; i < cloudsize; i++)
	{
		vec_xyzi[valid_n].intensity = dataRaw[i].intensity;
		vec_xyzi[valid_n].y = dataRaw[i].dis * sin(dataRaw[i].angle_scan * PI / 180.0);
		double dis_xz = dataRaw[i].dis * cos(dataRaw[i].angle_motor * PI / 180.0);
		vec_xyzi[valid_n].z = dis_xz * cos(dataRaw[i].angle_motor * PI / 180.0);
		vec_xyzi[valid_n].x = dis_xz * sin(dataRaw[i].angle_motor * PI / 180.0);
		valid_n++;
	}
	vec_xyzi.resize(valid_n);
	return vec_xyzi;
}

vector<Scanner::DataRaw> Scanner::getLidarRaw()
{
	return lidarRawData;
}

