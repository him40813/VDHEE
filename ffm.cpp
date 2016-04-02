#include "ffm.h"

FFM::FFM(cv::Ptr<display> d,int r,int dis,int coutz,cv::Ptr<GroundPlane> gp)
{
    this->gp=gp;
    this->d=d;
    mResist=coutz;
    cResist=2;
    R=r;
    D=dis;
    matchType=1;
    surf = xfeatures2d::SURF::create(500); // note extra namespace


}

void FFM::process(cv::Mat ff,cv::Mat frame){
    //check if no any current position
    cv::Mat nz;
    cv::findNonZero(ff,nz);
    this->frame=frame;
    if (curr.size()==0 && !nz.empty()){
        addNewSFF(nz);
    }else {
        nzDes=extractMatKeyPoint(nz);

        for (int i=0;i<curr.size();i++){
            //int nea=findNearest(curr.at(i).pt,nz);
            int nea=findNearestMatch(i,nz);

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

    if (matchType){

          currDes=extractKeyPoint(curr);

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

        }
    }
    nzUsed.push_back(index);
    return index;
}

int FFM::findNearestMatch(int rowNum,const Mat nz){
    int index=-1;

    Mat cdkp=currDes.row(rowNum);
    Mat nzdkp;
    std::vector<int> nzTemp;
    for (int i=0;i<nz.total();i++){
        double dis=calDis(nz.at<cv::Point>(i),curr.at(rowNum).pt);

        if (dis<R && std::find(nzUsed.begin(),nzUsed.end(),i)==nzUsed.end()){
            nzdkp.push_back(nzDes.row(i));
            nzTemp.push_back(i);
        }
        else{
            //nzdkp.row(i)=Mat::zeros(1,64,nzDes.type());
        }
    }

    FlannBasedMatcher matcher;

    if (!nzdkp.empty() && cv::sum(cdkp)[0]>0.0){
        matcher.match(cdkp,nzdkp,matches);

    }else{
        return -1;
    }

    index=nzTemp.at(matches[0].trainIdx);

//    if (matches[0].distance>=1){

//        return -1;

//    }

    nzUsed.push_back(matches[0].trainIdx);
    cout<<"in"<<matches[0].trainIdx<<endl;
    return index;

}

int FFM::findNearestMatch2(int index,Mat nz){
    if (index<curr.size() && matches[index].distance>0.02){
        return -1;
    }
    int result=matches[index].trainIdx;

    vector<Point2f> ttt;
    KeyPoint::convert(curr,ttt);
    double dis=calDis(nz.at<cv::Point>(result),ttt.at(index));
    if ( dis<R && std::find(nzUsed.begin(),nzUsed.end(),index)==nzUsed.end()){
        cout<<index<<"matches: "<<matches[index].queryIdx<<","<<matches[index].trainIdx<<endl;
        nzUsed.push_back(index);

        return matches[index].trainIdx;
    }
    return -1;
}


double FFM::calDis(cv::Point xx1,cv::Point xx2)
{

    return gp->calDis3DFrom2D(xx1,xx2);
//    double x = xx1.x - xx2.x;
//    double y = xx1.y - xx2.y;
//    double dist;

//    dist = pow(x,2)+pow(y,2);           //calculating distance by euclidean formula
//    dist = sqrt(dist);                  //sqrt is function in math.h

//    return dist;
}

Mat FFM::extractKeyPoint(std::vector<cv::KeyPoint> kp){
    Mat result;
    std::vector<KeyPoint> keypoints_object;
    std::vector<Point2f> tttemp;

    KeyPoint::convert(kp,tttemp);
    KeyPoint::convert(tttemp,keypoints_object);
    //Mat descriptors_object, descriptors_scene;

    surf->compute( frame, keypoints_object, result );
    return result;

    //extractor.compute( frame, keypoints_scene, descriptors_scene );
}

Mat FFM::extractMatKeyPoint(Mat kp){
    Mat result;
    std::vector<KeyPoint> keypoints_object;
    std::vector<Point2f> tttemp;
    for (int i=0;i<kp.rows;i++){
        cv::Point2f ffPos=kp.at<cv::Point2f>(i);
        tttemp.push_back(ffPos);
    }
    KeyPoint::convert(tttemp,keypoints_object);
    surf->compute( frame, keypoints_object, result );
    return result;
}

void FFM::calculateMatch(Mat nz){
    FlannBasedMatcher matcher;

    if (!nz.empty()){
        matcher.match(currDes,extractMatKeyPoint(nz),matches);

    }
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
