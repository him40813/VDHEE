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
}

DialogRange::~DialogRange()
{
    delete ui;
}

void DialogRange::setImageMat(cv::Mat im){
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

        highPoint=cv::Point3i(a,b,c);
        ui->posLabelH->setText("("+tools::num2str(a)+","+tools::num2str(b)+","+ui->textH->text()+")");

    }else if (ui->radioM->isChecked()){
        l=pp.x();
        m=pp.y();
        n=ui->textM->text().toInt();

        MediumPoint=cv::Point3i(l,m,n);
        ui->posLabelM->setText("("+tools::num2str(l)+","+tools::num2str(m)+","+ui->textM->text()+")");

    }else if (ui->radioL->isChecked()) {
        p=pp.x();
        q=pp.y();
        r=ui->textL->text().toInt();
        LowPoint=cv::Point3i(p,q,r);
        ui->posLabelL->setText("("+tools::num2str(p)+","+tools::num2str(q)+","+ui->textL->text()+")");
        calculateXYZ();
    }else{

        ui->posLabelTest->setText("("+tools::num2str(pp.x())+","+tools::num2str(pp.y())+","+tools::num2str((pp.x()*x)+(pp.y()*y)+z)+")");
    }
}

void DialogRange::calculateXYZ(){
//    ax+by+cz+d=0
//    lx+my+nz+k=0
//    px+qy+rz+s=0



    double det=tools::Determinant3D(a,b,1,l,m,1,p,q,1);

    double det1=tools::Determinant3D(c,b,1,n,m,1,r,q,1);
    double det2=tools::Determinant3D(a,c,1,l,n,1,p,r,1);
    double det3=tools::Determinant3D(a,b,c,l,m,n,p,q,r);

    x=det1/det;
    y=det2/det;
    z=det3/det;

    ui->textH->setText(tools::num2str(x));
    ui->textM->setText(tools::num2str(y));
    ui->textL->setText(tools::num2str(z));
}

void DialogRange::on_cdButton_clicked()
{
    ReadCameraMatrix();
    //ccc.start();
    cout<<cm<<endl;
}

void DialogRange::ReadCameraMatrix()
{
    cout << endl << "Reading: " << endl;
    FileStorage fs;
    fs.open("out_camera_data.xml", FileStorage::READ);

    fs["Camera_Matrix"]>>cm;
    fs.release();

}
