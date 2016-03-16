#include "dialogrange.h"
#include "ui_dialogrange.h"

DialogRange::DialogRange(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRange)
{
    ui->setupUi(this);
    ui->imgLabel->setDialog(this);
    x=0;
    y=0;
    z=0;
    eachDis=100;//centimetre
    size1=2;//number in 1 line
    size2=3;//number of lines
}

DialogRange::~DialogRange()
{
    delete ui;
}

void DialogRange::setImageMat(cv::Mat im,VideoCapture vid){
    this->vid=vid;
    QImage qim=tools::Mat2QImage(im);
    int imh=qim.height();
    int imw=qim.width();
    ui->imgLabel->setFixedHeight(imh);
    ui->imgLabel->setFixedWidth(imw);
//    //High
//    ui->radioH->setGeometry(QRect(imw+50,50,125,25));
//    ui->textH->setGeometry(QRect(imw+50,80,70,25));
//    ui->mLabelH->setGeometry(QRect(imw+130,80,30,25));
//    ui->posLabelH->setGeometry(QRect(imw+50,110,121,19));
//    //Medium
//    ui->radioM->setGeometry(QRect(imw+50,140,125,25));
//    ui->textM->setGeometry(QRect(imw+50,170,70,25));
//    ui->mLabelM->setGeometry(QRect(imw+130,170,30,25));
//    ui->posLabelM->setGeometry(QRect(imw+50,200,121,19));
//    //Low
//    ui->radioL->setGeometry(QRect(imw+50,230,125,25));
//    ui->textL->setGeometry(QRect(imw+50,270,70,25));
//    ui->mLabelL->setGeometry(QRect(imw+130,270,30,25));
//    ui->posLabelL->setGeometry(QRect(imw+50,300,121,19));
//    //end
//    ui->buttonBox->setGeometry(QRect(50,imh+50,125,25));
//    resize(imw+200,imh+300);
    ui->imgLabel->setPixmap(QPixmap::fromImage(qim));
}

void DialogRange::setHMLPos(QPoint pp){

    if (ui->radioH->isChecked()){
        a=pp.x();
        b=pp.y();
        c=ui->textH->text().toInt();
        pp1=Point2f(a,b);
        p1=Point3f(ui->textH->text().toInt(),ui->textH_2->text().toInt(),ui->textH_3->text().toInt());
//        highPoint=cv::Point3i(a,b,c);
//        ui->posLabelH->setText("("+tools::num2str(a)+","+tools::num2str(b)+","+ui->textH->text()+")");

    }else if (ui->radioM->isChecked()){
        l=pp.x();
        m=pp.y();
        n=ui->textM->text().toInt();
        pp2=Point2f(l,m);
        p2=Point3f(ui->textM->text().toInt(),ui->textM_2->text().toInt(),ui->textM_3->text().toInt());
//        MediumPoint=cv::Point3i(l,m,n);
//        ui->posLabelM->setText("("+tools::num2str(l)+","+tools::num2str(m)+","+ui->textM->text()+")");

    }else if (ui->radioL->isChecked()) {
        p=pp.x();
        q=pp.y();
        r=ui->textL->text().toInt();

        pp3=Point2f(p,q);
        p3=Point3f(ui->textL->text().toInt(),ui->textL_2->text().toInt(),ui->textL_3->text().toInt());
//        LowPoint=cv::Point3i(p,q,r);
//        ui->posLabelL->setText("("+tools::num2str(p)+","+tools::num2str(q)+","+ui->textL->text()+")");
//        calculateXYZ();
    }else{

        ui->posLabelTest->setText("("+tools::num2str(pp.x())+","+tools::num2str(pp.y())+","+tools::num2str((pp.x()*x)+(pp.y()*y)+z)+")");
    }
}

void DialogRange::calculateXYZ(){
//    ax+by+cz+d=0
//    lx+my+nz+k=0
//    px+qy+rz+s=0



//    double det=tools::Determinant3D(a,b,1,l,m,1,p,q,1);

//    double det1=tools::Determinant3D(c,b,1,n,m,1,r,q,1);
//    double det2=tools::Determinant3D(a,c,1,l,n,1,p,r,1);
//    double det3=tools::Determinant3D(a,b,c,l,m,n,p,q,r);

//    x=det1/det;
//    y=det2/det;
//    z=det3/det;

//    ui->textH->setText(tools::num2str(x));
//    ui->textM->setText(tools::num2str(y));
//    ui->textL->setText(tools::num2str(z));
}

