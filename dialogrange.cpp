#include "dialogrange.h"
#include "ui_dialogrange.h"

DialogRange::DialogRange(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogRange)
{
    ui->setupUi(this);
    ui->imgLabel->setDialog(this);

}

DialogRange::~DialogRange()
{
    delete ui;
}

void DialogRange::setImageMat(cv::Mat im,VideoCapture vid,Ptr<GroundPlane> gp,bool *p){
    this->vid=vid;
    this->p=p;
    QImage qim=tools::Mat2QImage(im);
    int imh=qim.height();
    int imw=qim.width();
    ui->imgLabel->setFixedHeight(imh);
    ui->imgLabel->setFixedWidth(imw);
    this->gp=gp;
    ui->imgLabel->setPixmap(QPixmap::fromImage(qim));

}





void DialogRange::on_cdButton_clicked()
{
    //gp->calPlaneEquation();
    //gp->saveGroundParam();
    gp->readGroundParam();
    cout<<gp->getPointAtGround(Point2d(285,327))<<endl;
    cout<<gp->getPointAtGround(Point2d(285,294))<<endl;


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
    gp->findCircle(im);
    QImage qim=tools::Mat2QImage(im);
    ui->imgLabel->setPixmap(QPixmap::fromImage(qim));


}

//set status Label
void DialogRange::setStatusLb(const string x,bool success)
{
    ui->statusLb->setStyleSheet(success?"QLabel {color : Green; }":"QLabel {color : Red; }");
    ui->statusLb->setText(QString::fromStdString("Status :"+x));
}

//Calibrate Camera
void DialogRange::on_ccBut_clicked()
{
    ccc.start();
}

//Load Camera Parameter
void DialogRange::on_lcpBut_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"), "C:\\Users\\HHM-MSI\\Videos\\TC3+FW", tr("xml Files (*.xml)"));
    std::string fileNameStr = fileName.toUtf8().constData();//cast to string
    gp->readCameraMatrix(fileNameStr);
    setStatusLb(gp->readCameraMatrix(fileNameStr)?"Load C Done":"Load C Fail",1);
}

//Save Camera Parameter
void DialogRange::on_scpBut_clicked()
{
    {//read
        FileStorage fs;
        if (fs.open("out_camera_data.xml", FileStorage::READ)){
            fs["Camera_Matrix"]>>cm;
            fs["Distortion_Coefficients"] >> dc;
            fs.release();
        }else{
            cout<<"cannot read file camera matrix"<<endl;
        }
    }
    //write
    QString fileName = QFileDialog::getSaveFileName(this,tr("Open Image"), "C:\\Users\\HHM-MSI\\Videos\\TC3+FW", tr("xml Files (*.xml)"));
    std::string fileNameStr = fileName.toUtf8().constData();//cast to string
    FileStorage fs(fileNameStr, FileStorage::WRITE );
    fs<<"Camera_Matrix"<<cm;
    fs<<"distortion_coefficients"<<dc;

    fs.release();
}

//From Image (GroundPlane)
void DialogRange::on_fmBut_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"), "C:\\Users\\HHM-MSI\\Videos\\TC3+FW", tr("xml Files (*.xml)"));
    std::string fileNameStr = fileName.toUtf8().constData();//cast to string

    Mat im;
    im=imread(fileNameStr);
    gp->findCircle(im);
    if (!gp->corners.empty())
        gp->calPlaneEquation();
    QImage qim=tools::Mat2QImage(im);
    ui->imgLabel->setPixmap(QPixmap::fromImage(qim));
}

//From Camera (GroundPlane)
void DialogRange::on_fcBut_clicked()
{
    Mat im;
    vid>>im;
    gp->findCircle(im);
    if (!gp->corners.empty())
        gp->calPlaneEquation();
    QImage qim=tools::Mat2QImage(im);
    ui->imgLabel->setPixmap(QPixmap::fromImage(qim));
}

//Load Ground Parameter
void DialogRange::on_lgpBut_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"), "C:\\Users\\HHM-MSI\\Videos\\TC3+FW", tr("xml Files (*.xml)"));
    std::string fileNameStr = fileName.toUtf8().constData();//cast to string
    gp->readGroundParam(fileNameStr);
}

//Save Ground Parameter
void DialogRange::on_sgpBut_clicked()
{
    gp->readGroundParam();
    QString fileName = QFileDialog::getSaveFileName(this,tr("Open Image"), "C:\\Users\\HHM-MSI\\Videos\\TC3+FW", tr("xml Files (*.xml)"));
    std::string fileNameStr = fileName.toUtf8().constData();//cast to string
    gp->saveGroundParam(fileNameStr);
}

//Load Setting Parameter
void DialogRange::on_lspBut_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this,tr("Open Image"), "C:\\Users\\HHM-MSI\\Videos\\TC3+FW", tr("xml Files (*.xml)"));
    std::string fileNameStr = fileName.toUtf8().constData();//cast to string
    gp->readCameraMatrix(fileNameStr);
    gp->readGroundParam(fileNameStr);

}

//Save Setting Parameter
void DialogRange::on_sspBut_clicked()
{
    QString fileName = QFileDialog::getSaveFileName(this,tr("Open Image"), "C:\\Users\\HHM-MSI\\Videos\\TC3+FW", tr("xml Files (*.xml)"));
    std::string fileNameStr = fileName.toUtf8().constData();//cast to string
    gp->writeSetting(fileNameStr);
}


//accept click(ok)
void DialogRange::on_buttonBox_accepted()
{
    *p=!(*p);
}
