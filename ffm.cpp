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
    cv::findNonZero(ff,nz);
    this->frame=frame;
    if (curr.size()==0 && !nz.empty()){
        addNewSFF(nz);
    }else {
//        nzDes=extractMatKeyPoint(nz);
//        for (int i=0;i<curr.size();i++){
//            //int nea=findNearest(curr.at(i).pt,nz);
//            int nea=findNearestMatch(i,nz);
//            updateForeground(i,nea);
//        }
        dynamicMatch();

        addNewSFF(nz);
        deleteFF();


    }


}

bool FFM::updateForeground(int i,int nea){
    if (nea!=-1){
        curr.at(i).pt=nz.at<cv::Point>(nea);
        curr.at(i).size=cResist;
        ffUsed.push_back(nea);
        if (calDis(curr.at(i).pt,start.at(i).pt)>D || start.at(i).size>0){
            start.at(i).size=mResist;
            mf.at(i)=1;

        }
    }else{
        curr.at(i).size-=1;
        start.at(i).size-=1;
        if (curr.at(i).size<0 && start.at(i).size<0){
            del.push_back(i);
        }
//                if (start.at(i).size>=0){
//                    mf.at(i)=1;
//                }

    }
}

int FFM::getMFCount(){
    return sum(mf)[0];
}

int FFM::getSFCount(){
    return curr.size()-sum(mf)[0];
}

void FFM::addNewSFF(cv::Mat nz){
    for (int ffi = 0; ffi < nz.total(); ffi++ ) {
        if (std::find(ffUsed.begin(),ffUsed.end(),ffi)==ffUsed.end()){
            cv::Point ffPos=nz.at<cv::Point>(ffi);
            start.push_back(cv::KeyPoint(ffPos,0));
            curr.push_back(cv::KeyPoint(ffPos,cResist));
            currDes.push_back(Mat::zeros(1,64,currDes.type()));
            mf.push_back(0);
        }
    }

    if (matchType){

          currDes=extractCurrentKeyPointDynamic(curr);

    }
}

void FFM::deleteFF(){
    std::vector<cv::KeyPoint> start2;
    std::vector<cv::KeyPoint> curr2;
    std::vector<int> mf2;
    Mat currDesTemp;
    for (int i=0;i<start.size();i++){
        if (std::find(del.begin(),del.end(),i)==del.end()){
            start2.push_back(start.at(i));
            curr2.push_back(curr.at(i));
            mf2.push_back(mf.at(i));
            currDesTemp.push_back(currDes.row(i));
        }
    }
    cout<<start.size()<<","<<currDes.rows<<endl;
    currDes=currDesTemp;
    start=start2;
    curr=curr2;
    mf=mf2;

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
        return findNearest(curr.at(rowNum).pt,nz);
    }

    index=nzTemp.at(matches[0].trainIdx);

    if (matches[0].distance>=1){

        return findNearest(curr.at(rowNum).pt,nz);

    }

    nzUsed.push_back(index);
    cout<<"in"<<matches[0].trainIdx<<endl;
    return index;

}

vector<int> FFM::dynamicMatch(){
    vector<int> result;
    vector<vector<int> > indexs;
    vector<vector<double> > values;
    for (int i=0;i<curr.size();i++){
        vector<int> tempIndex;
        vector<double> tempValue;
        for (int j=0;j<nz.total();j++){
            Point p1=curr.at(i).pt;
            Point p2=nz.at<cv::Point>(j);
            int distance=calDis(p1,p2);
            tempValue.push_back(distance);
            tempIndex.push_back(j);
            for (int k=0;k<tempValue.size();k++){
                if (tempValue.at(k)<=distance){
                    std::swap(tempValue.at(k),tempValue.at(j));
                    std::swap(tempIndex.at(k),tempIndex.at(j));
                }
            }

        }
        indexs.push_back(tempIndex);
        values.push_back(tempValue);
    }

    vector<int> alreadyUse;
    for (int i=0;i<indexs.size();i++){
        double min=10000;
        int tempRealIndex=-1;
        for (int j=0;j<indexs.at(i).size();j++){
            double val=values.at(i).at(j);
            if (val<=min && val<R && std::find(alreadyUse.begin(),alreadyUse.end(),indexs.at(i).at(j))==alreadyUse.end()){
                min=val;
                tempRealIndex=indexs.at(i).at(j);
            }
        }
        updateForeground(i,tempRealIndex);
        alreadyUse.push_back(tempRealIndex);
        result.push_back(tempRealIndex);
    }

    return result;

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

Mat FFM::extractCurrentKeyPointDynamic(std::vector<cv::KeyPoint> kp){
    Mat result;

    std::vector<KeyPoint> keypoints_object;
    std::vector<Point2f> tttemp;

    KeyPoint::convert(kp,tttemp);
    KeyPoint::convert(tttemp,keypoints_object);
    //Mat descriptors_object, descriptors_scene;

    surf->compute( frame, keypoints_object, result );

    for(int i=0;i<curr.size();i++){
        if (curr.at(i).size<cResist){
            result.row(i)=currDes.row(i);
        }
    }
    return result;

}

void FFM::calculateMatch(Mat nz){
    FlannBasedMatcher matcher;

    if (!nz.empty()){
        matcher.match(currDes,extractMatKeyPoint(nz),matches);

    }
}


void FFM::clearNSort(){
    int si=0;
    for (int i=0;i<mf.size();i++){
        if (mf.at(i)==1){
            std::swap(curr.at(si),curr.at(i));
            std::swap(start.at(si),start.at(i));
            if (matchType){
                Mat temp=currDes.row(si);
                currDes.row(si)=currDes.row(i);
                currDes.row(i)=temp;
            }

            si++;
        }
    }

    std::fill(mf.begin(),mf.end(),0);
    ffUsed.clear();
    nzUsed.clear();
    del.clear();
}

bool response_comparator(const cv::KeyPoint& p1, const cv::KeyPoint& p2) {
    return p1.size > p2.size;
}
