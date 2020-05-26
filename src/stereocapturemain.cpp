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

inline std::vector<std::string> split(const std::string& s, std::string delimiter)
{
	std::vector<std::string> result;

	std::size_t current = 0;
	std::size_t p = s.find_first_of(delimiter, 0);

	while (p != std::string::npos)
	{
		result.emplace_back(s, current, p - current);
		current = p + 1;
		p = s.find_first_of(delimiter, current);
	}

	result.emplace_back(s, current);

	return result;
}

int main(int argc, char *argv[])
{
	/****** parameter *****/
	int startImgIdx = 0;
	std::string imageSavePath= "./img";
	float exposuretime = 50; //ms
	int display_size = 512;
	bool isUsedAllDevices = true;
	std::vector<unsigned int> snlist;

	if (argc > 2) //two parameter
	{
		startImgIdx = std::atof(std::string(argv[1]).c_str());
		imageSavePath = std::string(argv[2]).c_str();
	}
	if (argc > 3)
	{
		exposuretime = std::atof(std::string(argv[3]).c_str());
	}
	if (argc > 4)
	{
		display_size = std::atof(std::string(argv[4]).c_str());
	}
	if (argc > 5)
	{
		isUsedAllDevices = false;
		std::vector<std::string> tmplist = split(std::string(argv[5]), ";");
		for (int i = 0; i < tmplist.size(); ++i)
		{
			snlist.push_back(std::atof(tmplist[i].c_str()));
		}
	}

	std::cout << "exposuretime = " << exposuretime << "\n";
	std::cout << "imageSavePath = " << imageSavePath << "\n";
	std::cout << "startImgIdx = " << startImgIdx << "\n";
	std::cout << "display_size = " << display_size << "\n";
	std::cout << "isUsedAllDevices = " << isUsedAllDevices << "\n";
	for (int i = 0; i < snlist.size(); ++i)
	{
		std::cout << "snlist = " << snlist[i] << "\n";
	}

	/****** start *****/
	PointGreyCam pgCam;
	pgCam.EnumateAllDevices();

	if (isUsedAllDevices)
	{
		pgCam.OpenDevice(pgCam.getAvailableSNs());
	}
	else
	{
		pgCam.OpenDevice(snlist);
	}
	
	pgCam.SetExposureTimeRaw(exposuretime);
	pgCam.GrabImageStart();

	/****** loop *****/
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

	/****** end *****/
	pgCam.GrabImageStop();
	pgCam.CloseDevice();
	
	return 0;
}
