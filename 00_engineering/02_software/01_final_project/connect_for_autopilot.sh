#!/bin/bash

# /camerainterface/output/image
# /motorsinterface/input/motors
# /wizard/output/motors
# /wizard/output/autopilot
# /wizard/input/image
# /autopilot/input/autopilot
# /autopilot/input/image
# /autopilot/output/motors
# /recorder/input/camera
# /recorder/input/motors

# camera interface

# wizard
yarp connect /camerainterface/output/image /wizard/input/image

# motors interface
yarp connect /autopilot/output/motors /motorsinterface/input/motors

# recorder

# autopilot
yarp connect /wizard/output/autopilot /autopilot/input/autopilot
yarp connect /camerainterface/output/image /autopilot/input/image

