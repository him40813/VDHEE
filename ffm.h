#ifndef FFM_H
#define FFM_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "ui_mainwindow.h"
#include "tools.h"
#include <opencv2/features2d.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/xfeatures2d.hpp>

#include <display.h>
#include "groundplane.h"

class FFM
{
public:
    FFM(cv::Ptr<display> d,int r,int dis,int coutz,cv::Ptr<GroundPlane> gp);
    void process(cv::
                 Mat ff,cv::Mat frame);
    int getMFCount();
    int getSFCount();
    int findNearest(cv::Point x,cv::Mat nz);
    int findNearestMatch(int rowNum,Mat nz);
    int findNearestMatch2(int index,const Mat nz);
    double calDis(cv::Point xx1,cv::Point xx2);
    void addNewSFF(cv::Mat nz);
    Mat extractKeyPoint(std::vector<cv::KeyPoint> kp);
    Mat extractMatKeyPoint(Mat kp);
    void calculateMatch(Mat nz);
    void deleteFF();
    void clearNSort();
    std::vector<int> ffUsed,mf,del,nzUsed;
    std::vector<cv::KeyPoint> start,curr;

    cv::Mat frame,currDes,nzDes;
    std::vector< DMatch > matches;
    std::vector<std::vector< DMatch > > mmm;
    int cResist,mResist;
    double R,D;
    cv::Ptr<display> d;

    cv::Ptr<GroundPlane> gp;

    Ptr<xfeatures2d::SURF> surf;
    int matchType;
};

#endif // FFM_H
