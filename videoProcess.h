#ifndef VIDEOPROCESS_H
#define VIDEOPROCESS_H

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <string>
#include <iostream>
#include "bfm.h"
#include "tools.h"
#include "ui_mainwindow.h"
#include <opencv2/features2d.hpp>
#include <time.h>
#include <display.h>
#include "ffm.h"
#include "dialogrange.h"


class videoProcess
{
public:


    videoProcess(Ui::MainWindow *m,cv::VideoCapture v);
    void setVid(cv::VideoCapture v);
    void base();
    void process();
    void stopProcess();
    void pause();
    void step();
    DialogRange dr;



protected:
    cv::VideoCapture vid;


private:
    int cIn,countIn;
    bool stop;
    bool p;
    bool s;
    bool human;
    bool setupRange;
    cv::Mat frame,realFrame,FF;
    std::vector<cv::KeyPoint> curr;
    Ui::MainWindow *m;
    cv::Ptr<BFM> b;
    cv::Ptr<FFM> ffm;
    cv::Ptr<display> d;
    int h;
    int w;
    int n;//number of frame
    int cin,cout;
};

#endif // VIDEOPROCESS_H
