
#include <ostream>
#include <iostream>
#include <istream>
#include <Vector>
#include <opencv2/opencv.hpp>
#include "FlyCapture2.h"

#define CString std::string
using namespace FlyCapture2;
//using namespace cv;

static std::vector<PGRGuid> g_DeviceGuidList;
static int g_nTotalDeviceNum = 0;

class PointGreyCam
{
public:
    PointGreyCam(void);
    ~PointGreyCam(void);

public:
    int  EnumateAllDevices();
    //int  GetDeviceIndexFromUserID(CString UserID);
    int m_imageCnt;
    //bool SetDeviceIndex(int nIndex);

    CString GetDeviceSN();
    //CString GetDeviceUserID();

    CString GetDeviceModelName();

    bool OpenDevice();
    bool CloseDevice();
    //bool DestroyDevice();

    int64_t GetWidth();
    int64_t GetHeight();

    bool GrabImageStart();
    //bool GrabImageStartContinuousThread();
    bool GrabOneImage(std::vector<cv::Mat>& capImages);
    bool GrabImageStop();


    bool  SetExposureTimeRaw(int64_t nValue);
    //int64_t GetExposureTimeRaw();
    unsigned char* m_pDataBuf;
    unsigned char* GrabOneImageData();

private:
    int m_nCurrentIndex;
	//static unsigned int m_numCams;
	FlyCapture2::Error error;
    //Create a camera object
    Camera m_InstantCamera;
	Camera** ppCameras;
    // Create a target image.
    Image m_targetImage;
    bool m_bSaveImage;
    bool bIsUSB3;
    bool m_bOpenDevice;
	unsigned int m_numCams = 1;
	TriggerMode triggerMode;
};


