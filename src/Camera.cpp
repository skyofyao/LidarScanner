//=============================================================================
//
//This program Captures images using Software Trigger and save it in a location
//The location can be changed
//v2 will have functionality to make change to the filename
//
//=============================================================================
#include"Camera.hpp"

#include<unistd.h>
#include<time.h>
#include<iostream>
#include<sstream>
#include<time.h>
#include<iomanip>
#include<bitset>

enum ExtendedShutterType
{
	NO_EXTENDED_SHUTTER,
	DRAGONFLY_EXTENDED_SHUTTER,
	GENERAL_EXTENDED_SHUTTER
};



//
//To Check the version of the software 
//Can be removed after integrating the system
//Mainly used for purposes to check the Library added
//No significant Imporatnce to this code
//

void Camera11::PrintBuildInfo()
{
	FC2Version fc2version;  //Version of the SDK
	Utilities::GetLibraryVersion(&fc2version);
	ostringstream version;
	version << "FlyCapture2 library Version:" << fc2version.major << "." << fc2version.minor << "." << fc2version.build;
	cout << version.str() << endl;
	ostringstream Timestamp;
	Timestamp << "Application Build date: " << __DATE__ << " " << __TIME__;
	cout << Timestamp.str() << endl << endl;
}


//
//To Print the Camera Information
//To check whether the camera is connected and the Firmware Version in it
//Can be removed after integrating the system
//

void PrintCameraInfo(CameraInfo* pCamInfo)
{
	cout << endl;
	cout << "**** CAMERA INFORMATION****" << endl;
	cout << "Serial Number- " << pCamInfo->serialNumber << endl;
	cout << "Camera model- " << pCamInfo->modelName << endl;
	cout << "Sensor -" << pCamInfo->sensorInfo << endl;
	cout << "Resolution- " << pCamInfo->sensorResolution << endl;
	cout << "Firmware Version -" << pCamInfo->firmwareVersion << endl;
	cout << endl;
}


//
//Start of Important program
//To check the presence of a Software trigger
//Need to use Software Trigger mode to get input from the main computer through software
//

bool CheckSoftwareTriggerPresence(Camera* pCam) //Camera Calss should be used to manipulate any registers in the system
{
	//530h corrsponds to the Trigger Inquiry Resgister
	//It gives the value of the presence of a certain feature
	const unsigned int k_triggerInq = 0x530;
	Error error;

	unsigned int regVal = 0;
	//Set to Inquire the value in the register
	//Read the trigger Inquiry Register and Obtain the 32 bit offset value in regVal

	error = pCam->ReadRegister(k_triggerInq, &regVal);

	//
	//Check for Error and print the error
	//
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return false;
	}

	//
	//The 15th bit corresponds to the presence of Software Trigger
	//Checking if the bit is high to check the presence of software trigger
	//for reference use page 20 CameraRegisterReference
	//
	if ((regVal & 0x10000) != 0x10000)
	{
		return false;
	}

	return true;

}

//
//Checking the Trigger is Ready
//If the bit read is 0 it is ready
//If bit read is 1 it is busy
//
bool PollForTriggerReady(Camera* pCam)
{
	const unsigned int k_softwareTrigger = 0x62C; //Register to check the Software Trigger is ready to be used
	Error error;
	unsigned int regVal = 0;

	do
	{
		//Reading the Register contents of 62Ch and getting the offset
		error = pCam->ReadRegister(k_softwareTrigger, &regVal);

		//Check for errors
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return false;
		}

	} while ((regVal >> 31) != 0); //Checking each bit if its not busy

	//If all the bits are 0 then it is in ready state
	return true;
}

//
//Firing the software trigger
//To fire we will write 1 to the 62Ch Register
//

bool FireSoftwareTrigger(Camera* pCam)
{
	const unsigned int k_softwareTrigger = 0x62C;
	const unsigned int k_fireVal = 0x80000000;
	Error error;

	error = pCam->WriteRegister(k_softwareTrigger, k_fireVal);//Writing the register 62Ch with value 1 in all 32 bits

	//Checking for error
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return false;
	}

	return true;
}

