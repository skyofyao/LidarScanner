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
	Camera11(){}
	~Camera11(){}

	int cam_connect();
	int cam_init();
	int cam_grab_save(string filename_prefix);
	int cam_trigger();
	int cam_disconnect();
	
	PGRGuid guid;
	Image image;
	
	Camera cam;
	string cam_num;
private:
	const Mode k_fmt7Mode = MODE_0;
	const PixelFormat k_fmt7PixFmt = PIXEL_FORMAT_RAW8;


	void PrintBuildInfo();
	void PrintCameraInfo(CameraInfo* pCamInfo);
	

	void PrintFormat7Capabilities(Format7Info fmt7Info);

};

class CameraPair
{
public:
	int camPair_connect();
	int camPair_init();
	int camPair_capture(string filename_prefix);
	int camPair_disconnect();
	
private:
	const int INIT_MAX_TRIES = 5;
	Camera11* 	pcam1;
	Camera11*	pcam2;
	BusManager busMgr;
};

void PrintError(Error error);
bool CheckSoftwareTriggerPresence(Camera* pCam);
bool PollForTriggerReady(Camera* pCam);
bool FireSoftwareTrigger(Camera* pCam);