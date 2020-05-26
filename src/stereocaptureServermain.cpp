#ifndef POINTGREYCAMERA
#define POINTGREYCAMERA
#endif


#include "PointGreyCam.h"

#include <string.h>
#include <stdio.h>
#include <chrono>  // for high_resolution_clock
#include <math.h>
#include <opencv2/opencv.hpp>
#include <zmq.hpp>
#include "imagepack.pb.h"



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
	float exposuretime = 5; //ms
	std::string serverport = "5555";
	bool isUsedAllDevices = true;
	std::vector<unsigned int> snlist;

	if (argc > 1) // 1 parameter
	{ 
		exposuretime = std::atof(std::string(argv[1]).c_str());
	}
	if (argc > 2)
	{
		serverport = std::string(argv[2]).c_str();
	}
	if (argc > 3)
	{
		isUsedAllDevices = false;
		std::vector<std::string> tmplist = split(std::string(argv[3]), ";");
		for (int i = 0; i < tmplist.size(); ++i)
		{
			snlist.push_back(std::atof(tmplist[i].c_str()));
			std::cout << "snlist = " << snlist[i] << "\n";
		}
	}

	std::cout << "exposuretime = " << exposuretime << "\n";
	std::cout << "serverport = " << serverport << "\n";
	std::cout << "isUsedAllDevices = " << isUsedAllDevices << "\n";
	for (int i = 0; i < snlist.size(); ++i)
	{
		std::cout << "sn = " << snlist[i] << "\n";
	}

	/****** start service **********/
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	zmq::context_t m_context = zmq::context_t(1);
	zmq::socket_t *m_pSock;
	m_pSock = new zmq::socket_t(m_context, ZMQ_REP); // rep for server
	std::string server_ip = "tcp://*:" + serverport;
	m_pSock->bind(server_ip);

	/****** start camera **********/
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

	/************ service loop ***************/
	while (true)
	{
		std::cout << "\r" <<"streaming...";  //inplace print
		zmq::message_t request;
		if (m_pSock->recv(&request, 0))
		{
			//std::cout << "receive message...\n";
			/************ receive message ***************/
			std::string  msgStr = std::string((char*)request.data(), request.size());
			if (msgStr == "imageRequest")
			{
				/************ service ***************/
				std::vector<cv::Mat> capturedImages;
				pgCam.GrabOneImage(capturedImages);

				/************ prepare reply ***************/
				imagepack sendPack;
				for (int i = 0; i < capturedImages.size(); ++i)
				{
					cv::Mat img = capturedImages[i];
					imagepack_Mat* sendMat = sendPack.add_imgs();
					(*sendMat).set_width(img.size().width);
					(*sendMat).set_height(img.size().height);
					(*sendMat).set_image_data((char *)img.data, sizeof(uchar) * img.size().width * img.size().height);
					//std::string s1 = (*sendMat).SerializeAsString();
					//std::cout << "s1.size() = " << s1.size() << "\n";
				}
				std::string s = sendPack.SerializeAsString();

				/************ send reply  ***************/
				zmq::message_t message(s.size());
				memcpy(message.data(), s.data(), s.size());
				m_pSock->send(message);
			}
		}
	}

	std::cout << "press to continue \n";
	getchar();

	/****** end *****/
	m_pSock->close();
	delete m_pSock;

	pgCam.GrabImageStop();
	pgCam.CloseDevice();

	return 0;
}