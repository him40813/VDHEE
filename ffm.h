#ifndef FFM_H
#define FFM_H
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <iostream>
#include "ui_mainwindow.h"
#include "tools.h"
#include <opencv2/features2d.hpp>
#include <display.h>

class FFM
{
public:
    FFM(cv::Ptr<display> d,int r,int dis,int cout);
    void process(cv::
                 Mat ff);
    int getMFCount();
    int getSFCount();
    int findNearest(cv::Point x,cv::Mat nz);
    double calDis(cv::Point xx1,cv::Point xx2);
    void addNewSFF(cv::Mat nz);
    void deleteFF();
    void clearNSort();
    std::vector<int> ffUsed,mf,del,nzUsed;
    std::vector<cv::KeyPoint> start,curr;
    int cResist,mResist;
    double R,D;
    cv::Ptr<display> d;


};

#endif // FFM_H