//
//Printing Format 7 Capabilities
//Just a Print statement Can be removed
//
void Camera11::PrintFormat7Capabilities(Format7Info fmt7Info)
{
	cout << "Max image pixels: (" << fmt7Info.maxWidth << ", " << fmt7Info.maxHeight << ")" << endl;
	cout << "Image Unit size: (" << fmt7Info.imageHStepSize << ", " << fmt7Info.imageVStepSize << ")" << endl;
	cout << "Offset Unit size: (" << fmt7Info.offsetHStepSize << ", " << fmt7Info.offsetVStepSize << ")" << endl;
	cout << "Pixel format bitfield: 0x" << fmt7Info.pixelFormatBitField << endl;

}

int Camera11::cam_connect()
{
	Error error;
	error = cam.Connect(&guid);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
	//
	// Power on the camera
	//To begin Power up process set 610h to all 1
	//
	const unsigned int k_cameraPower = 0x610;
    const unsigned int k_powerVal = 0x80000000;
	const unsigned int millisecondsToSleep = 100;
    unsigned int regVal = 0;
    unsigned int retries = 10;
	error = cam.WriteRegister(k_cameraPower, k_powerVal);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
	
	//
	//Waiting for the camera to power up 
	//by querying the 31 bit of 610h
	//
	//const unsigned int millisecondsToSleep = 100;
	//unsigned int regVal = 0;
	//unsigned int retries = 10;

	// Wait for camera to complete power-up
	cout << "powering up camera" << endl;
	do
	{
#if defined(WIN32) || defined(WIN64)
		Sleep(millisecondsToSleep);
#elif defined(LINUX)
		struct timespec nsDelay;
		nsDelay.tv_sec = 0;
		nsDelay.tv_nsec = (long)millisecondsToSleep * 1000000L;
		nanosleep(&nsDelay, NULL);
#endif
		//
		//Reading the value of the register 610h
		//
		error = cam.ReadRegister(k_cameraPower, &regVal);
		if (error == PGRERROR_TIMEOUT)
		{
			// ignore timeout errors, camera may not be responding to
			// register reads during power-up
		}
		else if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

		retries--;
	} while ((regVal & k_powerVal) == 0 && retries > 0); //Retry for 10 times

	// Check for timeout errors after retrying
	if (error == PGRERROR_TIMEOUT)
	{
		PrintError(error);
		return -1;
	}

	// Get the camera information
	//cout << "getting camera info" << endl;
	CameraInfo camInfo;
	error = cam.GetCameraInfo(&camInfo);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
		
	//Set Cam_num variable according to serialNumber
	if (camInfo.serialNumber == 15435734)
	{cam_num = "l";}
	else if (camInfo.serialNumber == 15435724) 
	{cam_num = "r";}
	return 0;
}
			
