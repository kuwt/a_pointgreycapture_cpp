cmake_minimum_required(VERSION 3.0.0)
project(pointgreyCapture VERSION 0.1.0)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# opencv
find_package( OpenCV REQUIRED )
find_package(FlyCapture REQUIRED)
find_package(Protobuf REQUIRED)
find_package(ZeroMQ REQUIRED)

SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -pthread")

add_executable(pointgreyCapture
"${CMAKE_CURRENT_SOURCE_DIR}/src/PointGreyCam.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/stereocapturemain.cpp"
)

add_executable(pointgreyServer
"${CMAKE_CURRENT_SOURCE_DIR}/src/PointGreyCam.cpp"
"${CMAKE_CURRENT_SOURCE_DIR}/src/imagepack.pb.cc"
"${CMAKE_CURRENT_SOURCE_DIR}/src/stereocaptureServermain.cpp"
)

target_include_directories(pointgreyCapture PRIVATE "${FLYCAPTURE2_INCLUDE_DIR}")
target_link_libraries( pointgreyCapture "${OpenCV_LIBS}" "${FLYCAPTURE2_LIBRARIES}")

target_include_directories(pointgreyServer PRIVATE "${FLYCAPTURE2_INCLUDE_DIR}")
target_link_libraries( pointgreyServer "${OpenCV_LIBS}" ${PROTOBUF_LIBRARIES} "${FLYCAPTURE2_LIBRARIES}" zmq)