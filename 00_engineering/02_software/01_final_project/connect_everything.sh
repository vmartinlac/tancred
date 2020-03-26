#!/bin/bash

# connect wizard
#yarp connect /camerainterface/raw_image_output /wizard/image_input
#yarp connect /wizard/motors_output /motorsinterface/input

# connect recorder
yarp connect /camerainterface/raw_image_output /recorder/camera_input
yarp connect /wizard/motors_output /recorder/motors_input

