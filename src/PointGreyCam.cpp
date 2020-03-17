#include "PointGreyCam.h"

PointGreyCam::PointGreyCam(void)
{
    m_imageCnt=0;
    m_nCurrentIndex = -1;
    m_bSaveImage = false;
    bIsUSB3 = false;
    m_pDataBuf = 0;
    m_bOpenDevice = 0;
}

PointGreyCam::~PointGreyCam(void)
{
    if(m_pDataBuf){
        delete m_pDataBuf;
        m_pDataBuf = 0;
    }
}

int PointGreyCam::EnumateAllDevices()
{
    printf(" Enumate all the devices! \n");
    
    BusManager busMgr;
	unsigned int numCams;
    busMgr.GetNumOfCameras(&numCams);

    for(unsigned int i=0; i<numCams;i++)
	{
        PGRGuid guid;
        busMgr.GetCameraFromIndex(i,&guid);
        g_DeviceGuidList.push_back(guid);
    }
    g_nTotalDeviceNum = numCams;
	m_numCams = numCams;
	std::cout << " Camera Num: " << numCams << std::endl;

    return numCams;
}

bool PointGreyCam::OpenDevice()
{
	ppCameras = new Camera*[m_numCams];
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		ppCameras[i] = new Camera();
		m_nCurrentIndex = i;
		PGRGuid guid = g_DeviceGuidList[m_nCurrentIndex];
		error = ppCameras[i]->Connect(&guid);
		std::cout << " Camera " << i << " Connected. " << std::endl;
	}
	std::cout << std::endl;

	if (error != PGRERROR_OK)
	{
		std::cout << "" << std::endl;
		return false;
	}
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		
		triggerMode.onOff = false;
		ppCameras[i]->SetTriggerMode(&triggerMode);

	}
	return true;
  
    
}

bool PointGreyCam::CloseDevice()
{
    bool btn = false;
    //if (m_nCurrentIndex>=0 && m_nCurrentIndex<g_nTotalDeviceNum)
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		if (ppCameras[i]->IsConnected())
		{
			ppCameras[i]->Disconnect();
		}
	}
        
    return btn;
}

CString PointGreyCam::GetDeviceModelName()
{
    char* m_modelName;
    //if (m_nCurrentIndex>=0 && m_nCurrentIndex<g_nTotalDeviceNum)
    {

        CameraInfo m_pCamInfo;
        m_InstantCamera.GetCameraInfo(&m_pCamInfo);
        // std::cout<<"modeName: "<<m_pCamInfo.modelName<<std::endl;
        m_modelName=m_pCamInfo.modelName;


    }
    return std::string(m_modelName);
}

CString PointGreyCam::GetDeviceSN()
{
    
	unsigned int m_serialNumber;
    //if (m_nCurrentIndex>=0 && m_nCurrentIndex<g_nTotalDeviceNum)
    {
        CameraInfo m_pCamInfo;
        m_InstantCamera.GetCameraInfo(&m_pCamInfo);
       // std::cout<<"serialNumber: "<<m_pCamInfo.serialNumber<<std::endl;
        m_serialNumber= m_pCamInfo.serialNumber;
    }
    return std::to_string(m_serialNumber);

}

int64_t PointGreyCam::GetWidth()
{
   int64_t w=1280;
   return w;
}

int64_t PointGreyCam::GetHeight()
{
    int64_t h=1024;
    return h;

}

bool PointGreyCam::GrabOneImage(std::vector<cv::Mat>& capImages)
{
    bool btn=false;

	for (unsigned int i = 0; i < m_numCams; i++)
    {
        Image rawImage,conertedImage;
		ppCameras[i]->RetrieveBuffer(&rawImage);
		rawImage.Convert(PIXEL_FORMAT_MONO8, &conertedImage);
		//std::cout << " Cam "<<	i <<" Image Captured. " << std::endl;
		cv::Mat tempImage = cv::Mat(conertedImage.GetRows(), conertedImage.GetCols(), CV_8UC1, conertedImage.GetData()).clone();
		capImages.push_back(tempImage);

		//m_pDataBuf= rawImage.GetData();
        btn=true;
    }
    return btn;

}

bool PointGreyCam::GrabImageStart(){
    bool btn=false;
	
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		ppCameras[i]->StartCapture();
		std::cout << " Cam " << i << " Start Capture. " << std::endl;

	}

    btn=true;	
    return btn;
}

bool PointGreyCam::GrabImageStop()
{
    bool btn=false;
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		ppCameras[i]->StopCapture();
		std::cout << "Cam " << i << " Stop Capture. " << std::endl;
	}
    btn=true;
    return btn;
}

bool PointGreyCam::SetExposureTimeRaw(int64_t nValue)
{
    bool btn=false;
    Property props;
    //SHUTTER
    props.type=SHUTTER;
    props.autoManualMode=false;
    props.absControl=true;
    props.absValue=nValue;
    props.onOff=true;
    props.onePush=false;
    //props.valueA=nValue;
	for (unsigned int i = 0; i < m_numCams; i++)
	{
		ppCameras[i]->SetProperty(&props);

	}
	btn = true;
	return btn;
}



