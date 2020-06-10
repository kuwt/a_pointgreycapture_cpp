#include <ostream>
#include <iostream>
#include <istream>
#include <vector>
#include "PointGreyCam.h"

PointGreyCam::PointGreyCam()
{
	bIsDeviceOpen = false;
}

PointGreyCam::~PointGreyCam()
{
}

int PointGreyCam::EnumateAllDevices()
{
    printf("Enumate all the devices! \n");
    
    BusManager busMgr;
	unsigned int numCams;
    busMgr.GetNumOfCameras(&numCams);

    for(unsigned int i = 0; i < numCams; i++)
	{
        PGRGuid guid;
        busMgr.GetCameraFromIndex(i,&guid);
		unsigned int serialNum = 0;
		busMgr.GetCameraSerialNumberFromIndex(i, &serialNum);
		m_DeviceGuidList.push_back(guid);
		m_ExistingSNList.push_back(serialNum);
	    std::cout << "serialNum = " << serialNum << "\n";
    }
	std::cout << "Number of Existing Cameras: " << numCams << std::endl;

    return 0;
}

std::vector<unsigned int> PointGreyCam::getAvailableSNs()
{
	return m_ExistingSNList;
}

int PointGreyCam::OpenDevice(const std::vector<unsigned int> &SNList)
{
	if (bIsDeviceOpen)
	{
		std::cerr << "device already opened!\n";
		return -1;
	}

	//verify the number of SN available and obtain the order 
	std::vector<int> v_order;
	for (int i = 0; i < SNList.size(); ++i)
	{
		for (int j = 0; j < m_ExistingSNList.size(); ++j)
		{
			if (SNList[i] == m_ExistingSNList[j])
			{
				m_numCams++;
				v_order.push_back(j);
			}
		}	
	}

	std::cout << "use cam num = " << m_numCams << "\n";
	ppCameras = new Camera*[m_numCams];
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		ppCameras[i] = new Camera();
		PGRGuid guid = m_DeviceGuidList[v_order[i]];
		error = ppCameras[i]->Connect(&guid);
		std::cout << " Camera " << i << " Connected. " << std::endl;
	}
	std::cout << std::endl;

	if (error != PGRERROR_OK)
	{
		std::cout << "" << std::endl;
		return -1;
	}
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		triggerMode.onOff = false;
		ppCameras[i]->SetTriggerMode(&triggerMode);
	}
	return 0;
}

int PointGreyCam::CloseDevice()
{
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		if (ppCameras[i]->IsConnected())
		{
			ppCameras[i]->Disconnect();
		}
		delete ppCameras[i];
		ppCameras[i] = 0;
	}
	delete[] ppCameras;
	ppCameras = 0;
	m_numCams = 0;
	bIsDeviceOpen = false;
    return 0;
}

int PointGreyCam::GrabOneImage(std::vector<cv::Mat>& capImages)
{

	for (unsigned int i = 0; i < m_numCams; i++)
    {
        Image rawImage,conertedImage;
		ppCameras[i]->RetrieveBuffer(&rawImage);
		rawImage.Convert(PIXEL_FORMAT_MONO8, &conertedImage);
		//std::cout << " Cam "<<	i <<" Image Captured. " << std::endl;
		cv::Mat tempImage = cv::Mat(conertedImage.GetRows(), conertedImage.GetCols(), CV_8UC1, conertedImage.GetData()).clone();
		capImages.push_back(tempImage);
    }
    return 0;

}

int PointGreyCam::GrabImageStart()
{
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		ppCameras[i]->StartCapture();
		std::cout << " Cam " << i << " Start Capture. " << std::endl;
	}
    return 0;
}

int PointGreyCam::GrabImageStop()
{
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		ppCameras[i]->StopCapture();
		std::cout << "Cam " << i << " Stop Capture. " << std::endl;
	}
    return 0;
}

int PointGreyCam::SetExposureTimeRaw(int64_t nValue)
{
    Property props;
    //SHUTTER
    props.type=SHUTTER;
    props.autoManualMode=false;
    props.absControl=true;
    props.absValue=nValue;
    props.onOff=true;
    props.onePush=false;

	for (unsigned int i = 0; i < m_numCams; i++)
	{
		ppCameras[i]->SetProperty(&props);

	}
	return 0;
}



