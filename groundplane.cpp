#include "groundplane.h"


GroundPlane::GroundPlane()
{
    readCameraMatrix();

    eachDis=100;//centimetre
    size1=2;//number in 1 line
    size2=3;//number of lines
}

void GroundPlane::readCameraMatrix()
{

    cout << endl << "Reading: " << endl;
    FileStorage fs;
    if (fs.open("out_camera_data.xml", FileStorage::READ)){
        fs["Camera_Matrix"]>>cm;
        fs["distortion_coefficients"] >> dc;
        fs.release();
    }else{
        cout<<"cannot read file camera matrix"<<endl;
    }
}

bool GroundPlane::readCameraMatrix(const string filename)
{

    cout << endl << "Reading: "<<filename << endl;
    FileStorage fs;
    if (fs.open(filename, FileStorage::READ)){
        fs["Camera_Matrix"]>>cm;
        fs["distortion_coefficients"] >> dc;
        fs.release();
        return true;
    }else{
        cout<<"cannot read file camera matrix"<<endl;
        return false;
    }
}


vector<Point2f> GroundPlane::findCircle(Mat im)
{

    Size patternsize(size1,size2); //interior number of corners
    SimpleBlobDetector::Params params;
    params.minArea = 50;
    params.minDistBetweenBlobs = 5;
    Ptr<SimpleBlobDetector> blobDetector = SimpleBlobDetector::create(params);
    Mat gray;
    cv::cvtColor(im,gray, CV_BGR2GRAY);
    bool patternfound = cv::findCirclesGrid(gray, patternsize, corners,CALIB_CB_SYMMETRIC_GRID + CALIB_CB_CLUSTERING);//, blobDetector);
    cout<<corners<<endl;
    drawChessboardCorners(im, patternsize, Mat(corners), patternfound);
    return corners;
}


void GroundPlane::calPlaneEquation()
{
//--prepare data
    //read camera parameter
    readCameraMatrix();
    //prepare object cordinate
    vector<Point3f> corner3d;
    for (int i=0;i<size2;i++){
        for(int j=size1-1;j>=0;j--){
            corner3d.push_back(Point3f(i*eachDis,j*eachDis,0));

        }
    }
    //solve equation to cameracordinate
    solvePnP(corner3d,corners,cm,dc,rvec,tvec,false);
    //prepare equation of T(rvec+tvec)
    cv::Mat R;
    cv::Rodrigues(rvec, R);
    cv::Mat T(4, 4, R.type()); // T is 4x4
    T( cv::Range(0,3), cv::Range(0,3) ) = R * 1; // copies R into T
    T.row(3)=0;
    T( cv::Range(0,3), cv::Range(3,4) )=tvec*1;
    T.at<double>(3,3)=1;
    cout<<"R"<<R<<endl;
    cout<<"TV"<<tvec<<endl;
    cout<<"T"<<T<<endl;

//--prepare to build groundplane
    //prepare 3 point on ground (object point to camera point [calculate by rvec,tvec])
    //3point angle of L
    Mat testP1 = (Mat_<double>(4,1) << 0, 0, 0,1);
    Mat testP2 = (Mat_<double>(4,1) << 0, eachDis, 0,1);
    Mat testP3 = (Mat_<double>(4,1) << eachDis, eachDis,0,1);
    Mat resu1=T*testP1;
    Mat resu2=T*testP2;
    Mat resu3=T*testP3;
    cout<<"result1="<<resu1<<endl;
    cout<<"result2="<<resu2<<endl;
    cout<<"result3="<<resu3<<endl;
//--calculate groundplane
    vector<double> temp12,temp23,tempN;
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
//--result show as ax+by+cz+d=0
    a=tempN[0];
    b=tempN[1];
    c=tempN[2];
    d=tempN[0]*(resu2.at<double>(0))+tempN[1]*(resu2.at<double>(1))+tempN[2]*(resu2.at<double>(2));

    cout<<"a="<<a<<endl;
    cout<<"b="<<b<<endl;
    cout<<"c="<<c<<endl;
    cout<<"d="<<d<<endl;

}

Point3d GroundPlane::imgCord2CameraCord(Point2d imgCord)
{
    Mat ttt1 = (Mat_<double>(3,1) << imgCord.x, imgCord.y,1);
    ttt1=cm.inv()*ttt1;
    return Point3d(ttt1.at<double>(0),ttt1.at<double>(1),ttt1.at<double>(2));
}

Point3d GroundPlane::getPointAtGround(Point2d imgCord)
{
    Mat ttt1 = (Mat_<double>(3,1) << imgCord.x, imgCord.y,1);
    ttt1=cm.inv()*ttt1;
    double x=d/(a*ttt1.at<double>(0)+b*ttt1.at<double>(1)+c*ttt1.at<double>(2))*ttt1.at<double>(0);
    double y=d/(a*ttt1.at<double>(0)+b*ttt1.at<double>(1)+c*ttt1.at<double>(2))*ttt1.at<double>(1);
    double z=d/(a*ttt1.at<double>(0)+b*ttt1.at<double>(1)+c*ttt1.at<double>(2));
    return Point3d(x,y,z);
}

double GroundPlane::calDis3D(Point3d a,Point3d b)
{

    return norm(a-b);

}

bool GroundPlane::saveGroundParam()
{
    FileStorage fs("ground.xml", FileStorage::WRITE );
    fs<<"a"<<a;
    fs<<"b"<<b;
    fs<<"c"<<c;
    fs<<"d"<<d;
    fs.release();
}

bool GroundPlane::readGroundParam()
{
    FileStorage fs;
    if (fs.open("ground.xml", FileStorage::READ)){
        fs["a"]>>a;
        fs["b"]>>b;
        fs["c"]>>c;
        fs["d"]>>d;
        cout<<a<<endl;
        cout<<b<<endl;
        cout<<c<<endl;
        cout<<d<<endl;
        fs.release();
    }else{
        cout<<"cannot read ground plane"<<endl;
    }
}


