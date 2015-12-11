#include "HOG.h"
#include "opencv2/opencv.hpp"
#include "ROI.h"
#include <opencv2/highgui/highgui_c.h>
#include "CTracker.h"
#include <iostream>
#include <vector>
#include <iostream>
using namespace cv;
using namespace std;

int main(){
    cordinate_vector.clear();
    Scalar Colors[]={Scalar(255,0,0),Scalar(0,255,0),Scalar(0,0,255),Scalar(255,255,0),Scalar(0,255,255),Scalar(255,0,255),Scalar(255,127,255),Scalar(127,0,255),Scalar(127,0,127)};
    VideoCapture capture("atrium.avi");
    if(!capture.isOpened())
    {
    return 0;
    }
    CTracker tracker(0.2,0.5,60.0,15,10);
    namedWindow("Video");
    capture >> img1;
    Size size(1080,720);
    resize(img1,img1,size);
    Mat roi;
    roi=ROI();
    imshow("ROI",roi);
    while(char(waitKey(1)!='q'));
    //destroyAllWindows(); 
    //Mat roi1;
    //roi1=ROI();
    //imshow("ROI",roi1);
    //while(char(waitKey(1)!='q'));

    Mat img;
    capture >> img;
    HOGDetector* detector = new HOGDetector(img);
    int k;
    int roi_sig;
    //int roi_sig1;
    while(k!=27){
        capture >> img;
        roi_sig =0;
        //roi_sig1 =0;
        //Mat img = imread("test.jpg");
        Size size(1080,720);
        resize(img,img,size);
        line(img, cordinate_vector[0], cordinate_vector[1], Scalar(255,0,0) );
        line(img, cordinate_vector[1], cordinate_vector[2], Scalar(255,0,0) );
        line(img, cordinate_vector[2], cordinate_vector[3], Scalar(255,0,0) );
        line(img, cordinate_vector[3], cordinate_vector[0], Scalar(255,0,0) );
        namedWindow("people detector",1);
        //double t = (double)getTickCount();
        //HOGDetector* detector = new HOGDetector(img);
        vector<Rect> found = detector->Detect(img);
        vector<Point2d> centers = detector->Position();
        size_t i;
        for( i=0; i<centers.size(); i++)
        {
        //circle(img,centers[i],3,Scalar(0,255,0),1,CV_AA);
        //cout << roi.at<Vec3b>(centers[i].x, centers[i].y) << endl;
        }


        if(centers.size()>0)
        {
            tracker.Update(centers);
            //cout << tracker.tracks.size()  << endl;

            for(size_t i=0;i<tracker.tracks.size();i++)
            {
                if(tracker.tracks[i]->trace.size()>1)
                {   
                    size_t m;
                    for(size_t j=0;j<tracker.tracks[i]->trace.size()-1;j++)
                    {
                        line(img,tracker.tracks[i]->trace[j],tracker.tracks[i]->trace[j+1],Colors[tracker.tracks[i]->track_id%9],2,CV_AA);
                        m=j;
                    }
                    Point p = tracker.tracks[i]->trace[m+1];
                    circle(img,p,3,Scalar(0,255,0),1,CV_AA);
                    //cout << roi.at<Vec3b>(p.x, p.y) << endl;
                    if(roi.at<Vec3b>(p.y, p.x) == Vec3b(255,255,255))
                    {
                        roi_sig=1;
                    }
                    //if(roi1.at<Vec3b>(p.y,p.x)== Vec3b(255,255,255))
                    //{
                        //roi_sig1=1;
                    //}
                }
            }
        }
        if(roi_sig == 1)
        {
            cout << "there are people in the area1"<< endl;
        }
        //if(roi_sig1==1)
        //{
            //cout<<"there are people in the area2" << endl;
        //}
        imshow("Video",img);

        k=waitKey(1);
        }
        delete detector;
        destroyAllWindows(); 
        return 0;
}
