#pragma once

#include "FlyCapture2.h"
#include <string>
//
//Using a Software Trigger To Capture an Image
//

#define SOFTWARE_TRIGGER_CAMERA

using namespace FlyCapture2;
using namespace std;

#define INIT_MAXTRIES 5

static const float SHUTTER_SPEED_VALUE = 40;
static const float GAIN_VALUE = 0;

class Camera11
{
public:
	int cam_connect();
	int cam_init();
	int cam_capture(string filename);
	
	string cam_sub;
	PGRGuid guid;
	
	const Mode k_fmt7Mode = MODE_0;
	const PixelFormat k_fmt7PixFmt = PIXEL_FORMAT_RAW8;
	
	
	string t;
	
	
	Camera cam;
	//Number of images to take
	const int k_numImages = 1;
	Error error;

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



	string get_time();
};

class CameraPair
{
public:
	int camPair_connect();
	int camPair_init();
	int camPair_capture(string filename);
	
private:
	Camera11 cam1, cam2;
	BusManager busMgr;
};
