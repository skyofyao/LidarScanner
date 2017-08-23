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

string Camera11::get_time()
{
time_t tt;
time(&tt);
stringstream ss;
ss << tt;
string ts = ss.str();
cout << ts << "is the time" << endl;
return ts;
}

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

void Camera11::PrintCameraInfo(CameraInfo* pCamInfo)
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

void Camera11::PrintError(Error error)
{
	error.PrintErrorTrace();
}

//
//Start of Important program
//To check the presence of a Software trigger
//Need to use Software Trigger mode to get input from the main computer through software
//

bool Camera11::CheckSoftwareTriggerPresence(Camera* pCam) //Camera Calss should be used to manipulate any registers in the system
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
bool Camera11::PollForTriggerReady(Camera* pCam)
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

bool Camera11::FireSoftwareTrigger(Camera* pCam)
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
//
//Main Program
//

//int main(int /*argc*/, char** /*argv*/)
//int cam_init()
//{
	
			
int Camera11::cam_init()
{
	
	ostringstream filename;
	
	//To print the Build Info
	PrintBuildInfo();
	

	
	//
	// Class BusManager used to find the number of cameras in the Bus
	//

	

	//Getting the number of cameras in the bus
	error = busMgr.GetNumOfCameras(&numCameras);

	//Checking for Error
	if (error != PGRERROR_OK)
	{
		PrintError(error);
		return -1;
	}

	cout << "Number of cameras detected: " << numCameras << endl;

	//To check if atleast one camera is attached
	if (numCameras < 1)
	{
		cout << "Insufficient number of cameras... exiting" << endl;
		return -1;
	}

	for (int z = 0; z<numCameras; z++)
	
	{
		cout << "running  loop" << endl; 

		//PGRGuid guid;
		//
		//Gets the serial number of the camera connected by the index
		//
		error = busMgr.GetCameraFromIndex(z, &guid);

		//Checking for error
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}



		// Connect to a camera with the serial number fetched
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
		//const unsigned int k_cameraPower = 0x610;
		//const unsigned int k_powerVal = 0x80000000;
		error = cam.WriteRegister(k_cameraPower, k_powerVal);

		//Check for errors
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
		//CameraInfo camInfo;
		cout << "getting camera info" << endl;
		error = cam.GetCameraInfo(&camInfo);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}
			
		//Set Cam_num variable according to serialNumber
		if (camInfo.serialNumber == 15435734)
		{cam_num = "left";}
		else if (camInfo.serialNumber == 15435724) 
		{cam_num = "right";}
		
		//
		//Main Aim is to set the width and height
		//To check whether the format 7 mode is supported 
		//will give error if not	
		// Query for available Format 7 modes
		//
		//Format7Info fmt7Info;
		//bool supported;
		fmt7Info.mode = k_fmt7Mode; // will return true if format7 is supported
		error = cam.GetFormat7Info(&fmt7Info, &supported);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

	//	PrintFormat7Capabilities(fmt7Info); // Prints the format7 features

		if ((k_fmt7PixFmt & fmt7Info.pixelFormatBitField) == 0)
		{
			// Pixel format not supported!
			cout << "Pixel format is not supported" << endl;
			return -1;
		}

		//Setting the format7 features
		//Setting the max width and height
		//Setting the pixel format
		//Format7ImageSettings fmt7ImageSettings;
		fmt7ImageSettings.mode = k_fmt7Mode;
		fmt7ImageSettings.offsetX = 0;
		fmt7ImageSettings.offsetY = 0;
		fmt7ImageSettings.width = fmt7Info.maxWidth;
		fmt7ImageSettings.height = fmt7Info.maxHeight;
		fmt7ImageSettings.pixelFormat = k_fmt7PixFmt;

		//bool valid;
		//Format7PacketInfo fmt7PacketInfo;

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
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}


		// Check if the camera supports the FRAME_RATE property

		//PropertyInfo propInfo;
		propInfo.type = FRAME_RATE;
		//Getting the Frame rate Property
		error = cam.GetPropertyInfo(&propInfo);

		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

				//Setting AutoExposure Off
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
		error = cam.SetProperty(&prop);
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

		// shutter code 
		Property prop;
		prop.type = SHUTTER;
		prop.onOff = true;
		prop. autoManualMode = false;
		prop.absControl = true; 
		prop.absValue = 100; // changes in this value changes shutter speed. It is in milliseconds
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
		//TriggerModeInfo triggerModeInfo;
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
		//TriggerMode triggerMode;
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
		//bool retVal = PollForTriggerReady(&cam);
		retVal = PollForTriggerReady(&cam);
		if (!retVal)
		{
			cout << endl;
			cout << "Error polling for trigger ready!" << endl;
			return -1;
		}
		
		cout << "camera ready" << endl;
		// Get the camera configuration
		//FC2Config config;
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

		// Camera is ready, start capturing images
		error = cam.StartCapture();
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

#ifdef SOFTWARE_TRIGGER_CAMERA
		if (!CheckSoftwareTriggerPresence(&cam))
		{
			cout << "SOFT_ASYNC_TRIGGER not implemented on this camera!  Stopping application" << endl;
			return -1;
		}
#else
		cout << "Trigger the camera by sending a trigger pulse to GPIO" << triggerMode.source << endl;
#endif
		//Image image;
		//for (int imageCount = 0; imageCount < k_numImages; imageCount++)
		//{

#ifdef SOFTWARE_TRIGGER_CAMERA
			// Check that the trigger is ready
			PollForTriggerReady(&cam);
			//cout << "Press the Enter key to initiate a software trigger" <bbbbbbbbbb< endl;
			//cin.ignore();
			// Fire software trigger
			//bool retVal1 = FireSoftwareTrigger(&cam);
			retVal1 = FireSoftwareTrigger(&cam);
			if (!retVal1)
			{
				cout << endl;
				cout << "Error firing software trigger" << endl;
				return -1;
			}
#endif
		
//}

//int grab()
//{
			// Grab image
			t = get_time();
			error = cam.RetrieveBuffer(&image);
			if (error != PGRERROR_OK)
			{
				PrintError(error);
				continue;
			}

			cout << "Grabbed Image for " << camInfo.serialNumber << endl;

			//Create a converted Image
			//Image convertedimage;

			//Converting the raw image to pgm
			error = image.Convert(PIXEL_FORMAT_RAW8, &convertedimage);
			if (error != PGRERROR_OK)
			{
				PrintError(error);
				return -1;
			}

			// Creating a Unique Filename
			//ostringstream filename;
			filename << t << "_" << cam_num << ".pgm";

			//saving the image
			error = convertedimage.Save(filename.str().c_str());
			if (error != PGRERROR_OK)
			{
				PrintError(error);
				return -1;
			}

			cout << "..." << endl;
			filename.str("");
// Turn trigger mode off.
//int disconnect()
//{
		triggerMode.onOff = false;
		error = cam.SetTriggerMode(&triggerMode);
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

		// Turn off trigger mode
		//triggerMode.onOff = false;
		//error = cam.SetTriggerMode(&triggerMode);


		// Disconnect the camera
		error = cam.Disconnect();
		if (error != PGRERROR_OK)
		{
			PrintError(error);
			return -1;
		}

	}				// FOR LOOP END

	//cout << "Done! Press Enter to exit..." << endl;
	//cin.ignore();

}						// INIT FUNCTION END



