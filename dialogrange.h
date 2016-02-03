#ifndef DIALOGRANGE_H
#define DIALOGRANGE_H

#include <QDialog>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include "tools.h"
#include "calibrater.h"

namespace Ui {
class DialogRange;
}

class DialogRange : public QDialog
{
    Q_OBJECT

public:
    void setImageMat(cv::Mat im);
    void setHMLPos(QPoint p);
    void calculateXYZ();

    Calibrater ccc;


    explicit DialogRange(QWidget *parent = 0);
    ~DialogRange();

    cv::Point3i highPoint,MediumPoint,LowPoint;
    cv::Mat A,B,xx,cm;
    void ReadCameraMatrix();

private slots:
    void on_cdButton_clicked();

private:
    Ui::DialogRange *ui;
    int a,b,c;
    int l,m,n;
    int p,q,r;
    double x,y,z;
};

#endif // DIALOGRANGE_H
