#ifndef DISPLAY_H
#define DISPLAY_H
#include "ui_mainwindow.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "tools.h"

class display
{
public:
    display(Ui::MainWindow *m,std::string axisName);
    void setImage(cv::Mat im);
    void drawMask(cv::Mat mask,cv::Scalar color,bool check);
    void drawTrueMask(cv::Mat mask,cv::Scalar color,bool check);
    void drawLine(std::vector<cv::KeyPoint> start,std::vector<cv::KeyPoint> curr,bool check);

    void showIm();

    //Setter
    void setFrameN(int value);

    void setFps(int value);

    void setBg(int value);

    void setBf(int value);

    void setCff(int value);

    void setFf(int value);

    cv::Mat im,temp;

    void setSfm(int value);

    void setMfm(int value);

    bool nonShow;





private:
    Ui::MainWindow *m;

    std::string axisName;
    QString frameN,fps,bg,bf,cff,ff,sfm,mfm;
    int circleSize;

};

#endif // DISPLAY_H