int Camera11::cam_init(float shutterspeed)
{
	//
	//Main Aim is to set the width and height
	//To check whether the format 7 mode is supported 
	//will give error if not	
	// Query for available Format 7 modes
	//
	Format7Info fmt7Info;
	bool supported;
	fmt7Info.mode = k_fmt7Mode; // will return true if format7 is supported
	Error error = cam.GetFormat7Info(&fmt7Info, &supported);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	//PrintFormat7Capabilities(fmt7Info); // Prints the format7 features

	if ((k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0)
	{
		// Pixel format not supported!
		cout << "Pixel format is not supported" << endl;
		return -1;
	}

	//Setting the format7 features
	//Setting the max width and height
	//Setting the pixel format
	Format7ImageSettings fmt7ImageSettings;
	fmt7ImageSettings.mode = k_fmt7Mode;
	fmt7ImageSettings.offsetX = 0;
	fmt7ImageSettings.offsetY = 0;
	fmt7ImageSettings.width = fmt7Info.maxWidth;
	fmt7ImageSettings.height = fmt7Info.maxHeight;
	fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;

	bool valid;
	Format7PacketInfo fmt7PacketInfo;

	//To chck whether the settings are accepted
	// Validate the settings to make sure that they are valid
	error = cam.ValidateFormat7Settings(
		&fmt7ImageSettings,
		&valid,
		&fmt7PacketInfo);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	if (!valid)
	{
		// Settings are not valid
		cout << "Format7 settings are not valid" << endl;
		return -1;
	}

	// Set the settings to the camera
	error = cam.SetFormat7Configuration(
		&fmt7ImageSettings,
		fmt7PacketInfo.recommendedBytesPerPacket);
		cout<<"Packet size:"<<fmt7PacketInfo.recommendedBytesPerPacket<<endl;
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}


	// Check if the camera supports the FRAME_RATE property
	PropertyInfo propInfo;
	propInfo.type = FRAME_RATE;
	//Getting the Frame rate Property
	error = cam.GetPropertyInfo(&propInfo);

	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
	//Setting AutoExposure Off
	Property prop;
	prop.type = AUTO_EXPOSURE;
	prop.autoManualMode = false;
	prop.onOff = false;
	error = cam.SetProperty(&prop);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
 	//Setting AutoWhiteBalance OFf
	prop.type = WHITE_BALANCE;
	prop.autoManualMode = false;
	prop.onOff = false;
	error = cam.SetProperty(&prop);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	//Setting AutoGain OFf
	prop.type = GAIN;
	prop.autoManualMode = false;
	prop.onOff = false;
	prop.absValue = GAIN_VALUE;
	error = cam.SetProperty(&prop);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// shutter code 
	prop.type = SHUTTER;
	prop.onOff = true;
	prop.autoManualMode = false;
	prop.absControl = true; 
	prop.absValue = SHUTTER_SPEED_VALUE; // changes in this value changes shutter speed. It is in milliseconds
	error = cam.SetProperty(&prop);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	/*

	if (ShutterInput == 1)
	{
		//cout << " shutter input 1 detected" << endl;
		//cin.ignore();
		ExtendedShutterType shutterType = NO_EXTENDED_SHUTTER;
	}

	else if ((propInfo.present == true) && (ShutterInput != 1))
	{	cout<<"entereing in shutter"<<endl;

		//Setting FRAME_RATE Off
		prop.type = FRAME_RATE;
		error = cam.GetProperty(&prop);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}
		cin.ignore();
		prop.autoManualMode = false;
		prop.onOff = false;

		error = cam.SetProperty(&prop);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

		ExtendedShutterType shutterType = GENERAL_EXTENDED_SHUTTER;

		// Set the shutter property of the camera
		prop.type = SHUTTER;
		error = cam.GetProperty(&prop);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

		prop.autoManualMode = false;
		prop.absControl = true;
		prop.absValue = ShutterInput;

		error = cam.SetProperty(&prop);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

		cout << "Shutter time set to " << fixed << setprecision(2) << ShutterInput << "ms" << endl;

	}
*/

#ifndef SOFTWARE_TRIGGER_CAMERA
	// Check for external trigger support
	TriggerModeInfo triggerModeInfo;
	error = cam.GetTriggerModeInfo(&triggerModeInfo);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	if (triggerModeInfo.present != true)
	{
		cout << "Camera does not support external trigger! Exiting..." << endl;
		return -1;
	}

	#endif

	// Get current trigger settings
	TriggerMode triggerMode;
	error = cam.GetTriggerMode(&triggerMode);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Set camera to trigger mode 0

	triggerMode.onOff = true;
	triggerMode.mode = 0;
	triggerMode.parameter = 0;

#ifdef SOFTWARE_TRIGGER_CAMERA
	// A source of 7 means software trigger
	triggerMode.source = 7;
#else
	// Triggering the camera externally using source 0.
	triggerMode.source = 0;
#endif

	error = cam.SetTriggerMode(&triggerMode);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	//const unsigned int k_TriggerMode = 0x830;
	//const unsigned int k_fireValu = 0x10;
	//error = cam.WriteRegister(k_TriggerMode, k_fireValu);//Writing the register 62Ch with value 1 in all 32 bits

	//Checking for error

	// Poll to ensure camera is ready
	bool retVal = PollForTriggerReady(&cam);
	//retVal = PollForTriggerReady(&cam);
	if (!retVal)
	{
		cout << endl;
		cout << "Error polling for trigger ready!" << endl;
		return -1;
	}
	
	cout << "camera ready" << endl;
	// Get the camera configuration
	FC2Config config;
	error = cam.GetConfiguration(&config);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Set the grab timeout to 5 seconds
	config.grabTimeout = 5000;

	// Set the camera configuration
	error = cam.SetConfiguration(&config);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Set the camera to use buffer
	//cam.WriteRegister(0x12E8, 0x80000F00);// Clear buffer
	//cam.WriteRegister(0x12E8, 0x82000F00);// Enable buffer

	return 0;
}

