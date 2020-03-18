#
# Written by kuwingto, 17 March 2020
#

import os
import math
import random
import numpy as np
import cv2
import importlib.util
import matplotlib.pyplot as plt
import matplotlib.image as mpimg
import sys
from time import time
from time import sleep
import zmq
import imagepack_pb2



if __name__ == '__main__' :
    ##### connect to server  #####
    context = zmq.Context()
    socket = context.socket(zmq.REQ)
    socket.connect("tcp://localhost:5555")
    
    while True:
        ##### send message #####
        socket.send_string("imageRequest")
        
        ##### receive message ####
        message = socket.recv()
        #print("message recv")
        
        ##### parse message ####
        message_input = imagepack_pb2.imagepack()
        message_input.ParseFromString(message)
        
        imglist = []
        for i in range(len(message_input.imgs) ):
            #print("received image width = ", message_input.imgs[i].width)
            #print("received image height = ", message_input.imgs[i].height)

            nparr = np.frombuffer(message_input.imgs[i].image_data,dtype=np.ubyte)
            #print("data shape = ",nparr.shape)
            img = nparr.reshape(message_input.imgs[i].height,message_input.imgs[i].width)
            #print("image shape = ",img.shape)
            
            stacked_img = np.stack((img,)*3, axis=-1) ##rearrange rgb
            imglist.append(stacked_img)
        
        ##### format display ####
        numpy_horizontal_concat = np.concatenate((imglist[0], imglist[1]), axis=1) 
        
        targetHeight = 400
        scaleFactor =  float(targetHeight)/ float(numpy_horizontal_concat.shape[0]);
        targetWidth = int(numpy_horizontal_concat.shape[1] * scaleFactor);
        displayImg = cv2.resize(numpy_horizontal_concat,(targetWidth,targetHeight))
   
        #####  display ####
        cv2.imshow('window',displayImg)
        key = cv2.waitKey(1) & 0xFF
        
        sleep(0.02) #20 fps

