#ifndef GROUNDPLANE_H
#define GROUNDPLANE_H

#include <opencv2/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace cv;
using namespace std;

class GroundPlane
{
public:
    Mat cm,dc;

    double a,b,c,d;

    GroundPlane();

    void readCameraMatrix();

    bool readCameraMatrix(const string filename);

    vector<Point2f> findCircle(Mat im);

    void calPlaneEquation();

    Point3d imgCord2CameraCord(Point2d imgCord);

    Point3d getPointAtGround(Point2d imgCord);

    double calDis3D(Point3d a,Point3d b);

    double calDis3DFrom2D(Point2d a,Point2d b);

    bool saveGroundParam();

    bool saveGroundParam(const string x);

    bool readGroundParam();

    bool readGroundParam(const string x);

    bool writeSetting(const string x);

    vector<Point2f> corners;

private:
    int eachDis,size1,size2;


    Mat rvec,tvec;
};

#endif // GROUNDPLANE_H
