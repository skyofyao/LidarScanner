#pragma once
#include <iostream>
#include <fstream>
#include "Scanner.hpp"

using namespace std;
void savePCD(std::vector<Scanner::DataPoint>& data, std::string filename)
{
		std::cout << "[Info] Writing to PCD file...";						
		// write file
		ofstream stlfile;										// initialize stlfile. stlfile contains data describing the layout of a 3D object 
		stlfile.open(filename);								// opening the file with filename
		// header
		stlfile << "VERSION .7" << endl;						
		stlfile << "FIELDS x y z intensity" << endl;// /* intensity"
		stlfile << "SIZE 4 4 4 4" << endl;// /* 4"
		stlfile << "TYPE F F F U"<< endl;// /* I"
		stlfile << "COUNT 1 1 1 1" << endl;// /* 1"
		stlfile << "WIDTH " << data.size() << endl;
		stlfile << "HEIGHT 1" << endl;
		stlfile << "VIEWPOINT 0 0 0 1 0 0 0" << endl;
		stlfile << "POINTS " << data.size() << endl;			// size of the scanned lidar data
		stlfile << "DATA ascii" << endl;// data

		for (unsigned int i = 0; i < data.size(); i++)			
		{
			Scanner::DataPoint point = data.at(i);				// picking data at each row
			stlfile << point.x << " " << point.y << " " << point.z << " " << point.intensity << endl;		// printing x, y, z, intensity from each data row 
		}
		stlfile.close();				
}

void saveColoredPCD(std::vector<Scanner::DataPoint>& data, std::string filename)
{
	std::cout << "[Info] Writing to colored PCD file...";
	// write file
	ofstream stlfile;										// initialize stlfile. stlfile contains data describing the layout of a 3D object 
	stlfile.open(filename);								// opening the file with filename
														// header
	stlfile << "VERSION .7" << endl;
	stlfile << "FIELDS x y z i rgb" << endl;// /* intensity"
	stlfile << "SIZE 4 4 4 4 4" << endl;// /* 4"
	stlfile << "TYPE F F F U U" << endl;// /* I"
	stlfile << "COUNT 1 1 1 1 1" << endl;// /* 1"
	stlfile << "WIDTH " << data.size() << endl;
	stlfile << "HEIGHT 1" << endl;
	stlfile << "VIEWPOINT 0 0 0 1 0 0 0" << endl;
	stlfile << "POINTS " << data.size() << endl;			// size of the scanned lidar data
	stlfile << "DATA ascii" << endl;// data

	for (unsigned int i = 0; i < data.size(); i++)
	{
		Scanner::DataPoint point = data.at(i);				// picking data at each row
		stlfile << point.x << " " << point.y << " " << point.z << " " << point.intensity <<" "<< point.rgb << endl;		// printing x, y, z, intensity from each data row 
	}
	stlfile.close();
}

void saveRaw(std::vector<Scanner::DataRaw>& data, std::string filename)
{
		std::cout << "[Info] Writing to rawd ata file...";						
		// write file
		ofstream stlfile;										// initialize stlfile. stlfile contains data describing the layout of a 3D object 
		stlfile.open(filename);								// opening the file with filename
		// header
		stlfile << "VERSION .7" << endl;						
		stlfile << "FIELDS dis angle_scan angle_motor intensity" << endl;// /* intensity"
		stlfile << "SIZE 4 4 4 2" << endl;// /* 4"
		stlfile << "TYPE F F F U"<< endl;// /* I"
		stlfile << "COUNT 1 1 1 1" << endl;// /* 1"
		stlfile << "WIDTH " << data.size() << endl;
		stlfile << "HEIGHT 1" << endl;
		stlfile << "VIEWPOINT 0 0 0 1 0 0 0" << endl;
		stlfile << "POINTS " << data.size() << endl;			// size of the scanned lidar data
		stlfile << "DATA ascii" << endl;// data

		for (unsigned int i = 0; i < data.size(); i++)			
		{
			Scanner::DataRaw point = data.at(i);				// picking data at each row
			stlfile << point.dis << " " << point.angle_scan << " " << point.angle_motor << " " << point.intensity << endl;		// printing x, y, z, intensity from each data row 
		}
		stlfile.close();				
}