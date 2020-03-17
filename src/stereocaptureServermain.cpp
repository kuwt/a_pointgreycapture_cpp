#ifndef POINTGREYCAMERA
#define POINTGREYCAMERA
#endif


#include "FlyCapture2.h"
#include "PointGreyCam.h"

#include <string.h>
#include <stdio.h>
#include <chrono>  // for high_resolution_clock
#include <math.h>
#include <opencv2/opencv.hpp>
#include <zmq.hpp>
#include "imagepack.pb.h"

using namespace FlyCapture2;
using namespace std;


int main(int argc, char *argv[])
{

	float exposuretime = 0; //ms
	if (argc < 2)
	{
		// default para
		std::cout << "using default para\n";

		exposuretime = 5;
		std::cout << "exposuretime = " << exposuretime << "\n";
	}
	else
	{
		exposuretime = std::atof(std::string(argv[1]).c_str());
		std::cout << "exposuretime = " << exposuretime << "\n";
	}

	/****** start service **********/
	GOOGLE_PROTOBUF_VERIFY_VERSION;
	zmq::context_t m_context = zmq::context_t(1);
	zmq::socket_t *m_pSock;
	m_pSock = new zmq::socket_t(m_context, ZMQ_REP); // rep for server
	m_pSock->bind("tcp://*:5555");

	/****** start camera **********/
	PointGreyCam pgCam;
	pgCam.EnumateAllDevices();
	pgCam.OpenDevice();
	pgCam.SetExposureTimeRaw(exposuretime);
	pgCam.GrabImageStart();

	/************ service loop ***************/
	while (true)
	{
		std::cout << "waiting connection...\n";
		zmq::message_t request;
		if (m_pSock->recv(&request, 0))
		{
			std::cout << "receive message...\n";
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
					std::string s1 = (*sendMat).SerializeAsString();
					std::cout << "s1.size() = " << s1.size() << "\n";
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

	m_pSock->close();
	delete m_pSock;

	pgCam.GrabImageStop();
	pgCam.CloseDevice();

	return 0;
}