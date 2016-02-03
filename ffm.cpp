#include "ffm.h"

FFM::FFM(cv::Ptr<display> d,int r,int dis,int cout)
{
    this->d=d;
    mResist=cout;
    cResist=2;
    R=r;
    D=dis;
}

void FFM::process(cv::Mat ff){
    //check if no any current position
    cv::Mat nz;
    cv::findNonZero(ff,nz);

    if (curr.size()==0){
        addNewSFF(nz);
    }else {
        for (int i=0;i<curr.size();i++){
            int nea=findNearest(curr.at(i).pt,nz);
            if (nea!=-1){
                curr.at(i).pt=nz.at<cv::Point>(nea);
                curr.at(i).size=cResist;
                ffUsed.push_back(nea);
                if (calDis(curr.at(i).pt,start.at(i).pt)>D || start.at(i).size>0){
                    start.at(i).size=mResist;
                    mf.push_back(i);
                }
            }else{
                curr.at(i).size-=1;
                start.at(i).size-=1;
                if (curr.at(i).size<0 && start.at(i).size<0){
                    del.push_back(i);
                }

            }


        }
        addNewSFF(nz);
        deleteFF();


    }


}

int FFM::getMFCount(){
    return mf.size();
}

int FFM::getSFCount(){
    return curr.size()-mf.size();
}

void FFM::addNewSFF(cv::Mat nz){
    for (int ffi = 0; ffi < nz.total(); ffi++ ) {
        if (std::find(ffUsed.begin(),ffUsed.end(),ffi)==ffUsed.end()){
            cv::Point ffPos=nz.at<cv::Point>(ffi);
            start.push_back(cv::KeyPoint(ffPos,0));
            curr.push_back(cv::KeyPoint(ffPos,cResist));
        }
    }

}

void FFM::deleteFF(){
    for (int i=0;i<del.size();i++){
        curr.erase(curr.begin()+del.at(i));
        start.erase(start.begin()+del.at(i));
    }

}

int FFM::findNearest(cv::Point x,cv::Mat nz){
    int index=-1;
    double min=10000;
    for (int i=0;i<nz.total();i++){
        double dis=calDis(nz.at<cv::Point>(i),x);
        if (dis<min && dis<R && std::find(nzUsed.begin(),nzUsed.end(),i)==nzUsed.end()){
            index=i;
            min=dis;
            nzUsed.push_back(i);
        }
    }
    return index;
}

double FFM::calDis(cv::Point xx1,cv::Point xx2)
{
    double x = xx1.x - xx2.x;
    double y = xx1.y - xx2.y;
    double dist;

    dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
    dist = sqrt(dist);                  //sqrt is function in math.h

    return dist;
}

void FFM::clearNSort(){
    for (int i=0;i<mf.size();i++){
        std::swap(curr.at(i),curr.at(mf.at(i)));
        std::swap(start.at(i),start.at(mf.at(i)));
    }

    mf.clear();
    ffUsed.clear();
    nzUsed.clear();
    del.clear();
}

bool response_comparator(const cv::KeyPoint& p1, const cv::KeyPoint& p2) {
    return p1.size > p2.size;
}
