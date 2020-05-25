#ifndef POINTGREYCAMERA
#define POINTGREYCAMERA
#endif

#include <thread>
#include <mutex>
#include <chrono>  // for high_resolution_clock

#include "FlyCapture2.h"
#include "PointGreyCam.h"

using namespace FlyCapture2;
using namespace std;

#define MAX_CAM_NUM (20)
cv::Mat g_buffers[MAX_CAM_NUM];
static std::mutex g_mutex;

int liveStreamThread(PointGreyCam *p, int show_size)
{
	int targetHeight = show_size;

	while (1)
	{
		auto starttime = std::chrono::steady_clock::now();
		std::vector<cv::Mat> capturedImages;
		p->GrabOneImage(capturedImages);

		{
			std::lock_guard<std::mutex> lk(g_mutex);
			for (int j = 0; j < capturedImages.size(); j++)
			{

				g_buffers[j] = capturedImages[j].clone();
			}
		}

		/***** fps *****/
		auto endtime = std::chrono::steady_clock::now();
		float timeElapsed = std::chrono::duration_cast<std::chrono::milliseconds>(endtime - starttime).count();
		float fps = 1.0 * 1000 / timeElapsed;

		for (int j = 0; j < capturedImages.size(); j++)
		{
			char windowName[1024];
			sprintf_s(windowName, " Cam_%d.bmp", j);
			cv::Mat temp;
			float scaleFactor =  (float)targetHeight / (float)capturedImages[j].size().height;
			int targetWidth = capturedImages[j].size().width * scaleFactor;
			cv::resize(capturedImages[j], temp, cv::Size(targetWidth,targetHeight));

			char buffer[1024];
			snprintf(buffer, 1024, "fps = %f Hz", fps);
			cv::putText(temp, buffer,
				cvPoint(30, 30), cv::FONT_HERSHEY_COMPLEX_SMALL,
				0.8, cvScalar(200, 200, 250), 1, CV_AA);

			cv::imshow(windowName, temp);
			cv::waitKey(1);
		}
	}
	return 0;
}

int main(int argc, char *argv[])
{
	int startImgIdx = 0;
	std::string imageSavePath;
	float exposuretime = 0; //ms
	int display_size;
	if (argc < 5)
	{
		// default para
		std::cout << "using default para\n";
		startImgIdx = 0;
		std::cout << "startImgIdx = " << startImgIdx << "\n";

		imageSavePath = "./img";
		std::cout << "imageSavePath = " << imageSavePath << "\n";

		exposuretime =  50;
		std::cout << "exposuretime = " << exposuretime << "\n";

		display_size = 512;
		std::cout << "display_size = " << display_size << "\n";
	}
	else
	{
		startImgIdx = std::atof(std::string(argv[1]).c_str());
		std::cout << "startImgIdx = " << startImgIdx << "\n";

		imageSavePath = std::string(argv[2]).c_str();
		std::cout << "imageSavePath = " << imageSavePath << "\n";

		exposuretime = std::atof(std::string(argv[3]).c_str());
		std::cout << "exposuretime = " << exposuretime << "\n";

		display_size = std::atof(std::string(argv[4]).c_str());;
		std::cout << "display_size = " << display_size << "\n";

	}

	PointGreyCam pgCam;

	pgCam.EnumateAllDevices();
	pgCam.OpenDevice(pgCam.getAvailableSNs());

	pgCam.SetExposureTimeRaw(exposuretime);
	pgCam.GrabImageStart();

	std::thread t(liveStreamThread, &pgCam, display_size);

	int numOfCameras = pgCam.getAvailableSNs().size();
	int currentImgIdx = startImgIdx;
	while (1)
	{
		std::cout << "press enter to save images." << "\n";
		cin.ignore();

		// save images
		char filename[1024];

		std::vector<cv::Mat> capturedImages;
		capturedImages.resize(numOfCameras);
		{
			std::lock_guard<std::mutex> lk(g_mutex);
			for (int j = 0; j < numOfCameras; j++)
			{
				capturedImages[j] = g_buffers[j].clone();
			}
		}

		for (int k = 0; k < capturedImages.size(); k++)
		{
			sprintf_s(filename, "%s/Cam_%d_%d.bmp", imageSavePath.c_str(), k, currentImgIdx);
			cv::imwrite(filename, capturedImages[k]);
			std::cout << filename << " Saved. " << std::endl;
		}
		currentImgIdx++;
	}
	pgCam.GrabImageStop();
	pgCam.CloseDevice();
	
	return 0;
}
