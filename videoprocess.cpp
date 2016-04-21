#include "videoProcess.h"

//init
videoProcess::videoProcess(Ui::MainWindow *m,cv::VideoCapture v)
{
    setVid(v);
    this->m=m;
    //cin=m->cin->text().toInt();
    cin=24;
    cout=m->cout->text().toInt();
    testType=0;
    setupRange=false;
    endOfTest=false;
}

void videoProcess::setVid(cv::VideoCapture v)
{
    this->vid=v;

}

//Main Function Each Lopp--------------------------------------------------------------------------------------------------------------
int videoProcess::base(){



//Init------------------------------------------------------------------------------------
    //readframe
    if (!vid.read(realFrame)){
        endOfVid=true;
        return 0;
    }
    //rgb2gray
    cv::cvtColor(realFrame,frame, CV_BGR2GRAY);
    d->setImage(realFrame);//set Original  Image
    //detect FAST
    cv::FAST(frame,curr,51);

    if (n==0 && setupRange==false){
    //http://stackoverflow.com/questions/11543298/qt-opencv-displaying-images-on-qlabel

        dr.show();
        dr.setImageMat(realFrame,vid,gp,&p);

        p=true;
    }

//core System-----------------------------------------------------------------------------
    b->add(curr,n,human);
    FF=tools::key2Mat(curr,h,w);
    FF=b->getForeground(FF);
    ffm->clearNSort();
    ffm->process(FF,frame);
    int test=ffm->getMFCount();
    if (test>0){
        cIn++;
    }
    else if (human){
        cOut++;
    }
    if (cIn==cin && !human){
        human=true;
        countIn++;
        m->StatLabel->setText(QString::fromStdString("Entry: "+tools::int2str(countIn)));
        exel1.push_back(n);
        exel2.push_back(test);
    }else if(cIn>cin){
         human=true;
         //pos in
         exel1.push_back(n);
         //average mf
         exel2.push_back(test);

    }
    //pos out
    if (!human)
    {
        exel4.push_back(n);
    }
    if (cOut==3 && human){
        human=false;
        cIn=0;
        cOut=0;
        //average max moving feature
        exel3.push_back(ffm->maxMovingDistance);
        ffm->maxMovingDistance=-1;
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
    d->setFf(test);
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

    endOfVid=false;
    cIn=0;
    countIn=0;
    cOut=0;
    //Create Object
    d=new display(m,"main");

    if (!testType)
        gp=new GroundPlane();

    b=new BFM(h,w,m->bgf->text().toInt(),m->bff->text().toInt(),d);
    //ffm=new FFM(d,m->r->text().toInt(),m->d->text().toInt(),cout,gp);


//    testType=7;
    //define test value
    if (testType==0){
        ffm=new FFM(d,8,16,cout,gp);
        ffm->calDisType=0;
        ffm->matchType=0;
        endOfTest=true;
    }else if (testType==1){
        ffm=new FFM(d,18,36,cout,gp);
        ffm->calDisType=1;
        ffm->matchType=0;
    }else if (testType==2){
        ffm=new FFM(d,8,16,cout,gp);
        ffm->calDisType=0;
        ffm->matchType=1;
    }else if (testType==3){
        ffm=new FFM(d,18,36,cout,gp);
        ffm->calDisType=1;
        ffm->matchType=1;
    }else if (testType==4){
        ffm=new FFM(d,16,16,cout,gp);
        ffm->calDisType=0;
        ffm->matchType=0;
    }else if (testType==5){
        ffm=new FFM(d,36,36,cout,gp);
        ffm->calDisType=1;
        ffm->matchType=0;
    }else if (testType==6){
        ffm=new FFM(d,16,16,cout,gp);
        ffm->calDisType=0;
        ffm->matchType=1;
    }else if (testType==7){
        ffm=new FFM(d,36,37,cout,gp);
        ffm->calDisType=1;
        ffm->matchType=1;
        endOfTest=true;
    }
    //old RD
    //ffm=new FFM(d,36,36,cout,gp);
    //new RD
//    ffm=new FFM(d,36,72,cout,gp);



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
            //readCheckingFile("testCheckFile.xml");
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

        if (endOfVid)
        {
            readCheckingFile("41.xml");
            saveMatToCsvDouble(exelStat,"exelStat-case"+tools::int2str(testType)+"-"+tools::int2str(ffm->calDisType)+tools::int2str(ffm->matchType)+".csv");
            saveMatToCsv(exel1,"exel1-case"+tools::int2str(testType)+"-"+tools::int2str(ffm->calDisType)+tools::int2str(ffm->matchType)+".csv");
            saveMatToCsv(exel2,"exel2-case"+tools::int2str(testType)+"-"+tools::int2str(ffm->calDisType)+tools::int2str(ffm->matchType)+".csv");
            saveMatToCsv(exel3,"exel3-case"+tools::int2str(testType)+"-"+tools::int2str(ffm->calDisType)+tools::int2str(ffm->matchType)+".csv");
            saveMatToCsv(exel4,"exel4-case"+tools::int2str(testType)+"-"+tools::int2str(ffm->calDisType)+tools::int2str(ffm->matchType)+".csv");
            exel1.release();
            exel2.release();
            exel3.release();
            exel4.release();
            exelStat.release();
            break;
        }


    }

    if (!endOfTest){
        ++testType;
        vid.set(CV_CAP_PROP_POS_MSEC,0);
        setupRange=true;
        n=0;
        process();

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

void videoProcess::saveMatToCsv(const Mat &matrix, const string filename){
     ofstream myfile;
     myfile.open(filename.c_str());

     for (int i=0;i<matrix.rows;i++){
         for (int j=0;j<matrix.cols;j++){
             myfile <<matrix.row(i).at<int>(j);
             if (j!=matrix.cols-1)
                myfile<<",";
         }
         myfile<<endl;
     }
    std::cout<<filename<<":writeFile Complete"<<endl;
     myfile.close();

}

void videoProcess::saveMatToCsvDouble(const Mat &matrix, const string filename){
     ofstream myfile;
     myfile.open(filename.c_str());

     for (int i=0;i<matrix.rows;i++){
         for (int j=0;j<matrix.cols;j++){
             myfile <<matrix.row(i).at<double>(j);
             if (j!=matrix.cols-1)
                myfile<<",";
         }
         myfile<<endl;
     }
    std::cout<<filename<<":writeFile Complete"<<endl;
     myfile.close();

}

bool videoProcess::readCheckingFile(const string x){
    //0.966416
    //0.972932
    //0.974185
    FileStorage fs;
    if (fs.open(x, FileStorage::READ)){
        checkList.clear();
        fs["checkFile"]>>checkFile;
        std::cout<<checkFile<<endl;

        if (!checkFile.empty()){
            for (int i=0;i<checkFile.rows;++i){
                cv::Point p=checkFile.at<cv::Point>(i);

                for (int nn=p.x;nn<=p.y;++nn){
                    for (int nnn=0;nnn<30;++nnn){
                        checkList.push_back(nn*30+nnn);

                    }

                }
            }
            double count=0;
            double count2=0;
            for (int ii=0;ii<exel1.rows;++ii){
                unsigned int pos = find(checkList.begin(), checkList.end(), exel1.at<int>(ii)) - checkList.begin();
                if (pos<checkList.size()){
                    checkList.at(pos)=-1;
                    ++count;
                }else{
                    ++count2;
                }

            }
            exelStat=cv::Mat( 0, 5, CV_64F);
            std::cout<<count<<","<<checkList.size()<<",avg: "<<count/checkList.size()<<endl;
            Mat stat1=(Mat_<double>(1,3) << count,checkList.size(),count/checkList.size());
            exelStat.push_back(stat1);

            double mfSum=cv::sum(exel2)[0];
            std::cout<<mfSum<<","<<exel2.rows<<",avg: "<<mfSum/exel2.rows<<endl;
            Mat stat2= (Mat_<double>(1,3)<<mfSum,exel2.rows, mfSum/exel2.rows);
            exelStat.push_back(stat2);

            double mfSum3=cv::sum(exel3)[0];
            std::cout<<mfSum3<<","<<exel3.rows<<",avg: "<<mfSum3/exel3.rows<<endl;
            Mat stat3=(Mat_<double>(1,3)<<mfSum3,exel3.rows, mfSum3/exel3.rows);
            exelStat.push_back(stat3);

            std::cout<<count2<<","<<(n-checkList.size())+1<<",avg: "<<1-count2/(n-checkList.size())+1<<endl;
            Mat stat4=(Mat_<double>(1,3) << count2,(n-checkList.size())+1,1-count2/(n-checkList.size())+1);
            exelStat.push_back(stat4);

            std::cout<<count2<<","<<(n-checkList.size())<<",avg: "<<count2/(n-checkList.size())<<endl;
            Mat stat5=(Mat_<double>(1,3) << (double)countIn,0,0);
            exelStat.push_back(stat5);

        }
        fs.release();

        return true;
    }else{
        std::cout<<"cannot read CheckFile:"<<x<<endl;
        return false;
    }
}
