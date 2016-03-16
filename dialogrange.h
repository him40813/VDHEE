
#ifndef DIALOGRANGE_H
#define DIALOGRANGE_H

#include <QDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>
#include "tools.h"
#include "calibrater.h"
#include <opencv2/features2d.hpp>

namespace Ui {
class DialogRange;
}

class DialogRange : public QDialog
{
    Q_OBJECT

public:
    void setImageMat(cv::Mat im,VideoCapture vid);
    void setHMLPos(QPoint p);
    void calculateXYZ();

    Calibrater ccc;
    VideoCapture vid;

    explicit DialogRange(QWidget *parent = 0);
    ~DialogRange();

    int eachDis,size1,size2;
    cv::Point3f p1,p2,p3;
    cv::Point2f pp1,pp2,pp3;
    cv::Mat A,B,xx,cm,dc,cf;
    void ReadCameraMatrix();
    vector<Point2f> corners;
    //plane Equation
    vector<double> temp12,temp23,tempN;
    double a,b,c,d,t;


private slots:
    void on_cdButton_clicked();

    void on_capButton_clicked();

private:
    Ui::DialogRange *ui;
    //int a,b,c;
    int l,m,n;
    int p,q,r;
    double x,y,z;
};

#endif // DIALOGRANGE_H