int Camera11::cam_trigger()
{
	cout<<"Trigger "<<cam_num<<endl;
	//Error error = cam.StartCapture();
#ifdef SOFTWARE_TRIGGER_CAMERA
	// Check that the trigger is ready
	bool retVal = PollForTriggerReady(&cam);
	//cout << "Press the Enter key to initiate a software trigger" <bbbbbbbbbb< endl;
	//cin.ignore();
	// Fire software trigger
	bool retVal1 = FireSoftwareTrigger(&cam);

	//cam.StopCapture();
	cout<<"trigger done"<<endl;
	if (!retVal1)
	{
		cout << endl;
		cout << "Error firing software trigger" << endl;
		return -1;
	}
	else
		return 0;
#endif
}
int Camera11::cam_grab_save(string filename_prefix)
{
	//cam.WriteRegister(0x12E8, 0x82000F01);
	//t = get_time();
	Error error = cam.RetrieveBuffer(&image);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	//cout << "Grabbed Image for " << camInfo.serialNumber << endl;

	//Create a converted Image
	Image convertedimage;

	//Converting the raw image to pgm
	error = image.Convert(PIXEL_FORMAT_RGB, &convertedimage);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Creating a filename
	// Issue: works fine whe directly accept the filename prefix from putty console, but failed when using ssh.net.
	// But the string contains the same content
	string file_name_cur = "/home/odroid/pheno3v2/photos/";
	//file_name_cur.append(filename_prefix);
	// int filename_num = stoi(filename_prefix);
	// string file_name_pfx = to_string(filename_num);
	file_name_cur.append(filename_prefix);
	file_name_cur.append("_");
	file_name_cur.append(cam_num);
	file_name_cur.append(".bmp");

	//saving the image
	cout<<"filename: "<<file_name_cur<<endl;
	
	// std::cout<<"Path chars: "<<file_name_cur.length()<<" as int:";
	// for(int i = 0; i < file_name_cur.length(); i++)
		// std::cout<<(int)file_name_cur[i]<<" ";
	// std::cout<<endl;
	
	error = convertedimage.Save(file_name_cur.c_str());
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
	return 0;
}

// Turn trigger mode off.
int Camera11::cam_disconnect()
{
	TriggerMode triggerMode;
	triggerMode.onOff = false;
	Error error = cam.SetTriggerMode(&triggerMode);
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
	cout << endl;
	cout << "Finished grabbing images" << endl;

	// Stop capturing images
	error = cam.StopCapture();
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	// Disconnect the camera
	error = cam.Disconnect();
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
}						



