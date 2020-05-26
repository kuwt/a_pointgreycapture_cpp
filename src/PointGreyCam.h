

#include <opencv2/opencv.hpp>
#include "FlyCapture2.h"

using namespace FlyCapture2;

class PointGreyCam
{
public:
    PointGreyCam();
    ~PointGreyCam();

public:
    int EnumateAllDevices();
	std::vector<unsigned int> getAvailableSNs();
    int OpenDevice(const std::vector<unsigned int> &SNList);
	int CloseDevice();

	int GrabImageStart();
	int GrabOneImage(std::vector<cv::Mat>& capImages);
	int GrabImageStop();

	int  SetExposureTimeRaw(int64_t nValue);

private:
	std::vector<PGRGuid> m_DeviceGuidList; //existing device list
	std::vector<unsigned int> m_ExistingSNList;

	Camera** ppCameras;
	unsigned int m_numCams = 0;
	FlyCapture2::Error error;
	bool bIsDeviceOpen;
	TriggerMode triggerMode;
};


