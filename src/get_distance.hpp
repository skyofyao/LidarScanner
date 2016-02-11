#pragma once

#include "Urg_driver.h"
#include <cmath>
#include <iostream>
#include <string>

using namespace qrk;
using namespace std;

const string ipAddress = "192.168.0.10";
const int port = 10940;

void print_data(const Urg_driver& urg,
                const vector<long>& data,
                const vector<unsigned short>& intensity, long time_stamp)
{
    long min_distance = urg.min_distance();
    long max_distance = urg.max_distance();
    size_t data_n = data.size();
    for (size_t i = 0; i < data_n; ++i) {
        long l = data[i];
        if ((l <= min_distance) || (l >= max_distance)) {
            continue;
        }

        double radian = urg.index2rad(i);
        long x = static_cast<long>(l * cos(radian));
        long y = static_cast<long>(l * sin(radian));
        cout << i << ", " << x << ", " << y << ", " << intensity[i] << ", " << time_stamp << endl;
    }
    cout << endl;
}


int get_distance()
{
    // Connects to the sensor
    Urg_driver urg;
    if (!urg.open(ipAddress.c_str(),
                  port,
                  Urg_driver::Ethernet)) {
        cout << "Urg_driver::open(): "
             << ipAddress << ": " << urg.what() << endl;
        return 1;
    }

    // Gets measurement data
    // Case where the measurement range (start/end steps) is defined
    urg.set_scanning_parameter(urg.deg2step(-135), urg.deg2step(+135), 0);
    enum { Capture_times = 10 };
    urg.start_measurement(Urg_driver::Distance_intensity, Urg_driver::Infinity_times, 0);
    for (int i = 0; i < Capture_times; ++i) {
        vector<long> data;
		vector<unsigned short> intensity;
        long time_stamp = 0;

        if (!urg.get_distance_intensity(data, intensity, &time_stamp)) {
            cout << "Urg_driver::get_distance(): " << urg.what() << endl;
            return 1;
        }
        print_data(urg, data, intensity, time_stamp);
    }
    return 0;
}




