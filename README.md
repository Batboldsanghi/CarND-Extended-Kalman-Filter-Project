# Extended Kalman Filter Project

[image1]: ./Docs/dataset1.png "dataset1"
[image2]: ./Docs/dataset2.png "dataset2"

## The Project results

In this project, I completed kalman filter to estimate position and speed of moving object using lidar and radar measurements. 
My px, py, vx, and vy RMSE are less than the required values [.11, .11, 0.52, 0.52]. Changes are made in 
src/FusionEKF.cpp, src/FusionEKF.h, kalman_filter.cpp, kalman_fitler.h, tools.cpp, and tools.h.

I used centos linux and modified installation scripts to work on centos 7. The installation scripts are not included.
Also, I had to install the required versions of cmake, make and gcc since the default libraries for centos 7 are much older.
The code should be compiled on ubuntu since i used exactly the same versions of cmake,make and gcc.

Red circles are blue circles are lidar and radar measurements ( radial velocity measurements are not shown).
Green markers are the car's position as estimated by the Kalman filter. It's clear that the Kalman filter is doing a good job.

Final result with dataset 1:

![alt text][image1]

Final result with dataset 2:

![alt text][image2]


