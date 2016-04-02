#include "display.h"

display::display(Ui::MainWindow *m,std::string axisName)
{
    this->m=m;
    this->axisName=axisName;
    cv::namedWindow(axisName,CV_WINDOW_NORMAL);
    cv::resizeWindow(axisName, 960, 720);

}



void display::setImage(cv::Mat im)
{
    this->im=im;
    circleSize=std::ceil(0.0025*im.size().width);

}

void display::drawMask(cv::Mat mask,cv::Scalar color,bool check)
{
    if (check){
        cv::Mat nz;
        cv::findNonZero(mask, nz);
        for (int i = 0; i < nz.total(); i++ ) {
            cv::circle(im,nz.at<cv::Point>(i),circleSize,color,-1);
        }
    }
}

void display::drawTrueMask(cv::Mat mask,cv::Scalar color,bool check){
    if (check){
        for(int j = 0;j < mask.rows;j++){
            for(int i = 0;i < mask.cols;i++){
                if (mask.at<uchar>(i,j)>0)
                    cv::circle(im,cv::Point(j,i),circleSize,color,-1);
            }
        }
    }
}

void display::drawLine(std::vector<cv::KeyPoint> start,std::vector<cv::KeyPoint> curr,bool check){
    if (check){
        for (int i=0;i<curr.size();i++){
            cv::line(im,curr.at(i).pt,start.at(i).pt,cv::Scalar(0,0,255));
        }
    }
}

void display::showIm()
{
    if (!im.empty())
    {
        imshow(axisName,im);
        m->frameNum->setText(frameN);
        m->FPSLabel->setText(fps);
        m->BGLabel->setText(bg);
        m->BFLabel->setText(bf);
        m->CurrFLabel->setText(cff);
        m->FFLabel->setText(ff);
        m->SFLabel->setText(sfm);
        m->MFLabel->setText(mfm);
    }
}







//Setter Param number Label
void display::setFrameN(int value)
{
    frameN = QString::fromStdString("Frame# "+tools::int2str(value));
}
void display::setFps(int value)
{
    fps = QString::fromStdString("AVG FPS: "+tools::int2str(value));
}
void display::setBg(int value)
{
    bg = QString::fromStdString("BGN "+tools::int2str(value));
}
void display::setBf(int value)
{
    bf = QString::fromStdString("BFN "+tools::int2str(value));
}
void display::setCff(int value)
{
    cff = QString::fromStdString("Current Feature: "+tools::int2str(value));
}
void display::setFf(int value)
{
    ff = QString::fromStdString("Foreground Feature: "+tools::int2str(value));
}
void display::setSfm(int value)
{
    sfm = QString::fromStdString("SFM: "+tools::int2str(value));
}
void display::setMfm(int value)
{
    mfm = QString::fromStdString("MFM: "+tools::int2str(value));
}