int CameraPair::camPair_connect()
{
	pcam1 = new Camera11();
	pcam2 = new Camera11();
	Error error1, error2, errorG;
	//To print the Build Info
	//PrintBuildInfo();
	
	//
	// Class BusManager used to find the number of cameras in the Bus
	//
	unsigned int numCameras;
	int init_tries = 0;
	while(init_tries < INIT_MAX_TRIES)
	{
		init_tries ++;
		//Getting the number of cameras in the bus
		errorG = busMgr.GetNumOfCameras(&numCameras);

		//Checking for Error
		if (errorG != PGRERROR_OK)
		{
			PrintError(errorG);
			return -1;
		}

		cout << "Number of cameras detected: " << numCameras << endl;
		
		//To check if atleast one camera is attached
		if (numCameras < 2)
		{
			cout << "Insufficient number of cameras... exiting, tries: "<< init_tries << endl;
			continue;
		}
		
		//
		//Gets the serial number of the camera connected by the index
		//
		error1 = busMgr.GetCameraFromIndex(0, &pcam1->guid);
		error2 = busMgr.GetCameraFromIndex(1, &pcam2->guid);

		//Checking for error
		if (error1 != PGRERROR_OK || error2 != PGRERROR_OK)
		{
			PrintError(error1);
			PrintError(error2);
			return -1;
		}

		// Connect to a camera with the serial number fetched
		int stat1 = pcam1->cam_connect();
		int stat2 = pcam2->cam_connect();
	
		if (stat1 <0 || stat2 <0)
		{
			std::cout<<"Connect camera error: L "<<stat1<<" R "<<stat2<<std::endl;
			return -1;
		}
		else
			return 0;
	}
	return -1;
}

int CameraPair::camPair_init()
{
	int stat1 = pcam1->cam_init();
	int stat2 = pcam2->cam_init();
	if (stat1 <0 || stat2 <0)
	{
		std::cout<<"Cam pair init error: L "<<stat1<<" R "<<stat2<<std::endl;
		return -1;
	}
	pcam1->cam.StopCapture();
	pcam2->cam.StopCapture();
	//const Camera* p_cam[2] = {&pcam1->cam, &pcam2->cam};
	// Camera is ready, start capturing images
	cout << "Cameras capture start l" <<endl;
	//Error error = Camera::StartSyncCapture(2, p_cam);
	Error error = pcam1->cam.StartCapture();
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
	// //pcam1->cam.StopCapture();
	cout << "Cameras capture start r" <<endl;
	error = pcam2->cam.StartCapture();
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}
	// //pcam2->cam.StopCapture();
	cout << "Cameras capture started" << endl;
// #ifdef SOFTWARE_TRIGGER_CAMERA
	// if (!CheckSoftwareTriggerPresence(&pcam1->cam) && !CheckSoftwareTriggerPresence(&pcam2->cam))
	// {
		// cout << "SOFT_ASYNC_TRIGGER not implemented on this camera!  Stopping application" << endl;
		// return -1;
	// }
// #else
	// cout << "Trigger the camera by sending a trigger pulse to GPIO" << triggerMode.source << endl;
// #endif
	
	return 0;
}

int CameraPair::camPair_capture(string filename_prefix)
{
	
	int stat1 = pcam1->cam_trigger();
	int stat2 = pcam2->cam_trigger();
	
	if (stat1 <0 || stat2 <0)
	{
		std::cout<<"Cam pair capture error: L "<<stat1<<" R "<<stat2<<std::endl;
		return -1;
	}
	
	stat1 = pcam1->cam_grab_save(filename_prefix);
	stat2 = pcam2->cam_grab_save(filename_prefix);
	
	if (stat1 <0 || stat2 <0)
	{
		std::cout<<"Cam pair save error: L "<<stat1<<" R "<<stat2<<std::endl;
		return -1;
	}
	return 0;
}

int CameraPair::camPair_disconnect()
{
	pcam1->cam_disconnect();
	pcam2->cam_disconnect();
	return 0;
}



void PrintError(Error error)
{
	error.PrintErrorTrace();
}
