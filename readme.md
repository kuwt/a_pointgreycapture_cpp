# GOAL:
This repo serves as the software interface operating one or more pointgrey cameras


# Design feature
Design feature: 
1. a standalone class that can perform pointgrey software trigger grabbing
2. support multiple cams

# Organization
All src code is found in ./src. The architecture is that pointgreycam is the core class. stereocaptureServermain and stereocapturemain are two testing code using the core class.
With stereocapturemain, you can make a test program that capture images and save images, with the support of multiple cameras. With stereocaptureServerMain, you can make a test program that serves its capture images through a zmq socket.


# Installation
Supports windows and linux.

# Dependency 
1. opencv3.2.0
2. protobuf 3.6.1
3. zeromq
4. flycapture2

### linux (support capturing only)
1. Install flycapture2 linux
2. build and run
```
mkdir build
cd build
cmake ..
make
./pointgreyCapture or ./pointgreyServer 
```
### windows (support capturing and capture server)
1. start drivePointgrey.sln with vs2015
2. put third party library to ./3rb_lib
2. build and run vs_stereoCapture project or vs_captureServer

### Note
if linux has limited usb bandwidth,
1. sudo sh –c 'echo -1 > /sys/module/usbcore/parameters/autosuspend' 
2. change to su and # echo 1000 > /sys/module/usbcore/parameters/usbfs_memory_mb
