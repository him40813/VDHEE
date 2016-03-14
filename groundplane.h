#ifndef GROUNDPLANE_H
#define GROUNDPLANE_H

#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

class GroundPlane
{
public:
    GroundPlane();
    void readCameraMatrix();

    void findCircle();

    void calPlaneEquation();

    void imgCord2CameraCord();

    void getPointAtGround();

};

#endif // GROUNDPLANE_H
