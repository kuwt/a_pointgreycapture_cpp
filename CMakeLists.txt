cmake_minimum_required(VERSION 3.0.0)
project(pointgreyCapture VERSION 0.1.0)

# opencv
find_package( OpenCV REQUIRED )
find_package(pylon REQUIRED)
find_package(protobuf REQUIRED)
find_package(ZeroMQ REQUIRED)

#add_definitions(-std=c++11)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")


add_executable(pointgreyCapture
"./src/PointGreyCam.cpp"
"./src/stereocapturemain.cpp"
)

add_executable(pointgreyServer
"./src/PointGreyCam.cpp"
"./src/imagepack.pb.cc"
"./src/stereocaptureServermain.cpp"
)

target_link_libraries( pointgreyCapture "${OpenCV_LIBS}" "${PYLON_LIBRARIES})
target_link_libraries( pointgreyServer "${OpenCV_LIBS}" ${PROTOBUF_LIBRARIES} "${PYLON_LIBRARIES} zmq)