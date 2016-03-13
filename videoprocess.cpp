#include "videoProcess.h"

//init
videoProcess::videoProcess(Ui::MainWindow *m,cv::VideoCapture v)
{
    setVid(v);
    this->m=m;
    cin=m->cin->text().toInt();
    cout=m->cout->text().toInt();
}

void videoProcess::setVid(cv::VideoCapture v)
{
    this->vid=v;

}

//Main Function Each Lopp--------------------------------------------------------------------------------------------------------------
void videoProcess::base(){



//Init------------------------------------------------------------------------------------
    //readframe
    vid.read(realFrame);
    //rgb2gray
    cv::cvtColor(realFrame,frame, CV_BGR2GRAY);
    d->setImage(realFrame);//set Original  Image
    //detect FAST
    cv::FAST(frame,curr,51);

    if (n==0 and setupRange==false){
    //http://stackoverflow.com/questions/11543298/qt-opencv-displaying-images-on-qlabel

        dr.show();
        dr.setImageMat(realFrame,vid);
        p=true;
    }

//core System-----------------------------------------------------------------------------
    b->add(curr,n,human);
    FF=tools::key2Mat(curr,h,w);
    FF=b->getForeground(FF);
    ffm->clearNSort();
    ffm->process(FF);
    int test=ffm->getMFCount();
    if (test>0){
        cIn++;
    }
    else{
        cIn=0;
    }
    if (cIn==cin ){
        human=true;
        countIn++;
        m->StatLabel->setText(QString::fromStdString("Entry: "+tools::int2str(countIn)));
    }else if(cIn>cin){
         human=true;
    }else{
        human=false;
    }
    d->setMfm(ffm->getSFCount());
    d->setMfm(ffm->getMFCount());




//footer----------------------------------------------------------------------------------
    d->drawTrueMask(b->result,cv::Scalar(1,255,1),m->chkBGM->isChecked());
    d->drawMask(b->Foreground,cv::Scalar(1,1,255),m->chkFF->isChecked());
    d->drawLine(ffm->start,ffm->curr,m->chkLine->isChecked());
    d->setBg(b->BG.size());
    d->setBf(b->BF.size());
    d->setCff(b->cff);
    d->setFf(b->fff);
    d->setMfm(ffm->getSFCount());
    d->setMfm(ffm->getMFCount());
    d->setFrameN(n);
    d->showIm();
    n++;
    //release mat
    frame.release();
    realFrame.release();
    FF.release();
}

//Main Loop Function-------------------------------------------------------------------------------------------------------------------
void videoProcess::process()
{
    //Time and Frame#
    double start, end, timeu=0;
    n=0;

    //init parameter
    w=vid.get(CV_CAP_PROP_FRAME_WIDTH);
    h=vid.get(CV_CAP_PROP_FRAME_HEIGHT);
    human=false;
    FF=cv::Mat(h,w,CV_8UC1,cv::Scalar(0));

    //Play Pause Stop boolean
    stop=false;
    s=false;
    p=false;
    setupRange=false;
    cIn=0;
    countIn=0;
    //Create Object
    d=new display(m,"main");
    b=new BFM(h,w,m->bgf->text().toInt(),m->bff->text().toInt(),d);
    ffm=new FFM(d,m->r->text().toInt(),m->d->text().toInt(),cout);
    //loop
    for(;;)
    {


//        start=cv::getTickCount();
        if (stop){//check if press stop
            break;
        }else if (s){//check if press step

            base();

            p=true;
            s=false;
            cv::waitKey(1);
            continue;
        }else if (p){//check if press pause
            cv::waitKey(1);
            continue;
        }else{//Normal loop
            start=cv::getTickCount();
            base();

            end=cv::getTickCount();
            timeu+=(double)(end-start)/cv::getTickFrequency();
            d->setFps(std::ceil(n/timeu));
        }

        if (cv::waitKey(1)>=27){//check if esc + delay for 30milisec
            break;
        }

        if (d->im.empty())
        {
            break;
        }

    }
}

void videoProcess::stopProcess(){
    stop=true;
}

void videoProcess::pause(){
    p=!p;
}

void videoProcess::step(){
    s=true;
}


