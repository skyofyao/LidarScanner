#pragma once

#include "FlyCapture2.h"
#include <string>
//
//Using a Software Trigger To Capture an Image
//

#define SOFTWARE_TRIGGER_CAMERA

using namespace FlyCapture2;
using namespace std;


class Camera11
{
public:
	string cam_num;
	string t;
	const Mode k_fmt7Mode = MODE_0;
	const PixelFormat k_fmt7PixFmt = PIXEL_FORMAT_RAW8;
	Camera cam;
	//Number of images to take
	const int k_numImages = 1;
	Error error;
	BusManager busMgr;
	unsigned int numCameras;
	PGRGuid guid;
	const unsigned int k_cameraPower = 0x610;
        const unsigned int k_powerVal = 0x80000000;
	const unsigned int millisecondsToSleep = 100;
        unsigned int regVal = 0;
        unsigned int retries = 10;
	CameraInfo camInfo;
	Format7Info fmt7Info;
        bool supported;
	Format7ImageSettings fmt7ImageSettings;
        bool valid;
        Format7PacketInfo fmt7PacketInfo;
	PropertyInfo propInfo;
	float ShutterInput = 1;
        Property prop;
	TriggerModeInfo triggerModeInfo;
	TriggerMode triggerMode;
	bool retVal;
	FC2Config config;
	Image image;
	bool retVal1;
	Image convertedimage;


void PrintBuildInfo();
void PrintCameraInfo(CameraInfo* pCamInfo);
void PrintError(Error error);
bool CheckSoftwareTriggerPresence(Camera* pCam);
bool PollForTriggerReady(Camera* pCam);
bool FireSoftwareTrigger(Camera* pCam);
void PrintFormat7Capabilities(Format7Info fmt7Info);
int cam_init();
string get_time();
};