void DialogRange::on_cdButton_clicked()
{
//    ccc.start();
    ReadCameraMatrix();

    cout<<cm<<endl;
    vector<Point3f> pointAt3d;
    pointAt3d.push_back(p1);
    pointAt3d.push_back(p2);
    pointAt3d.push_back(p3);
    vector<Point2f> pointAt2d;
    pointAt2d.push_back(pp1);
    pointAt2d.push_back(pp2);
    pointAt2d.push_back(pp3);
    Mat rvec,tvec;

    vector<Point3f> corner3d;
//    for (int i=size2-1;i>=0;i--){
//        for(int j=0;j<size1;j++){
//            corner3d.push_back(Point3f(i*25,j*25,0));

//        }
//    }

    for (int i=0;i<size2;i++){
        for(int j=size1-1;j>=0;j--){
            corner3d.push_back(Point3f(i*eachDis,j*eachDis,0));

        }
    }

    solvePnP(corner3d,corners,cm,dc,rvec,tvec,false);

    cv::Mat R;
    cv::Rodrigues(rvec, R);
    //Mat testP = (Mat_<double>(3,1) << p2.x, p2.y, p2.z);

    //cout<<"testP"<<testP<<endl;

//    R=R.t();
//    tvec = -R * tvec; // translation of inverse
      cv::Mat T(4, 4, R.type()); // T is 4x4
    T( cv::Range(0,3), cv::Range(0,3) ) = R * 1; // copies R into T
    T.row(3)=0;
    T( cv::Range(0,3), cv::Range(3,4) )=tvec*1;
    T.at<double>(3,3)=1;
//    double *p = T.ptr<double>(3);
//    p[0] = p[1] = p[2] = 0; p[3] = 1;
    cout<<"R"<<R<<endl;
    cout<<"TV"<<tvec<<endl;
    cout<<"T"<<T<<endl;
//    Mat resu1=R*testP+tvec;
//    cout<<"result1"<<resu1<<endl;

    Mat testP1 = (Mat_<double>(4,1) << 0, 0, 0,1);
    Mat testP2 = (Mat_<double>(4,1) << 0, eachDis, 0,1);
    Mat testP3 = (Mat_<double>(4,1) << eachDis, eachDis,0,1);
    Mat resu1=T*testP1;
    Mat resu2=T*testP2;
    Mat resu3=T*testP3;
    cout<<"result1="<<resu1<<endl;
    cout<<"result2="<<resu2<<endl;
    cout<<"result3="<<resu3<<endl;

    //temp12 vector 1 to 2
    temp12.push_back(resu2.at<double>(0)-resu1.at<double>(0));
    temp12.push_back(resu2.at<double>(1)-resu1.at<double>(1));
    temp12.push_back(resu2.at<double>(2)-resu1.at<double>(2));
    //temp23 vector 2 to 3
    temp23.push_back(resu3.at<double>(0)-resu2.at<double>(0));
    temp23.push_back(resu3.at<double>(1)-resu2.at<double>(1));
    temp23.push_back(resu3.at<double>(2)-resu2.at<double>(2));
    //det find norm vec
    tempN.push_back(temp12[1]*temp23[2]-temp23[1]*temp12[2]);
    tempN.push_back(temp12[2]*temp23[0]-temp23[2]*temp12[0]);
    tempN.push_back(temp12[0]*temp23[1]-temp23[0]*temp12[1]);
    //find Equation plane
    a=tempN[0];
    b=tempN[1];
    c=tempN[2];
    d=tempN[0]*(resu2.at<double>(0))+tempN[1]*(resu2.at<double>(1))+tempN[2]*(resu2.at<double>(2));

    cout<<"a="<<a<<endl;
    cout<<"b="<<b<<endl;
    cout<<"c="<<c<<endl;
    cout<<"d="<<d<<endl;

    Mat temptest;

    Mat ttt1 = (Mat_<double>(3,1) << corners[0].x, corners[0].y,1);
    Mat ttt2 = (Mat_<double>(3,1) << corners[1].x, corners[1].y,1);
    ttt1 = cm.inv()*ttt1;
    ttt2 = cm.inv()*ttt2;
    cout<<corners[0]<<endl;
    cout<<d/(a*ttt1.at<double>(0)+b*ttt1.at<double>(1)+c*ttt1.at<double>(2))*ttt1.at<double>(0)<<endl;
    cout<<d/(a*ttt1.at<double>(0)+b*ttt1.at<double>(1)+c*ttt1.at<double>(2))*ttt1.at<double>(1)<<endl;
    cout<<d/(a*ttt1.at<double>(0)+b*ttt1.at<double>(1)+c*ttt1.at<double>(2))<<endl;
    cout<<corners[1]<<endl;
    cout<<d/(a*ttt2.at<double>(0)+b*ttt2.at<double>(1)+c*ttt2.at<double>(2))*ttt2.at<double>(0)<<endl;
    cout<<d/(a*ttt2.at<double>(0)+b*ttt2.at<double>(1)+c*ttt2.at<double>(2))*ttt2.at<double>(1)<<endl;
    cout<<d/(a*ttt2.at<double>(0)+b*ttt2.at<double>(1)+c*ttt2.at<double>(2))<<endl;



}

void DialogRange::ReadCameraMatrix()
{
    cout << endl << "Reading: " << endl;
    FileStorage fs;
    fs.open("out_camera_data.xml", FileStorage::READ);

    fs["Camera_Matrix"]>>cm;
    fs["distortion_coefficients"] >> dc;
    fs.release();

}

void DialogRange::on_capButton_clicked()
{
    Mat im;
    //vid>>im;
    im=imread("3.jpg");
    Size patternsize(size1,size2); //interior number of corners
    SimpleBlobDetector::Params params;
    params.minArea = 50;
    params.minDistBetweenBlobs = 5;

     Ptr<SimpleBlobDetector> blobDetector = SimpleBlobDetector::create(params);

    Mat gray;
    cv::cvtColor(im,gray, CV_BGR2GRAY);


    //CALIB_CB_FAST_CHECK saves a lot of time on images
    //that do not contain any chessboard corners
    bool patternfound = cv::findCirclesGrid(gray, patternsize, corners,CALIB_CB_SYMMETRIC_GRID + CALIB_CB_CLUSTERING);//, blobDetector);

    cout<<corners<<endl;
//    if(patternfound)
//      cornerSubPix(gray, corners, Size(11, 11), Size(-1, -1),
//        TermCriteria(CV_TERMCRIT_EPS + CV_TERMCRIT_ITER, 30, 0.1));

    drawChessboardCorners(im, patternsize, Mat(corners), patternfound);



    QImage qim=tools::Mat2QImage(im);
    ui->imgLabel->setPixmap(QPixmap::fromImage(qim));


}



