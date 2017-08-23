#pragma once
#include <iostram>
#include "scanner.hpp"

using namespace std;
void savePCD(std::vector<Scanner::DataPoint> data, std::string filename)
{
		std::cout << "[Info] Writing to file...";						
		// write file
		ofstream stlfile;										// initialize stlfile. stlfile contains data describing the layout of a 3D object 
		stlfile.open(filename);								// opening the file with filename
		// header
		stlfile << "VERSION .7" << endl;						
		stlfile << "FIELDS x y z" << endl;// /* intensity"
		stlfile << "SIZE 4 4 4" << endl;// /* 4"
		stlfile << "TYPE F F F"<< endl;// /* I"
		stlfile << "COUNT 1 1 1" << endl;// /* 1"
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